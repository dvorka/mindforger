/*
 wingman.h     MindForger thinking notebook

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
#ifndef M8R_WINGMAN_H
#define M8R_WINGMAN_H

#include <string>

#include "../../../debug.h"

namespace m8r {

enum WingmanLlmProviders {
    WINGMAN_PROVIDER_MOCK,
    WINGMAN_PROVIDER_OPENAI,
};

/**
 * Wingman is a class that provides a set of LLM-based use cases.
 */
class Wingman
{
private:
    WingmanLlmProviders llmProvider;

public:
    explicit Wingman(WingmanLlmProviders llmProvider);
    Wingman(const Wingman&) = delete;
    Wingman(const Wingman&&) = delete;
    Wingman& operator =(const Wingman&) = delete;
    Wingman& operator =(const Wingman&&) = delete;
    virtual ~Wingman();

    virtual void summarize(const std::string& text, std::string& summary) = 0;
};

}
#endif // M8R_WINGMAN_H
