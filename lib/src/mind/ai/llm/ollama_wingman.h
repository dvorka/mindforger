/*
 ollama_wingman.h     MindForger thinking notebook

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
#ifndef M8R_OLLAMA_WINGMAN_H
#define M8R_OLLAMA_WINGMAN_H

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
 * ollama Wingman implementation.
 *
 * @see: https://ollama.com
 * @see: https://ollama.com/library/llama2
 * @see: https://ollama.com/library/mistral
 */
class OllamaWingman: Wingman
{
private:
    std::string url;
    std::vector<std::string> llmModels;

    void curlListModels();
    void curlGet(CommandWingmanChat& command);

public:
    explicit OllamaWingman(const std::string& url);
    OllamaWingman(const OllamaWingman&) = delete;
    OllamaWingman(const OllamaWingman&&) = delete;
    OllamaWingman& operator =(const OllamaWingman&) = delete;
    OllamaWingman& operator =(const OllamaWingman&&) = delete;
    ~OllamaWingman() override;

    virtual std::vector<std::string>& listModels() override;
    virtual void chat(CommandWingmanChat& command) override;
};

}
#endif // M8R_OLLAMA_WINGMAN_H
