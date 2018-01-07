/*
 installer.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "installer.h"

using namespace std;

namespace m8r {

Installer::Installer()
{
}

Installer::~Installer()
{
}

void Installer::createEmptyMindForgerRepository(const string& directory)
{
    if(!directory.empty()) {
        if(!isDirectoryExist(directory.c_str())) {
            createDirectory(directory);
        }
        // create skeleton directories: ./memory ./mind ./limbo ./stencils/outlines|notes
        string path{};
        path.append(directory);
        path.append(FILE_PATH_SEPARATOR);
        path.append(FILE_PATH_MEMORY);
        createDirectory(path);

        path.assign(directory);
        path.append(FILE_PATH_SEPARATOR);
        path.append(FILE_PATH_MIND);
        createDirectory(path);

        path.assign(directory);
        path.append(FILE_PATH_SEPARATOR);
        path.append(FILE_PATH_LIMBO);
        createDirectory(path);

        path.assign(directory);
        path.append(FILE_PATH_SEPARATOR);
        path.append(FILE_PATH_STENCILS);
        createDirectory(path);

        path.append(FILE_PATH_SEPARATOR);
        path.append(FILE_PATH_OUTLINES);
        createDirectory(path);

        path.assign(directory);
        path.append(FILE_PATH_SEPARATOR);
        path.append(FILE_PATH_STENCILS);
        path.append(FILE_PATH_SEPARATOR);
        path.append(FILE_PATH_NOTES);
        createDirectory(path);
    }
    // IMPROVE else
}

void Installer::createDirectory(const string& path) {
    int e = mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
    if(e) {
        cerr << "Failed to create directory '" << path << "' with error " << e;
        throw MindForgerException("Directory creation failed.");
    }
}

bool Installer::isMindForgerRepository(const std::string& directory)
{
    UNUSED_ARG(directory);

    // TODO
    return false;
}

}
