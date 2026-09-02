/*
 openrouter_wingman.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "openrouter_wingman.h"

#include "../../../representations/json/nlohmann/json.hpp"

#include "../../../gear/string_utils.h"

namespace m8r {

using namespace std;

/*
 * cURL callback for writing data to string.
 */

size_t openrouterCurlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

/*
 * OpenRouter Wingman class implementation.
 */

OpenRouterWingman::OpenRouterWingman(const std::string& apiKey)
    : Wingman(WingmanLlmProviders::WINGMAN_PROVIDER_OPENROUTER),
      apiKey{apiKey},
      llmModels{},
      defaultLlmModel{"openai/gpt-3.5-turbo"},
      lastListModelsSucceeded{false}
{
    // never log the raw API key - debug logs get shared for troubleshooting
    MF_DEBUG(
        "OpenRouterWingman::OpenRouterWingman() apiKey configured: "
        << boolalpha << !this->apiKey.empty() << endl);

    listModels();
}

OpenRouterWingman::~OpenRouterWingman()
{
}

std::vector<std::string>& OpenRouterWingman::listModels()
{
    llmModels.clear();

    // try to fetch models from OpenRouter API
    try {
        listModelsHttpGet();
    } catch (...) {
        MF_DEBUG("OpenRouterWingman::listModels() failed to fetch from API, using defaults" << endl);
    }

    // if API call failed or returned no models, use defaults
    if (llmModels.empty()) {
        llmModels.push_back("openai/gpt-3.5-turbo");
        llmModels.push_back("openai/gpt-4");
        llmModels.push_back("anthropic/claude-3-haiku");
        llmModels.push_back("anthropic/claude-3-sonnet");
    }

    return llmModels;
}

