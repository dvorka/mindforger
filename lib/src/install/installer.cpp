/*
 installer.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "installer.h"

using namespace std;

namespace m8r {

// IMPROVE platform specific delimiters
const std::string Installer::FILE_PATH_DEFAULT_LINUX_SYSTEM_REPOSITORY = std::string{"/usr/share/doc/mindforger"};

Installer::Installer()
#if defined(__APPLE__) || defined(_WIN32)
    : filePathDefaultSystemRepository{getExecutablePath()}
#else
    : filePathDefaultSystemRepository{FILE_PATH_DEFAULT_LINUX_SYSTEM_REPOSITORY}
#endif
{
#if defined(__APPLE__) || defined(_WIN32)
    // append path in DMG from the executable to documentation in resources directory
    if(filePathDefaultSystemRepository.size()) {
        filePathDefaultSystemRepository.erase(filePathDefaultSystemRepository.find_last_of(FILE_PATH_SEPARATOR)+1);
        filePathDefaultSystemRepository.append("..").append(FILE_PATH_SEPARATOR).append("Resources").append(FILE_PATH_SEPARATOR);
        filePathDefaultSystemRepository.append(DIRNAME_M8R_REPOSITORY);
    }
#endif
}

Installer::~Installer() = default;

bool Installer::createEmptyMindForgerRepository(const string& directory)
{
    if(!directory.empty()) {
        if(!isDirectoryOrFileExists(directory.c_str())) {
            if(!createDirectory(directory)) {
                return false;
            }
        }
        // create skeleton directories: ./memory ./mind ./limbo ./stencils/outlines|notes
        string path{};
        path += directory;
        path += FILE_PATH_SEPARATOR;
        path += FILE_PATH_MEMORY;
        createDirectory(path);

        path.assign(directory);
        path += FILE_PATH_SEPARATOR;
        path += FILE_PATH_MIND;
        createDirectory(path);

        path.assign(directory);
        path += FILE_PATH_SEPARATOR;
        path += FILE_PATH_LIMBO;
        createDirectory(path);

        path.assign(directory);
        path += FILE_PATH_SEPARATOR;
        path += FILE_PATH_STENCILS;
        createDirectory(path);

        path += FILE_PATH_SEPARATOR;
        path += FILE_PATH_OUTLINES;
        createDirectory(path);

        path.assign(directory);
        path += FILE_PATH_SEPARATOR;
        path += FILE_PATH_STENCILS;
        path += FILE_PATH_SEPARATOR;
        path += FILE_PATH_NOTES;
        createDirectory(path);

        return true;
    }

    return false;
}

bool Installer::initMindForgerRepository(bool copyDoc, bool copyStencils, const char* dstRepository)
{
    if(isDirectoryOrFileExists(dstRepository)) {
        if(isDirectoryOrFileExists(filePathDefaultSystemRepository.c_str())) {
            if(copyDoc) {
                string srcPath{filePathDefaultSystemRepository};
                srcPath += FILE_PATH_SEPARATOR;
                srcPath += FILE_PATH_MEMORY;
                string dstPath{dstRepository};
                dstPath += FILE_PATH_SEPARATOR;
                dstPath += FILE_PATH_MEMORY;
                if(isDirectoryOrFileExists(srcPath.c_str())) {
                    copyDirectoryRecursively(srcPath.c_str(), dstPath.c_str(), true);
                } else {
                    cerr << "ERROR: attempt to copy documentation failed - source directory doesn't exist: "
                         << filePathDefaultSystemRepository
                         << endl;
                }
            }

            if(copyStencils) {
                string srcPath{filePathDefaultSystemRepository};
                srcPath += FILE_PATH_SEPARATOR;
                srcPath += FILE_PATH_STENCILS;
                string dstPath{dstRepository};
                dstPath += FILE_PATH_SEPARATOR;
                dstPath += FILE_PATH_STENCILS;
                if(isDirectoryOrFileExists(srcPath.c_str())) {
                    copyDirectoryRecursively(srcPath.c_str(), dstPath.c_str(), true);
                } else {
                    cerr << "ERROR: attempt to copy stencils failed - source directory doesn't exist" << endl;
                }
            }

            // resource copy is NOT intentionally strict - copy what's possible, ignore eventual errors
            return true;
        } else {
            cerr << "ERROR: default source repository not found in " << filePathDefaultSystemRepository << endl;
            return false;
        }
    } else {
        cerr << "ERROR: it is expected that empty target repository exists " << dstRepository << endl;
        return false;
    }
}

} // m8r namespace
