/*
 color.h     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
 * Palette:
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

    // map of all colors ensuring singletons
    static std::map<unsigned long,const Color&> colors;
    // container ensuring custom colors destruction
    static std::vector<std::unique_ptr<Color>> customColors;

public:
    static const Color& RED(void) {
        static const Color color(0xFF,0,0);
        return color;
    }
    static const Color& GREEN(void) {
        static const Color color(0,0xFF,0);
        return color;
    }
    static const Color& BLUE(void) {
        static const Color color(0,0,0xFF);
        return color;
    }
    static const Color& WHITE(void) {
        static const Color color(0xFF,0xFF,0xFF);
        return color;
    }
    static const Color& BLACK(void) {
        static const Color color(0,0,0);
        return color;
    }
    static const Color& LIGHT_GRAY(void) {
        static const Color color(0xCC,0xCC,0xCC);
        return color;
    }
    static const Color& DARK_GRAY(void) {
        static const Color color(0x33,0x33,0x33);
        return color;
    }

    // MindForger colors
    static const Color& MF_RED(void) {
        static const Color color(0xFF,0x81,0x81);
        return color;
    }
    static const Color& MF_GREEN(void) {
        static const Color color(0x7F,0xC0,0x74);
        return color;
    }
    static const Color& MF_BLUE(void) {
        static const Color color(0x99,0xB1,0xFF);
        return color;
    }
    static const Color& MF_YELLOW(void) {
        static const Color color(0xFF,0xCD,0x8D);
        return color;
    }
    static const Color& MF_PURPLE(void) {
        static const Color color(0xC7,0x9E,0xC9);
        return color;
    }
    static const Color& MF_BLACK(void) {
        static const Color color(0,0,0);
        return color;
    }
    static const Color& MF_GRAY(void) {
        static const Color color(0xCC,0xCC,0xCC);
        return color;
    }

    // palette
    static const Color& findOrCreate(unsigned char r, unsigned char g, unsigned char b) {
        // IMPROVE condition is slow and NOT reentrant, but don't know how to implement it w/o it and avoid C++ init fiasco
        if(colors.empty()) {
            colors.insert(std::pair<unsigned long,const Color&>(RED().asLong(), RED()));
            colors.insert(std::pair<unsigned long,const Color&>(GREEN().asLong(), GREEN()));
            colors.insert(std::pair<unsigned long,const Color&>(BLUE().asLong(), BLUE()));
            colors.insert(std::pair<unsigned long,const Color&>(WHITE().asLong(), WHITE()));
            colors.insert(std::pair<unsigned long,const Color&>(BLACK().asLong(), BLACK()));
            colors.insert(std::pair<unsigned long,const Color&>(LIGHT_GRAY().asLong(), LIGHT_GRAY()));
            colors.insert(std::pair<unsigned long,const Color&>(DARK_GRAY().asLong(), DARK_GRAY()));

            colors.insert(std::pair<unsigned long,const Color&>(MF_RED().asLong(), MF_RED()));
            colors.insert(std::pair<unsigned long,const Color&>(MF_GREEN().asLong(), MF_GREEN()));
            colors.insert(std::pair<unsigned long,const Color&>(MF_BLUE().asLong(), MF_BLUE()));
            colors.insert(std::pair<unsigned long,const Color&>(MF_YELLOW().asLong(), MF_YELLOW()));
            colors.insert(std::pair<unsigned long,const Color&>(MF_PURPLE().asLong(), MF_PURPLE()));
            colors.insert(std::pair<unsigned long,const Color&>(MF_BLACK().asLong(), MF_BLACK()));
            colors.insert(std::pair<unsigned long,const Color&>(MF_GRAY().asLong(), MF_GRAY()));
        }

        unsigned long k = (r<<16) + (g<<8) + b;
        auto result = colors.find(k);
        if(result!=colors.end()) {
            return result->second;
        } else {
            Color* newColor = new Color{r,g,b};
            colors.insert(std::pair<unsigned long,const Color&>(newColor->asLong(), *newColor));
            customColors.push_back(std::unique_ptr<Color>(newColor));
            return *newColor;
        }
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

    const unsigned long& asLong(void) const { return c; }
    const std::string& asHtml(void) const { return html; }

    // IMPROVE introduce Palette - for an entity it will give a color, palette can be loaded from configuration (per theme colors)
};

}
#endif // M8R_COLOR_H
