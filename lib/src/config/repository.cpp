/*
 repository.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "repository.h"

namespace m8r {

using namespace std;

Repository::Repository(
    string dir,
    RepositoryType type,
    RepositoryMode mode,
    std::string file,
    bool readOnly)
    : dir(dir), file(file), type(type), mode(mode), readOnly(readOnly)
{
    updatePath();
    flags = 0;
}

Repository::Repository(const Repository& r)
{
    dir = r.dir;
    file = r.file;
    path = r.path;
    type = r.type;
    mode = r.mode;
    readOnly = r.readOnly;
    flags = r.flags;
}

Repository::~Repository()
{
}

void Repository::updatePath()
{
    path.assign(dir);
    if(mode == RepositoryMode::FILE) {
        path += FILE_PATH_SEPARATOR;
        path += file;
    }
}

} // m8r namespace
