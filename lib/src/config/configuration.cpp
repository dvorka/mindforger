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

using namespace std;

namespace m8r {

Configuration::Configuration()
{
    char *home = getenv(ENV_VAR_HOME);
    userHomePath = string{home};
    const char* r{};
    if((r=getRepositoryFromEnv()) != nullptr) {
        string* repository = new string{r};
        addRepository(repository);
        setActiveRepository(repository);
    } else {
        activeRepository = nullptr;
    }

    fontPointSize = DEFAULT_FONT_POINT_SIZE;

    // IMPROVE labels/types/tags might become extensible and can be loaded from configuration and memory content
}



Configuration::Configuration(const string& activeRepository)
{
    Configuration();
    setActiveRepository(addRepository(activeRepository));
}

void Configuration::addRepository(const string* repositoryPath)
{
    repositories.insert(repositoryPath);
}

const string* Configuration::addRepository(const string& repositoryPath)
{
    const string* result = new string{repositoryPath};
    addRepository(result);
    return result;
}

const string* Configuration::getActiveRepository(void) const
{
    return activeRepository;
}

std::set<const std::string*>& Configuration::getRepositories(void)
{
    return repositories;
}

void Configuration::setActiveRepository(const string* repositoryPath)
{
    if(repositories.find(repositoryPath)!=repositories.end()) {
        activeRepository = repositoryPath;

        memoryPath.clear();
        memoryPath.append(*activeRepository);
        memoryPath+=FILE_PATH_SEPARATOR;
        memoryPath+=FILE_PATH_MEMORY;

        limboPath.clear();
        limboPath.append(*activeRepository);
        limboPath+=FILE_PATH_SEPARATOR;
        limboPath+=FILE_PATH_LIMBO;
    } else {
        throw MindForgerException{"Active repository must be one of repositories known to Configuration!"};
    }
}

void Configuration::load(void)
{
    const string filePath = getConfigFileName();
    Markdown configMarkdown{&filePath};
    load(configMarkdown.getAst());
}

void Configuration::load(const vector<MarkdownAstNodeSection*>* ast)
{
    // IMPROVE remove ast param if NOT needed
    UNUSED_ARG(ast);

    // Search for repository in the following order:
    //   1. CLI argument - active repository would be set by main() on configuration
    //   2. Parse Configuration file ~/.mindforger
    // TODO parsing and loading of configuration

    //   3. Environment variable - is set up in configuration's constructor
    //   4. If CLI, config and environment didn't provide repository location, then check default location
    if(!activeRepository || (*activeRepository).empty()) {
        string defaultRepositoryPath{userHomePath};
        defaultRepositoryPath.append(FILE_PATH_SEPARATOR);
        defaultRepositoryPath.append(DIRNAME_M8R_REPOSITORY);
        MFDEBUG("Checking default repository existence: " << defaultRepositoryPath);
        if(isDirectoryExist(defaultRepositoryPath.c_str())) {
            setActiveRepository(addRepository(defaultRepositoryPath));
        } else {
	    // IMPROVE create default repository w/ default content using Installer class
            MFDEBUG("  ... doesn't exist > no repository found > running MF w/o repository");
        }
    }
}

void Configuration::save(void) const
{
    // TODO to be implemented
}

Configuration::~Configuration()
{
    for(const string* r:repositories) delete r;
}

const char* Configuration::getRepositoryFromEnv()
{
    char *repository = getenv(ENV_VAR_M8R_REPOSITORY);  // this is not leak (static reusable array)
    return repository;
}

const char* Configuration::getEditorFromEnv()
{
    char *editor = getenv(ENV_VAR_M8R_EDITOR);  // this is not leak (static reusable array)
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
