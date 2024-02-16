/*
 persistence.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_PERSISTENCE_H_
#define M8R_PERSISTENCE_H_

#include "../model/stencil.h"
#include "../model/outline.h"

namespace m8r {

/**
 * @brief Persistence.
 */
class Persistence {
public:
    virtual ~Persistence();

    virtual std::string createFileName(
            const std::string& directory,
            const std::string* text,
            const std::string& extension) = 0;
    virtual void load(Stencil* stencil) = 0;
    virtual bool isWriteable(const std::string& outlineKey) = 0;
    virtual void save(Outline* outline) = 0;
    virtual void saveAsHtml(Outline* outline, const std::string& fileName) = 0;
};

}

#endif /* M8R_PERSISTENCE_H_ */
