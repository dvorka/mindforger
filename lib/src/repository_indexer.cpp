/*
 repository_indexer.cpp     MindForger thinking notebook

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
#include "repository_indexer.h"

using namespace std;
using namespace m8r::filesystem;

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

    // markdowns (strings were cleared as a part of allFiles strings)
    markdowns.clear();
    // txt
    texts.clear();
    // PDFs
    pdfs.clear();

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
        if(!repository->getDir().empty() && !isDirectoryOrFileExists(repository->getDir().c_str())
        ) {
            throw MindForgerException{"Given repository path does NOT exist!"};
        }

        clear();

        this->repository = repository;

        memoryDirectory.assign(repository->getDir());

        if(repository->getType() == Repository::RepositoryType::MINDFORGER
             &&
           repository->getMode() == Repository::RepositoryMode::REPOSITORY
        ) {
            memoryDirectory += FILE_PATH_SEPARATOR;
            memoryDirectory += DIRNAME_MEMORY;

            outlineStencilsDirectory.assign(repository->getDir());
            outlineStencilsDirectory += FILE_PATH_SEPARATOR;
            outlineStencilsDirectory += DIRNAME_STENCILS;
            outlineStencilsDirectory += FILE_PATH_SEPARATOR;
            outlineStencilsDirectory += DIRNAME_OUTLINES;

            noteStencilsDirectory.assign(repository->getDir());
            noteStencilsDirectory += FILE_PATH_SEPARATOR;
            noteStencilsDirectory += DIRNAME_STENCILS;
            noteStencilsDirectory += FILE_PATH_SEPARATOR;
            noteStencilsDirectory += DIRNAME_NOTES;
        } else {
            outlineStencilsDirectory.clear();
            noteStencilsDirectory.clear();
        }

        updateIndex();
    } // IMPROVE else throw
}

void RepositoryIndexer::updateIndex() {
#ifdef DO_MF_DEBUG
    MF_DEBUG(endl << "Indexing repository:" << endl << "  " << repository->getDir());
    auto begin = chrono::high_resolution_clock::now();
#endif

    updateIndexMemory(memoryDirectory);

    if(repository->getType() == Repository::RepositoryType::MINDFORGER
       && repository->getMode() == Repository::RepositoryMode::REPOSITORY
    ) {
        updateIndexStencils(outlineStencilsDirectory, outlineStencils);
        updateIndexStencils(noteStencilsDirectory, noteStencils);
    }

#ifdef DO_MF_DEBUG
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG(endl << "Repository indexed in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms");
#endif
}

void RepositoryIndexer::updateIndexMemory(const string& directory)
{
    if(repository->getMode() == Repository::RepositoryMode::REPOSITORY) {
        MF_DEBUG(endl << "INDEXING memory DIR: " << directory);
        DIR* dir;
        if((dir = opendir(directory.c_str()))) {
            const struct dirent *entry;
            if((entry = readdir(dir))) {
                string path;
                string* ppath;
                do {
                    if(entry->d_type == DT_DIR) {
                        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                            continue;
                        }
                        //MF_DEBUG(endl << "DIVE> " << directory.c_str() << "//" << entry->d_name);
                        path.assign(directory);
                        path += FILE_PATH_SEPARATOR;
                        path += entry->d_name;

                        updateIndexMemory(path);
                    } else {
                        MF_DEBUG(endl << "  FILE: " << directory.c_str() << "//" << entry->d_name);
                        ppath = new string{directory};
                        ppath->append(FILE_PATH_SEPARATOR);
                        ppath->append(entry->d_name);

                        allFiles.insert(ppath);
                        if(File::fileHasMarkdownExtension(*ppath)) {
                            markdowns.insert(ppath);
                        } else if(File::fileHasPdfExtension(*ppath)) {
                            pdfs.insert(ppath);
                        } else if(File::fileHasTextExtension(*ppath)) {
                            texts.insert(ppath);
                        }
                    }
                } while ((entry = readdir(dir)) != 0);
                closedir(dir);
            }
        }
    } else {
        MF_DEBUG(endl << "INDEXING memory single FILE: " << repository->getFile() << " in " << repository->getDir());
        if(repository->getFile().size()) {
            string* path = new string{repository->getDir()};
            path->append(FILE_PATH_SEPARATOR);
            path->append(repository->getFile());
            allFiles.insert(path);
            if(File::fileHasMarkdownExtension(*path)) {
                markdowns.insert(path);
            }
        }
    }
}

void RepositoryIndexer::updateIndexStencils(const string& directory, set<const std::string*>& stencils)
{
    MF_DEBUG(endl << "INDEXING stencils DIR: " << directory);
    DIR *dir;
    if((dir = opendir(directory.c_str()))) {
        const struct dirent *entry;
        if((entry = readdir(dir))) {
            string *path;
            do {
                if(entry->d_type != DT_DIR) {
                    MF_DEBUG(endl << "  FILE: " << directory.c_str() << "//" << entry->d_name);
                    path = new string{directory};
                    path->append(FILE_PATH_SEPARATOR);
                    path->append(entry->d_name);
                    if(File::fileHasMarkdownExtension(*path)) {
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

const set<const string*> RepositoryIndexer::getPdfFiles() const {
    return pdfs;
}

const set<const string*> RepositoryIndexer::getTextFiles() const {
    return texts;
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
    path += directory;
    path += FILE_PATH_SEPARATOR;
    path += DIRNAME_MEMORY;
    if(!isDirectoryOrFileExists(path.c_str())) {
        return false;
    }

    return true;
}

Repository* RepositoryIndexer::getRepositoryForPath(const string& p)
{
    MF_DEBUG("Creating repository for path: '" << p << "'" << endl);
    string path{};
    resolvePath(p, path);
    MF_DEBUG("  Resolved absolute path: '" << path << "'" << endl);
    if(isDirectoryOrFileExists(path.c_str())) {
        if(isDirectory(path.c_str())) {
            if(isMindForgerRepository(path)) {
                return new Repository(path);
            } else {
                Repository* r = new Repository{path, Repository::RepositoryType::MARKDOWN};

                string p{path};
                p += FILE_PATH_SEPARATOR;
                p += "README.md";
                if(isDirectoryOrFileExists(p.c_str())) {
                    p.assign(path);
                    p += FILE_PATH_SEPARATOR;
                    p += ".git";
                    if(isDirectoryOrFileExists(p.c_str())) {
                        r->setGithubRepository();
                    }
                }

                return r;
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

std::string RepositoryIndexer::makePathRelative(
        const Repository* repository,
        const std::string& srcAbsoluteFile,
        const std::string& dstAbsolutePath,
        const bool dstIsFile)
{
    MF_DEBUG("Relativize " << srcAbsoluteFile << " > " << dstAbsolutePath << endl);
    // src O is always in the current directory, check whether dst is there as well
    string memoryPath{repository->getDir()};
    // handling for both repository types MINDFORGER/MARKDOWN is almost identical
    if(repository->getMode() == Repository::RepositoryMode::REPOSITORY) {
        if(repository->getType() == Repository::RepositoryType::MINDFORGER) {
            memoryPath+=FILE_PATH_SEPARATOR;
            memoryPath+=DIRNAME_MEMORY;
            memoryPath+=FILE_PATH_SEPARATOR;
        }
        if(stringStartsWith(dstAbsolutePath, memoryPath)) {
            // both src and dst are in the same repository

            // detect: same dir / src parent dst / dst parent src / siblings
            string srcFile{}, srcDir{};
            pathToDirectoryAndFile(srcAbsoluteFile, srcDir, srcFile);
            string dstFile{}, dstDir{};
            if(dstIsFile) {
                pathToDirectoryAndFile(dstAbsolutePath, dstDir, dstFile);
            } else {
                dstDir.assign(dstAbsolutePath);
            }

            // same dir
            if(!srcDir.compare(dstDir)) {
                // same file w/ anchor: #anchor
                size_t anchorIdx = dstFile.find("#");
                if(anchorIdx != string::npos) {
                    string anchor = dstFile.substr(anchorIdx);
                    string anchorHost = dstFile.substr(0, anchorIdx);
                    if(anchor.size()==1) {
                        return anchorHost;
                    }
                    if(!srcFile.compare(anchorHost)) {
                        return anchor;
                    }
                }
                return dstFile;
            } else if(stringStartsWith(dstDir, srcDir)) {
                // src parent of dst > suffix
                string result{dstAbsolutePath};
                result.erase(0, srcDir.size());
                if(result.at(0) == FILE_PATH_SEPARATOR_CHAR) {
                    result.erase(0, 1);
                }
                return result;
            } else if(stringStartsWith(srcDir, dstDir)) {
                // dst parent of src > ../dstFile
                string result{srcAbsoluteFile};
                result.erase(0, dstDir.size());
                if(result.at(0) == FILE_PATH_SEPARATOR_CHAR) {
                    result.erase(0, 1);
                }
                size_t parents = std::count(result.begin(), result.end(), FILE_PATH_SEPARATOR_CHAR);
                result.clear();
                for(size_t i=0; i<parents; i++) {
                    result+="..";
                    result+=FILE_PATH_SEPARATOR;
                }
                result+=dstFile;
                return result;
            } else {
                // different directories next to each other > use repository path as shared root
                // UP from src
                string memoryPath{repository->getDir()};
                if(repository->getType() == Repository::RepositoryType::MINDFORGER) {
                    memoryPath+=FILE_PATH_SEPARATOR;
                    memoryPath+=DIRNAME_MEMORY;
                    memoryPath+=FILE_PATH_SEPARATOR;
                }
                string s{srcAbsoluteFile};
                s.erase(0, memoryPath.size());
                if(s.at(0) == FILE_PATH_SEPARATOR_CHAR) {
                    s.erase(0, 1);
                }
                size_t parents = std::count(s.begin(), s.end(), FILE_PATH_SEPARATOR_CHAR);
                string result{};
                for(size_t i=0; i<parents; i++) {
                    result+="..";
                    result+=FILE_PATH_SEPARATOR;
                }
                // DOWN to dst
                s.assign(dstDir);
                s.erase(0, memoryPath.size());
                if(s.at(0) == FILE_PATH_SEPARATOR_CHAR) {
                    s.erase(0, 1);
                }
                result.append(s);
                if(dstFile.size()) {
                    result+=FILE_PATH_SEPARATOR;
                    result.append(dstFile);
                }
                return result;
            }
        }
        // not in the same repository
        return dstAbsolutePath;
    } else {
        // MF/MD file ~ no repository ~ handle just the same directory, absolute link otherwise
        // same dir
        string srcFile{}, srcDir{};
        pathToDirectoryAndFile(srcAbsoluteFile, srcDir, srcFile);
        string dstFile{}, dstDir{};
        if(dstIsFile) {
            pathToDirectoryAndFile(dstAbsolutePath, dstDir, dstFile);
            if(!srcDir.compare(dstDir)) {
                // same file w/ anchor: #anchor
                size_t anchorIdx = dstFile.find("#");
                if(anchorIdx != string::npos) {
                    string anchor = dstFile.substr(anchorIdx);
                    string anchorHost = dstFile.substr(0, anchorIdx);
                    if(anchor.size()==1) {
                        return anchorHost;
                    }
                    if(!srcFile.compare(anchorHost)) {
                        return anchor;
                    }
                }
                return dstFile;
            }
        }
    }

    return dstAbsolutePath;
}

} // m8r namespace
