/*
 thing.cpp     MindForger thinking notebook

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
#include "thing_class_rel_triple.h"

namespace m8r {

using namespace std;

/*
 * Thing
 */

long Thing::sequence = 0;

Thing::Thing()
    : key{std::to_string(++sequence)},
      name{}
{
}

Thing::Thing(const string name)
{
    setName(name);
}

Thing::~Thing()
{
}

void Thing::autolinkName()
{
    if(name.size()) {
        auto pos = name.find(":");
        if(pos != string::npos) {
            autolinkingAlias = name.substr(0, pos);
            autolinkingAbbr = autolinkingAlias;
            autolinkingName = name.substr(pos+1);
            autolinkingName = stringLeftTrim(autolinkingName);
        } else {
            autolinkingAlias = name;
            autolinkingName = name;
            autolinkingAbbr.clear();
        }
    } else {
        autolinkingAlias.clear();
        autolinkingName.clear();
        autolinkingAbbr.clear();
    }
}

/*
 * Thing in time
 */

ThingInTime::ThingInTime()
    : Thing{},
      created{},
      read{},
      modified{}
{
}

ThingInTime::ThingInTime(const std::string name)
    : Thing{name}
{
}

ThingInTime::~ThingInTime()
{
}

time_t ThingInTime::getCreated() const
{
    return created;
}

void ThingInTime::setCreated()
{
    this->modified = datetimeNow();
}

void ThingInTime::setCreated(time_t created)
{
    this->created = created;
}

time_t ThingInTime::getModified() const
{
    return modified;
}

void ThingInTime::setModified()
{
    this->modified = datetimeNow();
}

void ThingInTime::setModified(time_t modified)
{
    MF_ASSERT_FUTURE_TIMESTAMPS(created, read, modified, this->getKey() << "# " << name, name);

    this->modified = modified;
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

RelationshipType::RelationshipType(const std::string& name, Clazz* isA, const Color& color)
    : Clazz{name, isA}, color(color)
{
    reflexive = transitive = symetric = false;
}

RelationshipType::~RelationshipType()
{
}

/*
 * Triple
 */

Triple::Triple(const std::string name, const Thing* subject, const RelationshipType* predicate, const Thing* object)
    : Thing(name),
      subject(subject),
      predicate(predicate),
      object(object)
{
}

Triple::~Triple()
{
}

/*
 * Relationship
 */

Relationship::Relationship(const std::string name, Thing* subject, RelationshipType* predicate, Thing* object)
    : Triple(name, subject, predicate, object)
{
}

Relationship::~Relationship()
{
}

} // m8r namespace

