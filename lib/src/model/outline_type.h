/*
 outline_type.h     MindForger thinking notebook

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
#ifndef M8R_OUTLINE_TYPE_H_
#define M8R_OUTLINE_TYPE_H_

#include "../config/color.h"
#include "../mind/ontology/thing_class_rel_triple.h"

namespace m8r {

class OutlineType : public Clazz {
private:
    const Color& color;

public:
    // static initialization order fiasco prevention
    static const std::string& KeyOutline() {
        static const std::string KEY_OUTLINE = std::string{"Outline"};
        return KEY_OUTLINE;
    }
    static const std::string& KeyGrow() {
        static const std::string KEY_GROW = std::string{"Goal"};
        return KEY_GROW;
    }
    static const std::string& KeyPdf() {
        static const std::string KEY_PDF = std::string{"PDF"};
        return KEY_PDF;
    }

    OutlineType() = delete;
    explicit OutlineType(const std::string& name, Clazz* isA, const Color& color);
    OutlineType(const OutlineType&) = delete;
    OutlineType(const OutlineType&&) = delete;
    OutlineType &operator=(const OutlineType&) = delete;
    OutlineType &operator=(const OutlineType&&) = delete;
    virtual ~OutlineType();

    const Color& getColor() const { return color; }
};

} // m8r namespace

#endif /* M8R_OUTLINE_TYPE_H_ */
