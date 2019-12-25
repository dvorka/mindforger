/*
 concept_stencil.cpp     MindForger thinking notebook

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
#include "stencil.h"

using namespace std;

namespace m8r {

Stencil::Stencil(const std::string filePath, ResourceType type)
    : type(type), filePath(filePath), name(filePath)
{
    // IMPROVE extract filename from path
    content = nullptr;
}

Stencil::Stencil(const std::string filePath, const std::string name, ResourceType type)
    : type(type), filePath(filePath), name(name)
{
    content = nullptr;
}

Stencil::~Stencil()
{
    if(content) {
        delete content;
    }
}

ResourceType m8r::Stencil::getType() const
{
    return type;
}

const string& Stencil::getFilePath() const
{
    return filePath;
}

const string& Stencil::getName() const
{
    return name;
}

const string* Stencil::getStencil() const
{
    return content;
}

void Stencil::setContent(string* stencil)
{
    this->content = stencil;
}

} // m8r namespace
