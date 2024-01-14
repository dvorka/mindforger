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

OpenAiWingman::OpenAiWingman()
    : Wingman(WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI),
    apiKey{}
{
    const char* apiKeyEnv = std::getenv("OPENAI_API_KEY");
    if (!apiKeyEnv) {
        std::cerr << "OpenAI API key not found in the environment variable OPENAI_API_KEY." << std::endl;
        // TODO open FE dialog and ask for entering the key
        throw std::runtime_error(
            "OpenAI API key not found in the environment variable OPENAI_API_KEY.");
    }
    this->apiKey = string(apiKeyEnv);
}

OpenAiWingman::~OpenAiWingman()
{
}

/**
 * OpenAI cURL GET request.
 *
 * @see https://platform.openai.com/docs/guides/text-generation/chat-completions-api?lang=curl
 */
void OpenAiWingman::curlGet(
    const string& prompt,
    string& response
) {
    CURL* curl = curl_easy_init();
    if (curl) {
        // TODO get GPT model from the configuration
        string gptModel{"gpt-3.5-turbo"};

        string escapedPrompt{prompt};
        replaceAll("\n", " ", escapedPrompt);
        replaceAll("\"", "\\\"", escapedPrompt);

        string promptJSon{
            "{"
            "  \"model\": \"" + gptModel + "\","
            "  \"messages\": ["
            "    {"
            "      \"role\": \"system\","
            "      \"content\": \"You are a helpful assistant.\""
            "    },"
            // TODO conversation history as additional context
            //"    {"
            //"      \"role\": \"user\","
            //"      \"content\": \"Who won the world series in 2020?\""
            //"    },"
            //"    {"
            //"      \"role\": \"assistant\","
            //"      \"content\": \"The Los Angeles Dodgers won the World Series in 2020.\""
            //"    },"
            "    {"
            "      \"role\": \"user\","
            "      \"content\": \"" + escapedPrompt + "\""
            "    }"
            "  ]"
            "}"
        };
        MF_DEBUG(
            "OpenAiWingman::curlGet() promptJSon:" << endl
            << ">>>"
            << promptJSon
            << "<<<"
            << endl);
        // TODO remove
        // TODO response = promptJSon;

        // set up cURL options
        curl_easy_setopt(
            curl, CURLOPT_URL,
            "https://api.openai.com/v1/chat/completions");
        curl_easy_setopt(
            curl, CURLOPT_POSTFIELDS,
            promptJSon.c_str());
        curl_easy_setopt(
            curl, CURLOPT_WRITEFUNCTION,
            openaiCurlWriteCallback);
        curl_easy_setopt(
            curl, CURLOPT_WRITEDATA,
            &response);

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
            std::cerr << "cURL request failed: " << curl_easy_strerror(res) << std::endl;
            response.clear();
        }
        // TODO report error code to show RED in chat

        // parse JSon
        // TODO import to json namespace
        // TODO auto responseJSon = nlohmann::json::parse(R"({"happy": true, "pi": 3.141})");
        auto responseJSon = nlohmann::json::parse(response);
        MF_DEBUG(
            "OpenAiWingman::curlGet() responseJSon:" << endl
            << ">>>"
            << responseJSon.dump(4)
            << "<<<"
            << endl);
    }
}

void OpenAiWingman::chat(const std::string& prompt, std::string& answer)
{
    MF_DEBUG("OpenAiWingman::summarize() prompt:" << endl << prompt << endl);

    curlGet(prompt, answer);

    MF_DEBUG("OpenAiWingman::summarize() summary:" << endl << answer << endl);
}

} // m8r namespace
