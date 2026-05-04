/*
 palette.h     MindForger thinking notebook

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
#ifndef M8R_PALETTE_H
#define M8R_PALETTE_H

#include "color.h"

namespace m8r {

/**
 * @brief MindForger color palette.
 */
class Palette
{
private:
    // map of all colors ensuring singletons
    std::map<unsigned long,const Color&> colorsMap;
    // list of all colors
    std::vector<const Color*> colors;
    // container ensuring custom colors destruction
    std::vector<std::unique_ptr<Color>> customColors;

public:
    explicit Palette();
    Palette(const Palette&) = delete;
    Palette(const Palette&&) = delete;
    Palette &operator=(const Palette&) = delete;
    Palette &operator=(const Palette&&) = delete;
    ~Palette();

    /**
     * @brief Add MF built-in colors.
     */
    void addBuiltInColors();

    /**
     * @brief Add built-in color.
     *
     * Built-in color doesn't have to be deleted by destructor.
     */
    void addBuildInColor(const Color* c);

    /**
     * @brief Find or create custom color.
     */
    const Color& findOrCreate(unsigned char r, unsigned char g, unsigned char b);

    const Color& colorForName(const std::string& name) const;
};

}
#endif // M8R_PALETTE_H
