/*
 ollama_wingman.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "ollama_wingman.h"

#include "../../../representations/json/nlohmann/json.hpp"

#include "../../../gear/string_utils.h"

namespace m8r {

using namespace std;

/*
 * cURL callback for writing data to string.
 */

size_t ollamaCurlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

/*
 * Ollama Wingman class implementation.
 */

OllamaWingman::OllamaWingman(const string& url)
    : Wingman(WingmanLlmProviders::WINGMAN_PROVIDER_OLLAMA),
      url{url},
      llmModels{}
{
}

OllamaWingman::~OllamaWingman()
{
}

void OllamaWingman::listModelsHttpGet() {
    string url = 
        this->url +
        (stringEndsWith(this->url, "/")?"":"/") + 
        "api/tags";

    MF_DEBUG("    ollama: curlGet() url: " << url << endl);
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

        std::string responseString;
        std::string headerString;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ollamaCurlWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerString);

        // perform GET request
        CURLcode res = curl_easy_perform(curl);
        MF_DEBUG("    ollama: curlGet() response: '" << responseString << "'" << endl);

        // clean up
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            MF_DEBUG("    ollama: curlGet() error: " << curl_easy_strerror(res) << endl);
            //command.status = WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            //command.errorMessage = curl_easy_strerror(res);
        } else {
            MF_DEBUG("    ollama: curlGet() OK" << endl);
            //command.status = WingmanStatusCode::WINGMAN_STATUS_CODE_OK;
        }

        // curl_global_cleanup();
        curl = NULL;

        // parse JSon
        /*
        {
            "models": [
                {
                    "name": "llama2:latest",
                    "model": "llama2:latest",
                    "modified_at": "2024-03-02T09:59:31.011931063+01:00",
                    "size": 3826793677,
                    "digest": "78e26419b4469263f75331927a00a0284ef6544c1975b826b15abdaef17bb962",
                    "details": {
                        "parent_model": "",
                        "format": "gguf",
                        "family": "llama",
                        "families": [
                        "llama"
                        ],
                        "parameter_size": "7B",
                        "quantization_level": "Q4_0"
                    }
                },
            ...
        }
        */
       // parse json
       nlohmann::json httpResponseJSon;
       try {
           httpResponseJSon = nlohmann::json::parse(responseString);
       } catch (...) {
           // catch ALL exceptions
           MF_DEBUG(
               "Error: unable to parse ollama JSon response:" << endl <<
                "'" << responseString << "'" << endl
           );
           // TODO: create command so that I can transfer all the fields
           //command.status = WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
           //command.errorMessage = "Error: unable to parse ollama JSon response: '" + command.httpResponse + "'";
           //command.answerMarkdown.clear();
           //command.answerTokens = 0;
           //command.answerLlmModel = llmModel;

           return;
        }

        MF_DEBUG(
            "OllamaWingman::curlGet() parsed response:" << endl
            << ">>>"
            << httpResponseJSon.dump(4)
            << "<<<"
            << endl);

        MF_DEBUG("OllamaWingman::curlGet() fields:" << endl);
        if(httpResponseJSon.contains("models")) {
            for(const auto& item : httpResponseJSon["models"].items()) {
                MF_DEBUG("  item: " << item.key() << endl);
                if(item.value().contains("name")) {
                    MF_DEBUG("    name: " << item.value()["name"] << endl);
                    string llmModelName{item.value()["name"]};
                    // add model to list
                    this->llmModels.push_back(llmModel);
                }
            }
        }
    }
    MF_DEBUG("    DONE ollama: curlGet() url: " << url << endl); 
}

/**
 * cURL / Qt Network POST request.
 *
 * @see https://github.com/ollama/ollama/blob/main/docs/api.md#generate-a-completion
 * @see https://github.com/ollama/ollama/blob/main/docs/api.md#list-local-models
 * @see https://ollama.com/library/llama2
 * @see https://json.nlohmann.me/
 */
