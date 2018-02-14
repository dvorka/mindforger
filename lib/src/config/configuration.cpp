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
    installer = new Installer{};

    char *home = getenv(ENV_VAR_HOME);
    userHomePath = string{home};

    // repositories
    const char* r{};
    if((r=getRepositoryFromEnv()) != nullptr) {
        string* repositoryPath = new string{r};
        // TODO check repository type: MF/MD, RW by default
        setActiveRepository(addRepository(repositoryPath));
    } else {
        activeRepository = nullptr;
    }

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
    for(const string* r:repositories) {
        delete r;
    }
    repositories.clear();

    if(installer) {
        delete installer;
        installer = nullptr;
    }
}

xxx fix
const string* Configuration::addRepository(const string* repositoryPath)
{
    repositories.insert(repositoryPath);
    return repositoryPath;
}

xxx fix
const string* Configuration::addRepository(const string& repositoryPath)
{
    const string* result = new string{repositoryPath};
    addRepository(result);
    return result;
}

const string* Configuration::getActiveRepository() const
{
    if(activeRepository) {
        return activeRepository;
    } else {
        throw MindForgerException{"Active repository not set!"};
    }
}

xxx fix
std::set<const std::string*>& Configuration::getRepositories()
{
    return repositories;
}

xxx fix
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

void Configuration::findOrCreateDefaultRepository()
{
    if(!activeRepository || (*activeRepository).empty()) {
        string defaultRepositoryPath{userHomePath};
        defaultRepositoryPath.append(FILE_PATH_SEPARATOR);
        defaultRepositoryPath.append(DIRNAME_M8R_REPOSITORY);
        MF_DEBUG("Checking for default repository existence: " << defaultRepositoryPath << endl);
        if(isDirectoryExist(defaultRepositoryPath.c_str())) {
            setActiveRepository(addRepository(defaultRepositoryPath));
        } else {
            // create default repository w/ default content using Installer class
            MF_DEBUG("  Creating a default MF repository in " << defaultRepositoryPath << endl);
            installer->createEmptyMindForgerRepository(defaultRepositoryPath);
            if(!activeRepository) {
                string* r = new string{defaultRepositoryPath};
                addRepository(r);
                setActiveRepository(r);
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

const char* Configuration::getRepositoryFromEnv()
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
