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
    mindState = MindState::SLEEPING;

    char *home = getenv(ENV_VAR_HOME);
    userHomePath = string{home};

    configFilePath.assign(userHomePath);
    configFilePath.append(FILE_PATH_SEPARATOR);
    configFilePath.append(FILENAME_M8R_CONFIGURATION);

    // repositories - environment variable defined repository path to be used only if desired

    // lib
    writeMetadata = true;
    rememberReads = false;

    // GUI
    uiViewerShowMetadata = true;
    uiEditorEnableSyntaxHighlighting = true;
    uiEditorShowLineNumbers = true;
    uiThemeName.assign(UI_DEFAULT_THEME);
    uiFontPointSize = UI_DEFAULT_FONT_POINT_SIZE;
}

Configuration::~Configuration()
{
    for(auto& r:repositories) {
        delete r.second;
    }
    repositories.clear();

    if(installer) {
        delete installer;
        installer = nullptr;
    }
}

void Configuration::setMindState(MindState mindState)
{
    this->mindState = mindState;
}

Repository* Configuration::addRepository(Repository* repository)
{
    if(repositories[repository->getPath()] != nullptr) {
        // deleting clashing repository
        delete repositories[repository->getPath()];
    }
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
    MF_DEBUG("Loading configuration from " << configFilePath);
    if(isDirectoryOrFileExists(configFilePath.c_str())) {
        Markdown configMarkdown{&configFilePath};
        load(configMarkdown.getAst());
    } else {
        save();
    }
}

void Configuration::load(const vector<MarkdownAstNodeSection*>* ast)
{
    UNUSED_ARG(ast);

    // TODO deserialize AST to this instance
}

void Configuration::save() const
{
    MF_DEBUG("Saving configuration to " << configFilePath);

    stringstream s{};
    // IMPROVE build more in compile time and less in runtime
    s <<
         "# MindForger Configuration" << endl <<
         endl <<
         "This is MindForger configuration file (Markdown hosted DSL)." << endl <<
         "See documentation for configuration options details." << endl <<
         endl <<
         "# Settings" << endl <<
         "Application settings:" << endl <<
         endl <<
         "* Theme: " << uiThemeName << endl <<
         "    * Examples: dark, light" << endl <<
         "* Forget: " << "0y0m0d0h0m" << endl <<
         "    * Examples: 2y0m0d0h0m (recent 2 years), 0y3m15d0h0m (recent 3 months and 15 days)" << endl <<
         endl <<
         endl <<
         "# Repositories" << endl <<
         endl <<
         "If MindForger detects MindForger repository structure, then the directory is" << endl <<
         "threated as knowledge base, else it's used as a Markdown directory (e.g. metadata"  << endl <<
         "are not stored to Markdown files)." << endl <<
         endl <<
         "* Active repository: ~/mindforger-repository" << endl <<
         "* Repository: ~/mindforger-repository" << endl <<
         endl;

    std::ofstream out(configFilePath);
    out << s.str();
    out.close();
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

} // namespace
