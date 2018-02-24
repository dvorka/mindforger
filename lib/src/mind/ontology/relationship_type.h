/*
 relationship_type.h     MindForger thinking notebook

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
#ifndef M8R_RELATIONSHIP_TYPE_H
#define M8R_RELATIONSHIP_TYPE_H

namespace m8r {

class Clazz;

/**
 * @brief Ontology reationship type.
 *
 * See m8r::Ontology.
 */
class RelationshipType : public Clazz
{
public:
    explicit RelationshipType();
    RelationshipType(const RelationshipType&) = delete;
    RelationshipType(const RelationshipType&&) = delete;
    RelationshipType &operator=(const RelationshipType&) = delete;
    RelationshipType &operator=(const RelationshipType&&) = delete;
    ~RelationshipType();
};

}
#endif // M8R_RELATIONSHIP_TYPE_H
