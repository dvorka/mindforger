/*
 palette.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "palette.h"

namespace m8r {

using namespace std;

Palette::Palette() = default;

Palette::~Palette() = default;

void Palette::addBuiltInColors()
{
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::RED().asLong(), Color::RED()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::GREEN().asLong(), Color::GREEN()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::BLUE().asLong(), Color::BLUE()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::WHITE().asLong(), Color::WHITE()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::BLACK().asLong(), Color::BLACK()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::LIGHT_GRAY().asLong(), Color::LIGHT_GRAY()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::DARK_GRAY().asLong(), Color::DARK_GRAY()));

    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::MF_RED().asLong(), Color::MF_RED()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::MF_GREEN().asLong(), Color::MF_GREEN()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::MF_BLUE().asLong(), Color::MF_BLUE()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::MF_YELLOW().asLong(), Color::MF_YELLOW()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::MF_PURPLE().asLong(), Color::MF_PURPLE()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::MF_BLACK().asLong(), Color::MF_BLACK()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::MF_GRAY().asLong(), Color::MF_GRAY()));
    colorsMap.insert(std::pair<unsigned long,const Color&>(Color::MF_TURQUOISE().asLong(), Color::MF_TURQUOISE()));
}

void Palette::addBuildInColor(const Color* c)
{
    colorsMap.insert(std::pair<unsigned long,const Color&>(c->asLong(), *c));
    colors.push_back(c);
}

const Color& Palette::findOrCreate(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned long k = (r<<16) + (g<<8) + b;
    auto result = colorsMap.find(k);
    if(result!=colorsMap.end()) {
        return result->second;
    }

    Color* newColor = new Color{r,g,b};
    colors.push_back(newColor);
    colorsMap.insert(std::pair<unsigned long,const Color&>(newColor->asLong(), *newColor));
    customColors.push_back(std::unique_ptr<Color>(newColor));
    return *newColor;
}

const Color& Palette::colorForName(const string& name) const
{
    unsigned int result=5381;
    for(unsigned int i=0; i<name.size(); i++) {
        result=result*33+name[i];
    }
    result = result^(result>>16);

    return *colors[result%colors.size()];
}

} // m8r namespace