void OpenRouterWingman::listModelsHttpGet()
{
    lastListModelsSucceeded = false;

    string url = string{DEFAULT_OPENROUTER_API_URL} + "/models";

    MF_DEBUG("OpenRouterWingman::listModelsHttpGet() url: " << url << endl);

#if !defined(__APPLE__) && !defined(_WIN32)
    CURL* curl = curl_easy_init();
    if (!curl) {
        return;
    }
#endif

    string responseString;

#if defined(_WIN32) || defined(__APPLE__)
    QNetworkAccessManager networkManager;

    QNetworkRequest request(QUrl(QString::fromStdString(url)));
    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json");
    request.setRawHeader(
        "Authorization",
        "Bearer " + QString::fromStdString(this->apiKey).toUtf8());

    QNetworkReply* reply = networkManager.get(request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    auto error = reply->error();
    if (error != QNetworkReply::NoError) {
        MF_DEBUG("OpenRouterWingman::listModelsHttpGet() error: " << reply->errorString().toStdString() << endl);
        reply->deleteLater();
        return;
    }

    QByteArray read = reply->readAll();
    responseString = QString{read}.toStdString();
    reply->deleteLater();
#else
    // CURL implementation
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, openrouterCurlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(
        headers,
        ("Authorization: Bearer " + this->apiKey).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        MF_DEBUG("OpenRouterWingman::listModelsHttpGet() error: " << curl_easy_strerror(res) << endl);
        return;
    }
#endif

    // parse JSON response
    nlohmann::json httpResponseJson;
    try {
        httpResponseJson = nlohmann::json::parse(responseString);
    } catch (...) {
        MF_DEBUG(
            "Error: unable to parse OpenRouter models JSON response:" << endl <<
            "'" << responseString << "'" << endl
        );
        return;
    }

    MF_DEBUG(
        "OpenRouterWingman::listModelsHttpGet() parsed response:" << endl
        << ">>>"
        << httpResponseJson.dump(4)
        << "<<<"
        << endl);

    // OpenRouter returns HTTP 200 with a JSON error body (e.g. bad API key), so
    // a successful parse alone doesn't mean success - a "data" array does
    if (httpResponseJson.contains("data")) {
        lastListModelsSucceeded = true;
        for (const auto& item : httpResponseJson["data"].items()) {
            if (item.value().contains("id")) {
                string modelId = item.value()["id"];
                llmModels.push_back(modelId);
                MF_DEBUG("  Added model: " << modelId << endl);
            }
        }
    }
}

/**
 * OpenRouter cURL GET request - uses OpenAI-compatible chat completions API.
 *
 * @see https://openrouter.ai/docs#requests
 */
void OpenRouterWingman::curlGet(CommandWingmanChat& command) {
#if !defined(__APPLE__) && !defined(_WIN32)
    CURL* curl = curl_easy_init();
    if (curl) {
#endif
        string escapedPrompt{command.prompt};
        replaceAll("\n", " ", escapedPrompt);
        replaceAll("\"", "\\\"", escapedPrompt);

        nlohmann::json messageSystemJSon{};
        messageSystemJSon["role"] = "system";
        messageSystemJSon["content"] = "You are a helpful assistant.";

        nlohmann::json messageUserJSon{};
        messageUserJSon["role"] = "user";
        messageUserJSon["content"] = escapedPrompt;

        nlohmann::json requestJSon;
        requestJSon["model"] = llmModel;
        requestJSon["messages"] = nlohmann::json::array(
            {
                messageSystemJSon,
                messageUserJSon,
            }
        );
        string requestJSonStr = requestJSon.dump(4);

        MF_DEBUG(
            "OpenRouterWingman::curlGet() promptJSon:" << endl
            << ">>>"
            << requestJSonStr
            << "<<<"
            << endl);

        string chatUrl = string{DEFAULT_OPENROUTER_API_URL} + "/chat/completions";

#if defined(_WIN32) || defined(__APPLE__)
        QNetworkAccessManager networkManager;

        QNetworkRequest request(QUrl(QString::fromStdString(chatUrl)));
        request.setHeader(
            QNetworkRequest::ContentTypeHeader,
            "application/json");
        request.setRawHeader(
            "Authorization",
            "Bearer " + QString::fromStdString(this->apiKey).toUtf8());

        QNetworkReply* reply = networkManager.post(
            request,
            requestJSonStr.c_str()
        );
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK;

        auto error = reply->error();
        if(error != QNetworkReply::NoError) {
            command.errorMessage =
                "Error: request to OpenRouter Wingman provider failed due a network error - " +
                reply->errorString().toStdString();
            MF_DEBUG(command.errorMessage << endl);
            command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
        }
        QByteArray read;
        if(command.status == m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK) {
            read = reply->readAll();

            if(read.isEmpty()) {
                command.errorMessage =
                    "Error: Request to OpenRouter Wingman provider failed - response is empty";
                MF_DEBUG(command.errorMessage << endl);
                command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            }
        }

        if(command.status == m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK) {
            QString qCommandResponse = QString{read};
            command.httpResponse = qCommandResponse.toStdString();
            command.errorMessage.clear();
            command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK;
            MF_DEBUG(
                "Successful OpenRouter Wingman provider response:" << endl <<
                "  '" << command.httpResponse << "'" << endl);
        }
        reply->deleteLater();
#else
        command.httpResponse.clear();
        curl_easy_setopt(
            curl, CURLOPT_URL,
            chatUrl.c_str());
        curl_easy_setopt(
            curl, CURLOPT_POSTFIELDS,
            requestJSonStr.c_str());
        curl_easy_setopt(
            curl, CURLOPT_WRITEFUNCTION,
            openrouterCurlWriteCallback);
        curl_easy_setopt(
            curl, CURLOPT_WRITEDATA,
            &command.httpResponse);

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(
            headers,
            ("Authorization: Bearer " + this->apiKey).c_str());
        headers = curl_slist_append(
            headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            command.status = WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            command.errorMessage = curl_easy_strerror(res);
        } else {
            command.status = WingmanStatusCode::WINGMAN_STATUS_CODE_OK;
        }
#endif

        // finish error handling (shared by QNetwork/CURL)
        if(command.status == WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR) {
            std::cerr <<
            "Error: Wingman OpenRouter cURL/QtNetwork request failed (error message/HTTP response):" << endl <<
             "  '" << command.errorMessage << "'" << endl <<
             "  '" << command.httpResponse << "'" << endl;

            command.httpResponse.clear();
            command.answerMarkdown.clear();
            command.answerTokens = 0;
            command.answerLlmModel = llmModel.size()>0? llmModel: defaultLlmModel;

            return;
        }

        // parse JSon response (OpenAI-compatible format)
        nlohmann::json httpResponseJSon;
        try {
            httpResponseJSon = nlohmann::json::parse(command.httpResponse);
        } catch (...) {
            MF_DEBUG(
                "Error: unable to parse OpenRouter JSon response:" << endl <<
                "'" << command.httpResponse << "'" << endl
            );

            command.status = WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            command.errorMessage = "Error: unable to parse OpenRouter JSon response: '" + command.httpResponse + "'";
            command.answerMarkdown.clear();
            command.answerTokens = 0;
            command.answerLlmModel = llmModel;

            return;
        }

        MF_DEBUG(
            "OpenRouterWingman::curlGet() parsed response:" << endl
            << ">>>"
            << httpResponseJSon.dump(4)
            << "<<<"
            << endl);

        MF_DEBUG("OpenRouterWingman::curlGet() fields:" << endl);
        if(httpResponseJSon.contains("model")) {
            httpResponseJSon["model"].get_to(command.answerLlmModel);
            MF_DEBUG("  model: " << command.answerLlmModel << endl);
        }
        if(httpResponseJSon.contains("usage")) {
            if(httpResponseJSon["usage"].contains("prompt_tokens")) {
                httpResponseJSon["usage"]["prompt_tokens"].get_to(command.promptTokens);
                MF_DEBUG("  prompt_tokens: " << command.promptTokens << endl);
            }
            if(httpResponseJSon["usage"].contains("completion_tokens")) {
                httpResponseJSon["usage"]["completion_tokens"].get_to(command.answerTokens);
                MF_DEBUG("  answer_tokens: " << command.answerTokens << endl);
            }
        }
        if(httpResponseJSon.contains("choices")
            && httpResponseJSon["choices"].size() > 0
        ) {
            auto choice = httpResponseJSon["choices"][0];
            if(choice.contains("message")
                && choice["message"].contains("content")
            ) {
                choice["message"]["content"].get_to(command.answerMarkdown);
                m8r::replaceAll(
                    "\n",
                    "<br/>",
                    command.answerMarkdown);
                MF_DEBUG("  answer (HTML): " << command.answerMarkdown << endl);
            }
            if(choice.contains("finish_reason")) {
                string statusStr{};
                choice["finish_reason"].get_to(statusStr);
                if(statusStr == "stop") {
                    command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK;
                } else {
                    command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
                    command.errorMessage.assign(
                        "OpenRouter API HTTP request failed with finish_reason: "
                        + statusStr);
                    command.answerMarkdown.clear();
                    command.answerTokens = 0;
                    command.answerLlmModel = llmModel;
                }
                MF_DEBUG("  status: " << command.status << endl);
            }
        } else {
            command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            command.answerMarkdown.clear();
            command.answerTokens = 0;
            command.answerLlmModel = llmModel;
            if(
                httpResponseJSon.contains("error")
                && httpResponseJSon["error"].contains("message")
            ) {
                httpResponseJSon["error"]["message"].get_to(command.errorMessage);
            } else {
                command.errorMessage.assign(
                    "No choices in the OpenRouter API HTTP response");
            }
        }
#if !defined(__APPLE__) && !defined(_WIN32)
    }
    else {
        command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
        command.errorMessage.assign(
            "OpenRouter API HTTP request failed: unable to initialize cURL");
    }
#endif
}

void OpenRouterWingman::chat(CommandWingmanChat& command) {
    MF_DEBUG("OpenRouterWingman::chat() prompt:" << endl << command.prompt << endl);

    curlGet(command);

    MF_DEBUG("OpenRouterWingman::chat() answer:" << endl << command.answerMarkdown << endl);
}

} // m8r namespace
