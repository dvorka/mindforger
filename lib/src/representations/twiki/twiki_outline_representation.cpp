/*
 twiki_outline_representation.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "twiki_outline_representation.h"

namespace m8r {

TWikiOutlineRepresentation::TWikiOutlineRepresentation()
    : OutlineRepresentation{}
{
}

TWikiOutlineRepresentation::~TWikiOutlineRepresentation()
{
}

virtual Outline* TWikiOutlineRepresentation::outline(const m8r::File& file)
{
    // straightforward conversion: only sections are converted, the rest is kept intact
    fileSize = 0;
    if(fileToLines(filePath, lines, fileSize)) {
        unsigned offset = 0;
    }

    // TODO save converted file and load it using Markdown representation

    // TODO add twiki and import tags

    // TODO save outline to target destination
}

} // m8r namespace
