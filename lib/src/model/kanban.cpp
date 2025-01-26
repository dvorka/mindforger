/*
 kanban.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "kanban.h"

using namespace std;

namespace m8r {

string Kanban::createKanbanKey()
{
    return Organizer::createOrganizerKey(
        set<string>{}, "repository", Thing::getNextKey(), "/", "kanban"
    );
}

Kanban::Kanban(const std::string& name)
    : Organizer{name, Organizer::OrganizerType::KANBAN},
      columnTags{}
{
    if(!this->name.length()) {
        this->name = "Kanban";
    }

    initColumnTags();
}

Kanban::Kanban(const Kanban& k)
    : Organizer{dynamic_cast<const Organizer&>(k)},
      columnTags{}
{
    initColumnTags();
}

Kanban::~Kanban()
{
}

void Kanban::initColumnTags()
{
    columnTags.push_back(this->tagsUlQuadrant);
    columnTags.push_back(this->tagsUrQuadrant);
    columnTags.push_back(this->tagsLlQuadrant);
    columnTags.push_back(this->tagsLrQuadrant);
}

set<string>& Kanban::getStringTagsForColumn(unsigned column)
{
    static set<string> empty{};

    if(column < columnTags.size()) {
        return columnTags[column];
    } else {
        // out of range column fallback
        return empty;
    }
}

vector<const Tag*> Kanban::getTagsForColumn(unsigned column, Ontology& ontology)
{
    set<string> stringTags{this->getStringTagsForColumn(column)};

    vector<const Tag*> r{};
    for(auto stringTag:stringTags) {
        r.push_back(ontology.findOrCreateTag(stringTag));
    }

    return r;
}

} // m8r namespace