void OllamaWingman::chatHttpPost(CommandWingmanChat& command) {
#if !defined(__APPLE__) && !defined(_WIN32)
    CURL* curl = curl_easy_init();
    if (curl) {
#endif
        string escapedPrompt{command.prompt};
        replaceAll("\n", " ", escapedPrompt);
        replaceAll("\"", "\\\"", escapedPrompt);

        /*
        ollama API JSon request example - chat vs. generate answer:

        curl -X POST http://localhost:11434/api/generate -d '{
            "model": "llama2",
            "prompt":"Why is the sky blue?",
            "stream": false
        }'

        */
        nlohmann::json requestJSon;
        requestJSon["model"] = llmModel;
        requestJSon["prompt"] = escapedPrompt;
        requestJSon["stream"] = false;

        string requestJSonStr = requestJSon.dump(4);

        string url = 
            this->url +
            (stringEndsWith(this->url, "/")?"":"/") + 
            "api/generate";

        MF_DEBUG(
            "OllamaWingman::curlPost() promptJSon: " << url << endl
            << ">>>"
            << requestJSonStr
            << "<<<"
            << endl);

#if defined(_WIN32) || defined(__APPLE__)
        QNetworkAccessManager networkManager;

        QNetworkRequest request(QUrl(QString::fromStdString(url)));
        request.setHeader(
            QNetworkRequest::ContentTypeHeader,
            "application/json");

        QNetworkReply* reply = networkManager.post(
            request,
            requestJSonStr.c_str()
        );
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        reply->deleteLater();

        command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK;

        // response: error handling
        auto error = reply->error();
        if(error != QNetworkReply::NoError) {
            command.errorMessage =
                "Error: request to ollama Wingman provider failed due a network error - " +
                reply->errorString().toStdString();
            MF_DEBUG(command.errorMessage << endl);
            command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
        }
        QByteArray read;
        if(command.status == m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK) {
            read = reply->readAll();

            if(read.isEmpty()) {
                command.errorMessage =
                    "Error: Request to ollama Wingman provider failed - response is empty'";
                MF_DEBUG(command.errorMessage << endl);
                command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            }
        }

        // response: successful response processing
        if(command.status == m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK) {
            QString qCommandResponse = QString{read};
            command.httpResponse = qCommandResponse.toStdString();
            command.errorMessage.clear();
            command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK;
            MF_DEBUG(
                "Successful ollama Wingman provider response:" << endl <<
                "  '" << command.httpResponse << "'" << endl);
        }
#else
        // set up cURL options: POST
        command.httpResponse.clear();
        curl_easy_setopt(
            curl, CURLOPT_URL,
            url.c_str());
        curl_easy_setopt(
            curl, CURLOPT_POSTFIELDS,
            requestJSonStr.c_str());
        curl_easy_setopt(
            curl, CURLOPT_WRITEFUNCTION,
            ollamaCurlWriteCallback);
        curl_easy_setopt(
            curl, CURLOPT_WRITEDATA,
            &command.httpResponse);

        // perform POST request
        CURLcode res = curl_easy_perform(curl);

        // clean up
        curl_easy_cleanup(curl);

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
            "Error: Wingman ollama cURL/QtNetwork request failed (error message/HTTP response):" << endl <<
             "  '" << command.errorMessage << "'" << endl <<
             "  '" << command.httpResponse << "'" << endl;

            command.httpResponse.clear();
            command.answerMarkdown.clear();
            command.answerTokens = 0;
            command.answerLlmModel = llmModel;

            return;
        }

        // parse JSon
        /*
        ollama API JSon response example:

        {
            "model": "llama2",
            "created_at": "2023-08-04T19:22:45.499127Z",
            "response": "The sky is blue because it is the color of the sky.",
            "done": true,
            "context": [1, 2, 3],
            "total_duration": 5043500667,
            "load_duration": 5025959,
            "prompt_eval_count": 26,
            "prompt_eval_duration": 325953000,
            "eval_count": 290,
            "eval_duration": 4709213000
        }

        */

        // parse response string to JSon object
        nlohmann::json httpResponseJSon;
        try {
            httpResponseJSon = nlohmann::json::parse(command.httpResponse);
        } catch (...) {
            // catch ALL exceptions
            MF_DEBUG(
                "Error: unable to parse ollama JSon response:" << endl <<
                "'" << command.httpResponse << "'" << endl
            );

            command.status = WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            command.errorMessage = "Error: unable to parse ollama JSon response: '" + command.httpResponse + "'";
            command.answerMarkdown.clear();
            command.answerTokens = 0;
            command.answerLlmModel = llmModel;

            return;
        }

        MF_DEBUG(
            "OllamaWingman::curlPost() parsed response:" << endl
            << ">>>"
            << httpResponseJSon.dump(4)
            << "<<<"
            << endl);

        MF_DEBUG("OllamaWingman::curlPost() fields:" << endl);
        if(httpResponseJSon.contains("model")) {
            httpResponseJSon["model"].get_to(command.answerLlmModel);
            MF_DEBUG("  model: " << command.answerLlmModel << endl);
        }
        if(httpResponseJSon.contains("response")) {
            httpResponseJSon["response"].get_to(command.answerMarkdown);
            // TODO ask LLM for HTML formatted response
            m8r::replaceAll(
                "\n",
                "<br/>",
                command.answerMarkdown);
            MF_DEBUG("  response (HTML): " << command.answerMarkdown << endl);
        } else {
            command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            command.errorMessage.assign(
                "No response in the ollama API HTTP response");
            command.answerMarkdown.clear();
            command.answerTokens = 0;
            command.answerLlmModel = llmModel;
        }
        if(httpResponseJSon.contains("prompt_eval_count")) {
            httpResponseJSon["prompt_eval_count"].get_to(command.promptTokens);
            MF_DEBUG("  prompt_eval_count: " << command.promptTokens << endl);
        }
        if(httpResponseJSon.contains("eval_count")) {
            httpResponseJSon["eval_count"].get_to(command.answerTokens);
            MF_DEBUG("  eval_count: " << command.answerTokens << endl);
        }
#if !defined(__APPLE__) && !defined(_WIN32)
    }
    else {
        command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
        command.errorMessage.assign(
            "ollama API HTTP request to chat failed: unable to initialize cURL");
    }
