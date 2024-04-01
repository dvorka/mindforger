/*
 markdown_benchmark.cpp     MindForger markdown test

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include "../test_utils.h"
#include "../../../src/mind/ai/llm/wingman.h"
#include "../../../src/representations/json/nlohmann/json.hpp"
#include "../../../src/gear/string_utils.h"

using namespace std;

TEST(JSonTestCase, SerializeOpenAiRequest)
{
    // GIVEN
    string escapedPrompt{"Who is the godfather of C++?"};
    string gptModel{"gpt-3.5-turbo"};

    nlohmann::json messageSystemJSon{};
    messageSystemJSon["role"] = "system"; // system (instruct GPT who it is), user (user prompts), assistant (GPT answers)
    messageSystemJSon["content"] = "You are a helpful assistant.";
    // ... more messages like above (with chat history) can be created to provide context
    nlohmann::json messageUserJSon{};
    messageUserJSon["role"] = "user";
    messageUserJSon["content"] = escapedPrompt;

    nlohmann::json requestJSon;
    requestJSon["model"] = gptModel;
    requestJSon["messages"] = nlohmann::json::array(
        {
            messageSystemJSon,
            messageUserJSon,
        }
    );

    // WHEN
    MF_DEBUG("Request JSON: " << requestJSon.dump(4) << endl);
    string requestJSonStr = requestJSon.dump(); // add 4 for indentation spaces

    // THEN
    EXPECT_EQ(
        "{\"messages\":[{\"content\":\"You are a helpful assistant.\",\"role\":\"system\"},{\"content\":\"Who is the godfather of C++?\",\"role\":\"user\"}],\"model\":\"gpt-3.5-turbo\"}",
        requestJSonStr);
}

TEST(JSonTestCase, ParseOpenAiResponse)
{
    // GIVEN
    string httpResponseStr{"{\"id\": \"chatcmpl-8gspbsufrxF42A6JfaiwuxoitQ1fT\",\"object\": \"chat.completion\",\"created\": 1705231239,\"model\": \"gpt-3.5-turbo-0613\",\"choices\": [{\"index\": 0,\"message\": {\"role\": \"assistant\",\"content\": \"LLM answer:\\n42\"},\"logprobs\": null,\"finish_reason\": \"stop\"}],\"usage\": {\"prompt_tokens\": 26,\"completion_tokens\": 491,\"total_tokens\": 517},\"system_fingerprint\": null}"};

    // WHEN
    auto httpResponseJSon = nlohmann::json::parse(httpResponseStr);

    m8r::WingmanStatusCode status{
        m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK
    };
    string errorMessage{};
    string answerGptModel{};
    int promptTokens{};
    int answerTokens{};
    string answerMarkdown{};

    MF_DEBUG(
        "OpenAiWingman::curlGet() parsed response:" << endl
        << ">>>"
        << httpResponseJSon.dump(4)
        << "<<<"
        << endl);

    MF_DEBUG("OpenAiWingman::curlGet() fields:" << endl);
    if(httpResponseJSon.contains("model")) {
        httpResponseJSon["model"].get_to(answerGptModel);
        MF_DEBUG("  model: " << answerGptModel << endl);
    }
    if(httpResponseJSon.contains("usage")) {
        if(httpResponseJSon["usage"].contains("prompt_tokens")) {
            httpResponseJSon["usage"]["prompt_tokens"].get_to(promptTokens);
            MF_DEBUG("  prompt_tokens: " << promptTokens << endl);
        }
        if(httpResponseJSon["usage"].contains("completion_tokens")) {
            httpResponseJSon["usage"]["completion_tokens"].get_to(answerTokens);
            MF_DEBUG("  answer_tokens: " << answerTokens << endl);
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
            choice["message"]["content"].get_to(answerMarkdown);
            // TODO ask LLM for HTML formatted response
            m8r::replaceAll(
                "\n",
                "<br/>",
                answerMarkdown);
            MF_DEBUG("  answer (HTML): " << answerMarkdown << endl);
        }
        if(choice.contains("finish_reason")) {
            string statusStr{};
            choice["finish_reason"].get_to(statusStr);
            if(statusStr == "stop") {
                status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK;
            } else {
                status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
                errorMessage.assign(
                    "OpenAI API HTTP required failed with finish_reason: "
                    + statusStr);
            }
            MF_DEBUG("  status: " << status << endl);
        }
    } else {
        status = m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_ERROR;
        errorMessage.assign(
            "No choices in the OpenAI API HTTP response");
    }

    // THEN
    EXPECT_EQ(
        "",
        errorMessage);
    EXPECT_EQ(
        "gpt-3.5-turbo-0613",
        answerGptModel);
    EXPECT_EQ(
        "LLM answer:<br/>42",
        answerMarkdown);
    EXPECT_EQ(
        m8r::WingmanStatusCode::WINGMAN_STATUS_CODE_OK,
        status);
    EXPECT_EQ(
        26,
        promptTokens);
    EXPECT_EQ(
        491,
        answerTokens);
}
