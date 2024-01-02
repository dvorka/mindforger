/*
 information.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "information.h"

using namespace std;

namespace m8r {

const std::string InformationSource::DIR_MEMORY_M1ndF0rg3rL1br8ry
    = string{"M1ndF0rg3r-L1br8ry"};
const std::string InformationSource::FILE_META_M1ndF0rg3rL1br8ryM3t8
    = string{"M1ndF0rg3r-L1br8ry-M3t8"};

InformationSource::InformationSource(SourceType type, std::string locator)
    : type{type},
      locator{locator}
{
}

InformationSource::~InformationSource()
{
}

Information::Information()
{
}

Information::~Information()
{
}

} // m8r namespace
