/*
 openrouter_wingman.h     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_OPENROUTER_WINGMAN_H
#define M8R_OPENROUTER_WINGMAN_H

#include <string>
#include <stdexcept>

// HTTP client: CURL on Linux, Qt Network on macOS and Win
#if defined(_WIN32) || defined(__APPLE__)
  #include <QtNetwork>
#else
  #include "curl/curl.h"
#endif

#include "wingman.h"

namespace m8r {

/**
 * OpenRouter Wingman implementation.
 *
 * OpenRouter is an OpenAI-compatible API gateway that routes requests
 * to various LLM providers. API endpoint: https://openrouter.ai/api/v1
 */
class OpenRouterWingman: Wingman
{
private:
    // API key needed to access OpenRouter API endpoint
    std::string apiKey;
    // names of LLM models provided by the OpenRouter API endpoint
    std::vector<std::string> llmModels;
    // name of the default LLM model
    std::string defaultLlmModel;

    void curlGet(CommandWingmanChat& command);
    void listModelsHttpGet();

public:
    explicit OpenRouterWingman(const std::string& apiKey);
    OpenRouterWingman(const OpenRouterWingman&) = delete;
    OpenRouterWingman(const OpenRouterWingman&&) = delete;
    OpenRouterWingman& operator =(const OpenRouterWingman&) = delete;
    OpenRouterWingman& operator =(const OpenRouterWingman&&) = delete;
    ~OpenRouterWingman() override;

    /**
     * @brief List (and cache) LLM model names.
     */
    virtual std::vector<std::string>& listModels() override;

    /**
     * @brief Chat with configured LLM model.
     */
    virtual void chat(CommandWingmanChat& command) override;

    /**
     * @brief Get embeddings (not supported by OpenRouter Wingman).
     */
    virtual void embeddings(CommandWingmanEmbeddings& command) override {
        UNUSED_ARG(command);
        throw std::runtime_error("OpenRouter Wingman does not support embeddings");
    }
};

}
#endif // M8R_OPENROUTER_WINGMAN_H
