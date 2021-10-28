/*
 kanban.cpp     MindForger thinking notebook

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
#include "kanban.h"

using namespace std;

namespace m8r {

std::string Kanban::createKanbanKey(
    const set<string>& keys,
    const string& directory,
    const string& id,
    const string& separator
) {
    return Organizer::createOrganizerKey(
        keys, directory, id, separator, "kanban"
    );
}

Kanban::Kanban(const std::string& name):
    Organizer{name, Organizer::OrganizerType::KANBAN}
{
    if(!this->name.length()) {
        this->name = "Kanban";
    }
}

Kanban::Kanban(const Kanban& k):
    Organizer{k}
{
}

Kanban::~Kanban()
{
}

} // m8r namespace
