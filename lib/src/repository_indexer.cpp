/*
 repository_indexer.cpp     MindForger thinking notebook

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
#include "repository_indexer.h"

#include <chrono>

using namespace std;

namespace m8r {

RepositoryIndexer::RepositoryIndexer(const string& repositoryDirPath)
{
    repositoryPath.append(repositoryDirPath);

    memoryDirectory.append(repositoryPath);
    memoryDirectory.append(FILE_PATH_SEPARATOR);
    memoryDirectory.append(FILE_PATH_MEMORY);

    outlineStencilsDirectory.append(repositoryPath);
    outlineStencilsDirectory.append(FILE_PATH_SEPARATOR);
    outlineStencilsDirectory.append(FILE_PATH_STENCILS);
    outlineStencilsDirectory.append(FILE_PATH_SEPARATOR);
    outlineStencilsDirectory.append(FILE_PATH_OUTLINES);

    noteStencilsDirectory.append(repositoryPath);
    noteStencilsDirectory.append(FILE_PATH_SEPARATOR);
    noteStencilsDirectory.append(FILE_PATH_STENCILS);
    noteStencilsDirectory.append(FILE_PATH_SEPARATOR);
    noteStencilsDirectory.append(FILE_PATH_NOTES);
}

void RepositoryIndexer::updateIndex() {
#ifdef DO_MF_DEBUG
    MF_DEBUG("\nIndexing repository:\n  " << repositoryPath);
    auto begin = chrono::high_resolution_clock::now();
#endif

    updateIndexMemory(memoryDirectory);
    updateIndexStencils(outlineStencilsDirectory, outlineStencils);
    updateIndexStencils(noteStencilsDirectory, noteStencils);

#ifdef DO_MF_DEBUG
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG("\nRepository indexed in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms");
#endif
}

void RepositoryIndexer::updateIndexMemory(const string directory)
{
    MF_DEBUG("\nINDEXING memory DIR: " << directory);
    DIR *dir;
    if((dir = opendir(directory.c_str()))) {
        const struct dirent *entry;
        if((entry = readdir(dir))) {
            string path;
            string *ppath;
            do {
                if (entry->d_type == DT_DIR) {
                    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                        continue;
                    }
                    //MFDEBUG("\nDIVE> " << directory.c_str() << "//" << entry->d_name);
                    path.assign(directory);
                    path.append(FILE_PATH_SEPARATOR);
                    path.append(entry->d_name);

                    updateIndexMemory(path);
                } else {
                    MF_DEBUG("\n  FILE: " << directory.c_str() << "//" << entry->d_name);
                    ppath = new string{directory};
                    ppath->append(FILE_PATH_SEPARATOR);
                    ppath->append(entry->d_name);

                    allFiles.push_back(ppath);
                    if(stringEndsWith(*ppath, FILE_EXTENSION_MARKDOWN)) {
                        markdowns.push_back(ppath);
                    }
                }
            } while ((entry = readdir(dir)) != 0);
            closedir(dir);
        }
    }
}

void RepositoryIndexer::updateIndexStencils(const string& directory, vector<const std::string*>& stencils)
{
    MF_DEBUG("\nINDEXING stencils DIR: " << directory);
    DIR *dir;
    if((dir = opendir(directory.c_str()))) {
        const struct dirent *entry;
        if((entry = readdir(dir))) {
            string *path;
            do {
                if (entry->d_type != DT_DIR) {
                    MF_DEBUG("\n  FILE: " << directory.c_str() << "//" << entry->d_name);
                    path = new string{directory};
                    path->append(FILE_PATH_SEPARATOR);
                    path->append(entry->d_name);
                    if(stringEndsWith(*path, FILE_EXTENSION_MARKDOWN)) {
                        stencils.push_back(path);
                    }
                }
            } while ((entry = readdir(dir)) != 0);
            closedir(dir);
        }
    }
}

const vector<const string*> RepositoryIndexer::getMarkdownFiles() const {
    return markdowns;
}

const vector<const string*> RepositoryIndexer::getAllOutlineFileNames() const {
    return allFiles;
}

const vector<const std::string*> RepositoryIndexer::getOutlineStencilsFileNames() const
{
    return outlineStencils;
}

const vector<const std::string*> RepositoryIndexer::getNoteStencilsFileNames() const
{
    return noteStencils;
}

char* RepositoryIndexer::getTagsFromPath() {
    return nullptr;
}

RepositoryIndexer::~RepositoryIndexer() {
    for(const string* f:allFiles) {
        delete f;
    }
    for(const string* s:outlineStencils) {
        delete s;
    }
    for(const string* s:noteStencils) {
        delete s;
    }
}

} /* namespace */

