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

#include "curl/curl.h"

#include "wingman.h"

namespace m8r {

/**
 * OpenAI Wingman implementation.
 */
class OpenAiWingman: Wingman
{
private:
    std::string apiKey;

    void curlGet(
        const std::string& prompt,
        std::string& response);

public:
    explicit OpenAiWingman();
    OpenAiWingman(const OpenAiWingman&) = delete;
    OpenAiWingman(const OpenAiWingman&&) = delete;
    OpenAiWingman& operator =(const OpenAiWingman&) = delete;
    OpenAiWingman& operator =(const OpenAiWingman&&) = delete;
    ~OpenAiWingman();

    virtual void chat(const std::string& prompt, std::string& answer) override;
};

}
#endif // M8R_OPENAI_WINGMAN_H