#endif
}

void OllamaWingman::embeddingsHttpPost(CommandWingmanEmbeddings& command) {
    // empty prompt - empty vector
    if(command.prompt.empty()) {
        command.httpResponse.clear();
        command.status = WingmanStatusCode::WINGMAN_STATUS_CODE_OK;
        command.errorMessage.clear();
        command.answerLlmModel = llmModel;
        command.answerEmbeddings.clear();

        return;
    }

#if !defined(__APPLE__) && !defined(_WIN32)
    CURL* curl = curl_easy_init();
    if (curl) {
#endif
        // TODO: remove escape if not needed
        //string escapedPrompt{command.prompt};
        //replaceAll("\n", " ", escapedPrompt);
        //replaceAll("\"", "\\\"", escapedPrompt);

        /*
        ollama API JSon request example:

        curl -X POST http://localhost:11434/api/embeddings -d '{
            "model": "llama2",
            "prompt":"Why is the sky blue?",
        }'

        */
        nlohmann::json requestJSon;
        requestJSon["model"] = llmModel;
        requestJSon["prompt"] = command.prompt;

        string requestJSonStr = requestJSon.dump(4);

        string url = 
            this->url +
            (stringEndsWith(this->url, "/")?"":"/") + 
            "api/embeddings";

        MF_DEBUG(
            "OllamaWingman::curlPost() promptJSon: '" << url << "'" << endl
            << ">>>"
            << requestJSonStr
            << "<<<"
            << endl);

#if defined(_WIN32) || defined(__APPLE__)
        QNetworkAccessManager networkManager;

        QNetworkRequest request(QUrl(QString::fromStdString(url)));
        request.setHeader(
            QNetworkRequest::ContentTypeHeader,
            "application/json");

        QNetworkReply* reply = networkManager.post(
            request,
            requestJSonStr.c_str()
        );
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        reply->deleteLater();

        command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK;

        // response: error handling
        auto error = reply->error();
        if(error != QNetworkReply::NoError) {
            command.errorMessage =
                "Error: request to ollama Wingman provider failed due a network error - " +
                reply->errorString().toStdString();
            MF_DEBUG(command.errorMessage << endl);
            command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
        }
        QByteArray read;
        if(command.status == m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK) {
            read = reply->readAll();

            if(read.isEmpty()) {
                command.errorMessage =
                    "Error: Request to ollama Wingman provider failed - response is empty'";
                MF_DEBUG(command.errorMessage << endl);
                command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            }
        }

        // response: successful response processing
        if(command.status == m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK) {
            QString qCommandResponse = QString{read};
            command.httpResponse = qCommandResponse.toStdString();
            command.errorMessage.clear();
            command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK;
            MF_DEBUG(
                "Successful ollama Wingman provider response:" << endl <<
                "  '" << command.httpResponse << "'" << endl);
        }
#else
        // set up cURL options: POST
        command.httpResponse.clear();
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, 1);
        curl_easy_setopt(
            curl, CURLOPT_URL,
            url.c_str());
        curl_easy_setopt(
            curl, CURLOPT_POSTFIELDS,
            requestJSonStr.c_str());
        curl_easy_setopt(
            curl, CURLOPT_WRITEFUNCTION,
            ollamaCurlWriteCallback);
        curl_easy_setopt(
            curl, CURLOPT_WRITEDATA,
            &command.httpResponse);

        // perform POST request
        CURLcode res = curl_easy_perform(curl);

        // clean up
        curl_easy_cleanup(curl);

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
            "Error: Wingman ollama cURL/QtNetwork request to get embeddings "
            "failed (error message/HTTP response):" << endl <<
            "  '" << command.errorMessage << "'" << endl <<
            "  '" << command.httpResponse << "'" << endl;

            command.answerLlmModel = llmModel;
            command.answerEmbeddings.clear();

            return;
        }

        // parse JSon
        /*
        ollama API JSon response example:

        {
            "embedding": [
                0.8623529076576233,
                ...
                0.8623529076576233
            ]
        }

        */

        // parse response string to JSon object
        nlohmann::json httpResponseJSon;
        try {
            httpResponseJSon = nlohmann::json::parse(command.httpResponse);
        } catch (...) {
            // catch ALL exceptions
            MF_DEBUG(
                "Error: unable to parse ollama JSon response:" << endl <<
                "'" << command.httpResponse << "'" << endl
            );

            command.status = WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            command.errorMessage = "Error: unable to parse ollama JSon response: '" + command.httpResponse + "'";
            command.answerLlmModel = llmModel;
            command.answerEmbeddings.clear();

            return;
        }

        /*
        MF_DEBUG(
            "OllamaWingman::curlPost() parsed response:" << endl
            << ">>>"
            << httpResponseJSon.dump(4)
            << "<<<"
            << endl);
        */

        // MF_DEBUG("OllamaWingman::curlPost() fields:" << endl);
        if(httpResponseJSon.contains("embedding")) {
            for(const auto& item : httpResponseJSon["embedding"].items()) {
                // MF_DEBUG("  item #" << item.key() << ": " << item.value() << endl);
                command.answerEmbeddings.push_back(item.value());
            }
            // MF_DEBUG("  EMBEDDINGS vector: " << command.answerEmbeddings.size() << endl);
        }
#ifdef MF_DEBUG
        else {
            MF_DEBUG("  EMBEDDINGS: 'embedding' key not found in" << endl);
        }
#endif

#if !defined(__APPLE__) && !defined(_WIN32)
    }
    else {
        command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
        command.errorMessage.assign(
            "ollama API HTTP request to get embeddings failed: unable to initialize cURL");
    }
#endif
}

std::vector<std::string>& OllamaWingman::listModels()
{
    listModelsHttpGet();

    return this->llmModels;
}

void OllamaWingman::chat(CommandWingmanChat& command) {
    chatHttpPost(command);
}

void OllamaWingman::embeddings(CommandWingmanEmbeddings& command) {
    embeddingsHttpPost(command);
}

} // m8r namespace
