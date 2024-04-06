/*
 openai_wingman.h     MindForger thinking notebook

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
#ifndef M8R_OPENAI_WINGMAN_H
#define M8R_OPENAI_WINGMAN_H

#include <string>

// HTTP client: CURL on Linux, Qt Network on macOS and Win
#if defined(_WIN32) || defined(__APPLE__)
  #include <QtNetwork>
#else
  #include "curl/curl.h"
#endif

#include "../../../config/configuration.h"

#include "wingman.h"

namespace m8r {

/**
 * OpenAI Wingman implementation.
 */
class OpenAiWingman: Wingman
{
public:
    static const std::string LLM_MODEL_OPENAI_GPT35;
    static const std::string LLM_MODEL_OPENAI_GPT4;

private:
    // API key needed to access OpenAI API endpoint - read from config
    Configuration& config;
    // Names of LLM models provided by the OpenAI API endpoint
    std::vector<std::string> llmModels;
    // Name of the LLM model which is used by Wingman - must be one of llmModels ^
    std::string defaultLlmModel;

    void curlGet(CommandWingmanChat& command);

public:
    explicit OpenAiWingman();
    OpenAiWingman(const OpenAiWingman&) = delete;
    OpenAiWingman(const OpenAiWingman&&) = delete;
    OpenAiWingman& operator =(const OpenAiWingman&) = delete;
    OpenAiWingman& operator =(const OpenAiWingman&&) = delete;
    ~OpenAiWingman() override;

    /**
     * @brief List (and cache) LLM model names
     */
    virtual std::vector<std::string>& listModels() override;

    /**
     * @brief Chat with configured LLM model.
     */
    virtual void chat(CommandWingmanChat& command) override;

    /**
     * @brief Get embeddings from configured LLM model.
     */
    virtual void embeddings(CommandWingmanEmbeddings& command) override {
        UNUSED_ARG(command);
        throw std::runtime_error("OpenAI Wingman does not support embeddings");
    }
};

}
#endif // M8R_OPENAI_WINGMAN_H
