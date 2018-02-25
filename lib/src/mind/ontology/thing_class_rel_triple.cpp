/*
 thing.cpp     MindForger thinking notebook

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
#include "thing_class_rel_triple.h"

namespace m8r {

using namespace std;

/*
 * Thing
 */

Thing::Thing()
{}

Thing::Thing(const string name)
{
    this->name = name;
}

Thing::~Thing()
{
}

/*
 * Class
 */

Clazz::Clazz(const std::string& name, Clazz* isA)
    : Thing{name}
{
    this->isA = isA;
}

Clazz::~Clazz()
{
}

/*
 * RelationshipType
 */

RelationshipType::RelationshipType(const std::string& name, Clazz* isA)
    : Clazz(name, isA)
{
}

RelationshipType::~RelationshipType()
{
}

/*
 * Triple
 */

Triple::Triple(const std::string name)
    : Thing(name)
{
}

Triple::~Triple()
{
}

/*
 * Relationship
 */

Relationship::Relationship(const std::string name)
    : Triple(name)
{
}

Relationship::~Relationship()
{
}

} // m8r namespace

