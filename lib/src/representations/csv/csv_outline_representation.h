/*
 csv_outline_representation.h     MindForger thinking notebook

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
#ifndef M8R_CSV_OUTLINE_REPRESENTATION_H
#define M8R_CSV_OUTLINE_REPRESENTATION_H

#include "../markdown/markdown_outline_representation.h"
#include "../../persistence/persistence.h"
#include "../../gear/string_utils.h"

namespace m8r {

class CsvOutlineRepresentation
{
    MarkdownOutlineRepresentation& markdownRepresentation;
    Persistence* persistence;

    std::vector<std::string*> lines;

public:
    explicit CsvOutlineRepresentation();
    CsvOutlineRepresentation(const CsvOutlineRepresentation&) = delete;
    CsvOutlineRepresentation(const CsvOutlineRepresentation&&) = delete;
    CsvOutlineRepresentation &operator=(const CsvOutlineRepresentation&) = delete;
    CsvOutlineRepresentation &operator=(const CsvOutlineRepresentation&&) = delete;
    virtual ~CsvOutlineRepresentation();

    // TODO add cout method for ALL Os saving
    bool outline(const m8r::File& file, const m8r::File& outlineFile);
    // TODO write header
};

}
#endif // M8R_CSV_OUTLINE_REPRESENTATION_H
