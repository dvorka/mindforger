/*
 csv_outline_representation.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <iostream>
#include <vector>

#include "../../model/outline.h"
#include "../../gear/async_utils.h"
#include "../../gear/file_utils.h"
#include "../../gear/string_utils.h"

namespace m8r {

/**
 * @brief The primary CSV purpose is to be bridge to machine learning world.
 *
 * CSV format is therefore designed to make loading of CSVs as datasets to ML frameworks.
 * No library is used to make things simple - also parsing is not needed, just serialization.
 *
 * @see https://tools.ietf.org/html/rfc4180
 */
class CsvOutlineRepresentation
{
public:
    explicit CsvOutlineRepresentation();
    CsvOutlineRepresentation(const CsvOutlineRepresentation&) = delete;
    CsvOutlineRepresentation(const CsvOutlineRepresentation&&) = delete;
    CsvOutlineRepresentation& operator=(const CsvOutlineRepresentation&) = delete;
    CsvOutlineRepresentation& operator=(const CsvOutlineRepresentation&&) = delete;
    virtual ~CsvOutlineRepresentation();

    bool to(
        const std::vector<Outline*>& os,
        const filesystem::File& sourceFile,
        ProgressCallbackCtx* callbackCtx = nullptr
    );

    void toHeader(std::ofstream& out);
    void to(const Outline* o, std::ofstream& out);

private:
    void quoteValue(const std::string& is, std::string& os);

};

}
#endif // M8R_CSV_OUTLINE_REPRESENTATION_H
