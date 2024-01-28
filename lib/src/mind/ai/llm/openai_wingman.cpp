/*
 openai_wingman.cpp     MindForger thinking notebook

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
#include "openai_wingman.h"

#include "../../../representations/json/nlohmann/json.hpp"

#include "../../../gear/string_utils.h"

namespace m8r {

using namespace std;

/*
 * cURL callback for writing data to string.
 */

size_t openaiCurlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

/*
 * OpenAi Wingman class implementation.
 */

OpenAiWingman::OpenAiWingman(
    const string& apiKey,
    const std::string& llmModel
)
    : Wingman(WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI),
      apiKey{apiKey},
      llmModel{llmModel}
{
    MF_DEBUG("OpenAiWingman::OpenAiWingman() apiKey: " << apiKey << endl);
}

OpenAiWingman::~OpenAiWingman()
{
}

/**
 * OpenAI cURL GET request.
 *
 * @see https://platform.openai.com/docs/guides/text-generation/chat-completions-api?lang=curl
 * @see https://github.com/nlohmann/json?tab=readme-ov-file
 * @see https://json.nlohmann.me/
 */
void OpenAiWingman::curlGet(CommandWingmanChat& command) {
    CURL* curl = curl_easy_init();
    if (curl) {
        string escapedPrompt{command.prompt};
        replaceAll("\n", " ", escapedPrompt);
        replaceAll("\"", "\\\"", escapedPrompt);

        /*
        OpenAI API JSon request example (see unit test):

        ...

        */
        nlohmann::json messageSystemJSon{};
        messageSystemJSon["role"] = "system"; // system (instruct GPT who it is), user (user prompts), assistant (GPT answers)
        messageSystemJSon["content"] = "You are a helpful assistant.";
        // ... more messages like above (with chat history) can be created to provide context
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
            "OpenAiWingman::curlGet() promptJSon:" << endl
            << ">>>"
            << requestJSonStr
            << "<<<"
            << endl);

        // set up cURL options
        command.httpResponse.clear();
        curl_easy_setopt(
            curl, CURLOPT_URL,
            "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(
            curl, CURLOPT_POSTFIELDS,
            requestJSonStr.c_str());
        curl_easy_setopt(
            curl, CURLOPT_WRITEFUNCTION,
            openaiCurlWriteCallback);
        curl_easy_setopt(
            curl, CURLOPT_WRITEDATA,
            &command.httpResponse);

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // perform the request
        CURLcode res = curl_easy_perform(curl);

        // clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);

        if (res != CURLE_OK) {
            command.status = WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            command.errorMessage = curl_easy_strerror(res);
            std::cerr << "Error: Wingman OpenAI cURL request failed: " << command.errorMessage << std::endl;

            command.httpResponse.clear();
            command.answerHtml.clear();
            command.answerTokens = 0;
            command.answerLlmModel = llmModel;

            return;
        }

        // parse JSon
        /*
        OpenAI API JSon response example:
        {
            "id": "chatcmpl-8gspbsufrxF42A6JfaiwuxoitQ1fT",
            "object": "chat.completion",
            "created": 1705231239,
            "model": "gpt-3.5-turbo-0613",
            "choices": [
                {
                "index": 0,
                "message": {
                    "role": "assistant",
                    "content": "...LLM answer...",
                },
                "logprobs": null,
                "finish_reason": "stop"
                }
            ],
            "usage": {
                "prompt_tokens": 26,
                "completion_tokens": 491,
                "total_tokens": 517
            },
            "system_fingerprint": null
        }
        */
        auto httpResponseJSon = nlohmann::json::parse(command.httpResponse);

        MF_DEBUG(
            "OpenAiWingman::curlGet() parsed response:" << endl
            << ">>>"
            << httpResponseJSon.dump(4)
            << "<<<"
            << endl);

        MF_DEBUG("OpenAiWingman::curlGet() fields:" << endl);
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
            // TODO get the last choice rather than the first one
            auto choice = httpResponseJSon["choices"][0];
            if(choice.contains("message")
                && choice["message"].contains("content")
            ) {
                choice["message"]["content"].get_to(command.answerHtml);
                // TODO ask GPT for HTML formatted response
                m8r::replaceAll(
                    "\n",
                    "<br/>",
                    command.answerHtml);
                MF_DEBUG("  answer (HTML): " << command.answerHtml << endl);
            }
            if(choice.contains("finish_reason")) {
                string statusStr{};
                choice["finish_reason"].get_to(statusStr);
                if(statusStr == "stop") {
                    command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK;
                } else {
                    command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
                    command.errorMessage.assign(
                        "OpenAI API HTTP required failed with finish_reason: "
                        + statusStr);
                }
                MF_DEBUG("  status: " << command.status << endl);
            }
        } else {
            command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
            if(
                httpResponseJSon.contains("error")
                && httpResponseJSon["error"].contains("message")
            ) {
                httpResponseJSon["error"]["message"].get_to(command.errorMessage);
            } else {
                command.errorMessage.assign(
                    "No choices in the OpenAI API HTTP response");
            }
        }
    } else {
        command.status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
        command.errorMessage.assign(
            "OpenAI API HTTP request failed: unable to initialize cURL");
    }
}

void OpenAiWingman::chat(CommandWingmanChat& command) {
    MF_DEBUG("OpenAiWingman::chat() prompt:" << endl << command.prompt << endl);

    curlGet(command);

    MF_DEBUG("OpenAiWingman::chat() answer:" << endl << command.answerHtml << endl);
}

} // m8r namespace
