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
    std::string apiKey;
    std::vector<std::string> llmModels;
    std::string defaultLlmModel;

    void curlGet(CommandWingmanChat& command);

public:
    explicit OpenAiWingman(const std::string& apiKey);
    OpenAiWingman(const OpenAiWingman&) = delete;
    OpenAiWingman(const OpenAiWingman&&) = delete;
    OpenAiWingman& operator =(const OpenAiWingman&) = delete;
    OpenAiWingman& operator =(const OpenAiWingman&&) = delete;
    ~OpenAiWingman() override;

    virtual std::vector<std::string>& listModels() override;
    virtual void chat(CommandWingmanChat& command) override;
};

}
#endif // M8R_OPENAI_WINGMAN_H
