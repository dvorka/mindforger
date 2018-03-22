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

namespace m8r {

using namespace std;

const std::string Configuration::DEFAULT_ACTIVE_REPOSITORY_PATH = std::string{FILE_PATH_M8R_REPOSITORY};
const std::string Configuration::DEFAULT_UI_THEME_NAME = std::string{UI_DEFAULT_THEME};
const std::string Configuration::DEFAULT_UI_HTML_CSS_THEME = std::string{UI_DEFAULT_HTML_CSS_THEME};
const std::string Configuration::DEFAULT_EDITOR_KEY_BINDING= std::string{UI_DEFAULT_EDITOR_KEY_BINDING};
const std::string Configuration::DEFAULT_TIME_SCOPE = std::string{"0y0m0d0h0m"};

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

    reset();
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

void Configuration::reset()
{
    // repositories
    activeRepository = nullptr;
    for(auto& r:repositories) {
        delete r.second;
    }
    repositories.clear();

    // lib
    writeMetadata = true;
    saveReadsMetadata = DEFAULT_SAVE_READS_METADATA;
    timeScopeAsString.assign(DEFAULT_TIME_SCOPE);
    md2HtmlOptions = 0
    // change default options by enabling flags below
    //        | Configuration::MdToHtmlOption::AutolinkOption
    //        | Configuration::MdToHtmlOption::NoStrikethroughOption
    //        | Configuration::MdToHtmlOption::NoAlphaListOption
    //        | Configuration::MdToHtmlOption::NoDefinitionListOption
    //        | Configuration::MdToHtmlOption::NoSmartypantsOption
    //        | Configuration::MdToHtmlOption::ExtraFootnoteOption
    //        | Configuration::MdToHtmlOption::NoSuperscriptOption;
            | Configuration::MdToHtmlOption::MathSupport
            | Configuration::MdToHtmlOption::MathInlineSupport
            | Configuration::MdToHtmlOption::CodeHighlighting
            | Configuration::MdToHtmlOption::DiagramSupport;

    // GUI
    uiViewerShowMetadata = true;
    uiEditorEnableSyntaxHighlighting = true;
    uiEditorShowLineNumbers = true;
    uiThemeName.assign(UI_DEFAULT_THEME);
    uiHtmlCssPath.assign(UI_DEFAULT_HTML_CSS_THEME);
    uiFontPointSize = UI_DEFAULT_FONT_POINT_SIZE;
    uiShowNotebookEditButton = DEFAULT_SHOW_NOTEBOOK_EDIT_BUTTON;
}

void Configuration::setMindState(MindState mindState)
{
    this->mindState = mindState;
}

Repository* Configuration::addRepository(Repository* repository)
{
    Repository* clash;
    if((clash=repositories[repository->getPATH()]) != nullptr) {
        // deleting clashing repository and update active repository (if needed)
        repositories.erase(repository->getPATH());
        if(activeRepository == clash) {
            activeRepository = repository;
            delete clash;

        }
    }

    repositories[repository->getPATH()] = repository;

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
    if(repository) {
        if(repositories.find(repository->getPATH()) != repositories.end()) {
            activeRepository = repository;

            memoryPath.clear();
            memoryPath.append(activeRepository->getDir());
            memoryPath+=FILE_PATH_SEPARATOR;
            memoryPath+=FILE_PATH_MEMORY;

            limboPath.clear();
            limboPath.append(activeRepository->getDir());
            limboPath+=FILE_PATH_SEPARATOR;
            limboPath+=FILE_PATH_LIMBO;
        } else {
            throw MindForgerException{"Active repository must be one of repositories known to Configuration!"};
        }
    } else {
        activeRepository = nullptr;
    }
}

void Configuration::findOrCreateDefaultRepository()
{
    if(!activeRepository || activeRepository->getDir().empty()) {
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
