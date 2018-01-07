/*
 outline_representation.h     MindForger thinking notebook

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
#ifndef M8R_OUTLINE_REPRESENTATION_H_
#define M8R_OUTLINE_REPRESENTATION_H_

#include <string>

#include "../gear/file_utils.h"
#include "../model/outline.h"

namespace m8r {

class OutlineRepresentation {
public:
    explicit OutlineRepresentation();
    OutlineRepresentation(const OutlineRepresentation&) = delete;
    OutlineRepresentation(const OutlineRepresentation&&) = delete;
    OutlineRepresentation &operator=(const OutlineRepresentation&) = delete;
    OutlineRepresentation &operator=(const OutlineRepresentation&&) = delete;
    virtual ~OutlineRepresentation();

    virtual Outline* outline(const m8r::File& file) = 0;
};

} /* namespace m8r */

#endif /* M8R_OUTLINE_REPRESENTATION_H_ */
