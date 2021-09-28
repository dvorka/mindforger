/*
 repository_configuration.cpp     M8r configuration management

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "repository_configuration.h"

namespace m8r {

using namespace std;


RepositoryConfiguration::RepositoryConfiguration()
{
}

RepositoryConfiguration::~RepositoryConfiguration()
{
    clear();
}

void RepositoryConfiguration::clear()
{
    // organizers
    for(auto& o:organizers) {
        delete o;
    }
    organizers.clear();
}

void RepositoryConfiguration::addOrganizer(Organizer* organizer)
{
    this->organizers.push_back(organizer);
}

void RepositoryConfiguration::removeOrganizer(Organizer* organizer)
{
    this->organizers.erase(
        std::remove(
            this->organizers.begin(),
            this->organizers.end(),
            organizer
        ),
        this->organizers.end()
    );

    delete organizer;
}

} // m8r namespace
