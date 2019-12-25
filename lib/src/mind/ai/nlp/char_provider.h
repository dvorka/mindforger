/*
 char_provider.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_CHAR_PROVIDER_H
#define M8R_CHAR_PROVIDER_H

namespace m8r {

class CharProvider
{
public:
    virtual ~CharProvider() {}

    /**
     * @brief Indicate whether there are more characters to consume.
     */
    virtual bool hasNext() = 0;

    /**
     * @brief Get current character, but do NOT move in the stream.
     */
    virtual const char& get() = 0;

    /**
     * @brief Get next character, but do NOT move in the stream.
     */
    virtual const char& getLookahead() = 0;

    /**
     * @brief Move to the NEXT character in the stream and return it.
     */
    virtual const char& next() = 0;
};

}
#endif // M8R_CHAR_PROVIDER_H
