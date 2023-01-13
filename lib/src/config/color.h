/*
 color.h     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_COLOR_H
#define M8R_COLOR_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <vector>
#include <memory>

namespace m8r {

/**
 * @brief The Color class.
 *
 * Palette of MF reserved colors:
 *
 *   red    #ff8181     (important)
 *   blue   #99b1ff     (cool)
 *   green  #7fc074     (personal)
 *   purple #c79ec9     (later)
 *   gray   #cccccc     (obsolete)
 *   black  #000000     (problem)
 *   yellow #ffcd8d     (todo)
 */
class Color
{
private:
    unsigned long c;
    std::string html{};

public:
    static const Color& RED() {
        static const Color color(0xFF,0,0);
        return color;
    }
    static const Color& GREEN() {
        static const Color color(0,0xFF,0);
        return color;
    }
    static const Color& BLUE() {
        static const Color color(0,0,0xFF);
        return color;
    }
    static const Color& WHITE() {
        static const Color color(0xFF,0xFF,0xFF);
        return color;
    }
    static const Color& BLACK() {
        static const Color color(0,0,0);
        return color;
    }
    static const Color& LIGHT_GRAY() {
        static const Color color(0xCC,0xCC,0xCC);
        return color;
    }
    static const Color& DARK_GRAY() {
        static const Color color(0x33,0x33,0x33);
        return color;
    }

    // MindForger colors
    static const Color& MF_RED() {
        static const Color color(0xFF,0x81,0x81);
        return color;
    }
    static const Color& MF_GREEN() {
        static const Color color(0x7F,0xC0,0x74);
        return color;
    }
    static const Color& MF_BLUE() {
        static const Color color(0x99,0xB1,0xFF);
        return color;
    }
    static const Color& MF_YELLOW() {
        static const Color color(0xFF,0xCD,0x8D);
        return color;
    }
    static const Color& MF_PURPLE() {
        static const Color color(0xC7,0x9E,0xC9);
        return color;
    }
    static const Color& MF_BLACK() {
        static const Color color(0,0,0);
        return color;
    }
    static const Color& MF_GRAY() {
        static const Color color(0x88,0x88,0x88);
        return color;
    }
    static const Color& MF_TURQUOISE() {
        static const Color color(64,224,208);
        return color;
    }

    explicit Color(unsigned char r, unsigned char g, unsigned char b) {
        c = b;
        c += g<<8;
        c += r<<16;

        std::ostringstream ss;
        ss << "#" << std::setfill('0') << std::setw(6) << std::hex << c;
        html = ss.str();
    }
    Color(const Color&) = delete;
    Color(const Color&&) = delete;
    Color &operator=(const Color&) = delete;
    Color &operator=(const Color&&) = delete;

    const unsigned long& asLong() const { return c; }
    const std::string& asHtml() const { return html; }
};

}
#endif // M8R_COLOR_H
