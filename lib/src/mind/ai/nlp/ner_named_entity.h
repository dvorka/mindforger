/*
 ner_named_entity.h     MindForger thinking notebook

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
#ifndef M8R_NER_NAMED_ENTITY_H
#define M8R_NER_NAMED_ENTITY_H

#include <string>

namespace m8r {

enum NerNamedEntityType {
    PERSON = 1<<0,
    LOCATION = 1<<1,
    ORGANIZATION = 1<<2,
    MISC = 1<<3
};

struct NerNamedEntity
{
    std::string name;
    NerNamedEntityType type;
    float score;

    explicit NerNamedEntity(const std::string& n, NerNamedEntityType t, float s) {
        this->name = n;
        this->type = t;
        this->score = s;
    }
};

}
#endif // M8R_NER_NAMED_ENTITY_H
