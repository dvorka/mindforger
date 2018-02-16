/*
 configuration.cpp     M8r configuration management

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

#include "configuration.h"

#include <iostream>

namespace m8r {

using namespace std;

Configuration::Configuration()
    : installer(new Installer{})
{
    char *home = getenv(ENV_VAR_HOME);
    userHomePath = string{home};

    // repositories - environment variable defined repository path to be used only if desired

    // lib
    writeMetadata = true;
    rememberReads = false;
    externalEditorPath = nullptr;

    // GUI
    viewerShowMetadata = true;
    editorEnableSyntaxHighlighting = true;
    editorShowLineNumbers = true;
    fontPointSize = DEFAULT_FONT_POINT_SIZE;
}


Configuration::~Configuration()
{
    for(auto& r:repositories) {
        delete r.second;
    }
    repositories.clear();

    if(installer) {
        delete installer;
    }
}

Repository* Configuration::addRepository(Repository* repository)
{
    repositories[repository->getPath()] = repository; // overwrites item map under given key (map::insert keeps existing)
    return repository;
}

Repository* Configuration::getActiveRepository() const
{
    if(activeRepository) {
        return activeRepository;
    } else {
        throw MindForgerException{"Active repository not set!"};
    }
}

std::map<const std::string,Repository*>& Configuration::getRepositories()
{
    return repositories;
}

void Configuration::setActiveRepository(Repository* repository)
{
    if(repositories.find(repository->getPath()) != repositories.end()) {
        activeRepository = repository;

        memoryPath.clear();
        memoryPath.append(activeRepository->getPath());
        memoryPath+=FILE_PATH_SEPARATOR;
        memoryPath+=FILE_PATH_MEMORY;

        limboPath.clear();
        limboPath.append(activeRepository->getPath());
        limboPath+=FILE_PATH_SEPARATOR;
        limboPath+=FILE_PATH_LIMBO;
    } else {
        throw MindForgerException{"Active repository must be one of repositories known to Configuration!"};
    }
}

void Configuration::findOrCreateDefaultRepository()
{
    if(!activeRepository || activeRepository->getPath().empty()) {
        string defaultRepositoryPath{userHomePath};
        defaultRepositoryPath.append(FILE_PATH_SEPARATOR);
        defaultRepositoryPath.append(DIRNAME_M8R_REPOSITORY);
        MF_DEBUG("Checking for default repository existence: " << defaultRepositoryPath << endl);
        if(isDirectoryOrFileExists(defaultRepositoryPath.c_str())) {
            setActiveRepository(addRepository(RepositoryIndexer::getRepositoryForPath(defaultRepositoryPath)));
        } else {
            // create default repository w/ default content using Installer class
            MF_DEBUG("  Creating a default MF repository in " << defaultRepositoryPath << endl);
            installer->createEmptyMindForgerRepository(defaultRepositoryPath);
            if(!activeRepository) {
                setActiveRepository(addRepository(new Repository(defaultRepositoryPath)));
            }
        }
    }
}

void Configuration::load()
{
    const string filePath = getConfigFileName();
    Markdown configMarkdown{&filePath};
    load(configMarkdown.getAst());
}

void Configuration::load(const vector<MarkdownAstNodeSection*>* ast)
{
    UNUSED_ARG(ast);

    // TODO deserialize AST to this instance

    findOrCreateDefaultRepository();
}

void Configuration::save() const
{
    // TODO serialize this instance directly to MD file using streams
}

const char* Configuration::getRepositoryPathFromEnv()
{
    char* repository = getenv(ENV_VAR_M8R_REPOSITORY);  // this is not leak (static reusable array)
    return repository;
}

const char* Configuration::getEditorFromEnv()
{
    char* editor = getenv(ENV_VAR_M8R_EDITOR);  // this is not leak (static reusable array)
    return editor;
}

const string Configuration::getConfigFileName()
{
    string filePath{userHomePath};
    filePath.append(FILE_PATH_SEPARATOR);
    filePath.append(FILENAME_M8R_CONFIGURATION);
    return std::move(filePath);
}

} // namespace
