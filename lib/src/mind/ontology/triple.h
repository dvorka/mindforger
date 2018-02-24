/*
 triple.h     MindForger thinking notebook

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
#ifndef M8R_TRIPLE_H
#define M8R_TRIPLE_H

#include "thing.h"
#include "relationship_type.h"


namespace m8r {

class Thing;

/**
 * @brief Ontology triple.
 *
 * Subject -predicate-> Object
 *
 * See m8r::Ontology.
 */
class Triple : public Thing
{
protected:
    Thing* subject;
    RelationshipType* predicate;
    Thing* object;

public:
    explicit Triple();
    Triple(const Triple&) = delete;
    Triple(const Triple&&) = delete;
    Triple &operator=(const Triple&) = delete;
    Triple &operator=(const Triple&&) = delete;
    ~Triple();

    Thing* getSubject() const { return subject; }
    void setSubject(Thing* subject) { this->subject = subject; }
    RelationshipType* getPredicate() const { return predicate; }
    void setPredicate(RelationshipType* predicate) { this->predicate = predicate; }
    Thing* getObject() const { return object; }
    void setObject(Thing* object) { this->object = object; }
};

}
#endif // M8R_TRIPLE_H
