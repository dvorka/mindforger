/*
 organizer.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "organizer.h"

using namespace std;

namespace m8r {

const string Organizer::TYPE_STR_KANBAN = string{"Kanban"};
const string Organizer::TYPE_STR_EISENHOWER_MATRIX= string{"Eisenhower Matrix"};

std::string Organizer::createOrganizerKey(
    const set<string>& keys,
    const string& directory,
    const string& id,
    const string& separator,
    const string& type
) {
    string key = directory + separator
            + "m1ndf0rg3r" + separator
            + type + separator
            + id;
    do {
        key += "_";
    } while(keys.find(key) != keys.end());
    return key;
}

Organizer::Organizer(const std::string& name, OrganizerType organizerType):
    organizerType{organizerType},
    filterBy{Organizer::FilterBy::OUTLINES_NOTES},
    modified{datetimeNow()}
{
    this->name = name;
}

Organizer::Organizer(const Organizer& o):
    Thing{o.getName()},
    organizerType{o.organizerType},
    filterBy{o.getFilterBy()},
    modified{o.modified}
{    
    this->tagsUrQuadrant = o.tagsUrQuadrant;
    this->tagsUlQuadrant = o.tagsUlQuadrant;
    this->tagsLrQuadrant = o.tagsLlQuadrant;
    this->tagsLlQuadrant = o.tagsLlQuadrant;

    this->scopeOutlineId = o.scopeOutlineId;
}

Organizer::~Organizer()
{
}

string Organizer::getFilterByAsStr() {
    switch(this->filterBy) {
    case Organizer::FilterBy::OUTLINES:
        return Organizer::CONFIG_VALUE_FILTER_BY_O;
    case Organizer::FilterBy::NOTES:
        return Organizer::CONFIG_VALUE_FILTER_BY_N;
    case Organizer::FilterBy::OUTLINES_NOTES:
    default:
        return Organizer::CONFIG_VALUE_FILTER_BY_O_N;
    }
}

bool Organizer::isValid() const {
    if(!this->tagsUrQuadrant.size()
       || !this->tagsLrQuadrant.size()
       || !this->tagsLlQuadrant.size()
       || !this->tagsUlQuadrant.size()
    ) {
        return false;
    }

    return true;
}

void Organizer::makeModified()
{
    this->modified = datetimeNow();
}

} // m8r namespace
