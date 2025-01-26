/*
 unicode.h     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_UNICODE_H
#define M8R_UNICODE_H

namespace m8r {

constexpr const auto U_CODE_URGENCY_ON = 0x2666;
    // 0x2665  ... hart (filled)
    // 0x2666  ... diamond (filled)
    // 0x2713  ... check
    // 0x25D5  ... timer clock (filled) - unreadable/ugly on Win
    // 0x29D7  ... sand clocks - not in fonts on macOS and Fedora
    // 0x1F514 ... sound bell - not in fonts on Ubuntu
constexpr const auto U_CODE_URGENCY_OFF = 0x2662;
    // 0x2662 ... diamond (empty)
    // 0x00B7 ... middle dot
    // 0x2717 ... cross check (x)
    // 0x25F4 ... timer clocks (empty)
    // 0x29D6 ... sand clocks (empty)

constexpr const auto U_CODE_IMPORTANCE_ON = 0x2605;
    // 0x2605 ... star (filled)
constexpr const auto U_CODE_IMPORTANCE_OFF = 0x2606;
    // 0x2606 ... star (empty)
    // 0x00B7 ... middle dot

constexpr const auto U_CODE_PARAGRAPH_NL = 0x2029;
    // 0x2029 ... used by Qt as paragraph delimiter
    //   From Qt selectedText() documentation: If the selection obtained from
    // an editor spans a line break, the text will contain a Unicode U+2029 paragraph
    // separator character instead of a newline \n character.

/**
 * @brief The Unicode.
 *
 * @see Unicode table: https://unicode-table.com/
 * @see Unicode 1.1: https://emojipedia.org/unicode-1.1/
 */
class Unicode
{
public:
    explicit Unicode();
    Unicode(const Unicode&) = delete;
    Unicode(const Unicode&&) = delete;
    Unicode& operator =(const Unicode&) = delete;
    Unicode& operator =(const Unicode&&) = delete;
    ~Unicode();
};

}
#endif // M8R_UNICODE_H
