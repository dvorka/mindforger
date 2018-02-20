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

RepositoryIndexer::RepositoryIndexer()
    : repository(nullptr)
{}

RepositoryIndexer::~RepositoryIndexer() {
    clear();
}

void RepositoryIndexer::clear()
{
    repository = nullptr;

    for(const string* f:allFiles) {
        delete f;
    }
    allFiles.clear();

    // markdowns strings were cleared as a part of allFiles strings
    markdowns.clear();

    for(const string* s:outlineStencils) {
        delete s;
    }
    outlineStencils.clear();

    for(const string* s:noteStencils) {
        delete s;
    }
    noteStencils.clear();
}

void RepositoryIndexer::index(Repository* repository)
{
    if(repository) {
        clear();

        this->repository = repository;

        memoryDirectory.assign(repository->getPath());

        if(repository->getType() == Repository::RepositoryType::MINDFORGER
             &&
           repository->getMode() == Repository::RepositoryMode::REPOSITORY
        ) {
            memoryDirectory.append(FILE_PATH_SEPARATOR);
            memoryDirectory.append(FILE_PATH_MEMORY);

            outlineStencilsDirectory.assign(repository->getPath());
            outlineStencilsDirectory.append(FILE_PATH_SEPARATOR);
            outlineStencilsDirectory.append(FILE_PATH_STENCILS);
            outlineStencilsDirectory.append(FILE_PATH_SEPARATOR);
            outlineStencilsDirectory.append(FILE_PATH_OUTLINES);

            noteStencilsDirectory.assign(repository->getPath());
            noteStencilsDirectory.append(FILE_PATH_SEPARATOR);
            noteStencilsDirectory.append(FILE_PATH_STENCILS);
            noteStencilsDirectory.append(FILE_PATH_SEPARATOR);
            noteStencilsDirectory.append(FILE_PATH_NOTES);
        } else {
            outlineStencilsDirectory.clear();
            noteStencilsDirectory.clear();
        }

        updateIndex();
    } // IMPROVE else throw
}

void RepositoryIndexer::updateIndex() {
#ifdef DO_MF_DEBUG
    MF_DEBUG("\nIndexing repository:\n  " << repository->getPath());
    auto begin = chrono::high_resolution_clock::now();
#endif

    updateIndexMemory(memoryDirectory);

    if(repository->getType() == Repository::RepositoryType::MINDFORGER
         &&
       repository->getMode() == Repository::RepositoryMode::REPOSITORY
    ) {
        updateIndexStencils(outlineStencilsDirectory, outlineStencils);
        updateIndexStencils(noteStencilsDirectory, noteStencils);
    }

#ifdef DO_MF_DEBUG
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG("\nRepository indexed in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms");
#endif
}

void RepositoryIndexer::updateIndexMemory(const string& directory)
{
    if(repository->getMode() == Repository::RepositoryMode::REPOSITORY) {
        MF_DEBUG("\nINDEXING memory DIR: " << directory);
        DIR *dir;
        if((dir = opendir(directory.c_str()))) {
            const struct dirent *entry;
            if((entry = readdir(dir))) {
                string path;
                string *ppath;
                do {
                    if(entry->d_type == DT_DIR) {
                        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                            continue;
                        }
                        //MF_DEBUG("\nDIVE> " << directory.c_str() << "//" << entry->d_name);
                        path.assign(directory);
                        path.append(FILE_PATH_SEPARATOR);
                        path.append(entry->d_name);

                        updateIndexMemory(path);
                    } else {
                        MF_DEBUG("\n  FILE: " << directory.c_str() << "//" << entry->d_name);
                        ppath = new string{directory};
                        ppath->append(FILE_PATH_SEPARATOR);
                        ppath->append(entry->d_name);

                        allFiles.insert(ppath);
                        if(stringEndsWith(*ppath, FILE_EXTENSION_MARKDOWN)) {
                            markdowns.insert(ppath);
                        }
                    }
                } while ((entry = readdir(dir)) != 0);
                closedir(dir);
            }
        }
    } else {
        MF_DEBUG("\nINDEXING memory single FILE: " << repository->getFile());
        if(repository->getFile().size()) {
            string* path = new string{repository->getPath()};
            path->append(FILE_PATH_SEPARATOR);
            path->append(repository->getFile());
            allFiles.insert(path);
            if(stringEndsWith(*path, FILE_EXTENSION_MARKDOWN)) {
                markdowns.insert(path);
            }
        }
    }
}

void RepositoryIndexer::updateIndexStencils(const string& directory, set<const std::string*>& stencils)
{
    MF_DEBUG("\nINDEXING stencils DIR: " << directory);
    DIR *dir;
    if((dir = opendir(directory.c_str()))) {
        const struct dirent *entry;
        if((entry = readdir(dir))) {
            string *path;
            do {
                if(entry->d_type != DT_DIR) {
                    MF_DEBUG("\n  FILE: " << directory.c_str() << "//" << entry->d_name);
                    path = new string{directory};
                    path->append(FILE_PATH_SEPARATOR);
                    path->append(entry->d_name);
                    if(stringEndsWith(*path, FILE_EXTENSION_MARKDOWN)) {
                        stencils.insert(path);
                    }
                }
            } while ((entry = readdir(dir)) != 0);
            closedir(dir);
        }
    }
}

const set<const string*> RepositoryIndexer::getMarkdownFiles() const {
    return markdowns;
}

const set<const string*> RepositoryIndexer::getAllOutlineFileNames() const {
    return allFiles;
}

const set<const std::string*> RepositoryIndexer::getOutlineStencilsFileNames() const
{
    return outlineStencils;
}

const set<const std::string*> RepositoryIndexer::getNoteStencilsFileNames() const
{
    return noteStencils;
}

char* RepositoryIndexer::getTagsFromPath() {
    return nullptr;
}

bool RepositoryIndexer::isMindForgerRepository(const string& directory)
{
    if(directory.empty()) {
        return false;
    } else if(!isDirectoryOrFileExists(directory.c_str())) {
        return false;
    }

    string path{};
    path.append(directory);
    path.append(FILE_PATH_SEPARATOR);
    path.append(FILE_PATH_MEMORY);
    if(!isDirectoryOrFileExists(path.c_str())) {
        return false;
    }

    return true;
}

Repository* RepositoryIndexer::getRepositoryForPath(const std::string& path)
{
    if(isDirectoryOrFileExists(path.c_str())) {
        if(isDirectory(path.c_str())) {
            if(isMindForgerRepository(path)) {
                return new Repository(path);
            } else {
                return new Repository(path, Repository::RepositoryType::MARKDOWN);
            }
        } else {
            string directory, file;
            pathToDirectoryAndFile(path, directory, file);
            // MF/MD for single file will be determined after parsing i.e. metadata detection (would be too expensive here)
            return new Repository(directory, Repository::RepositoryType::MINDFORGER, Repository::RepositoryMode::FILE, file);
        }
    } else {
        // directory doesn't exist - nothing to learn
        return nullptr;
    }
}

} /* namespace */

