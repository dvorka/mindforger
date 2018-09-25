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
const std::string Configuration::DEFAULT_EDITOR_FONT= std::string{UI_DEFAULT_EDITOR_FONT};
const std::string Configuration::DEFAULT_TIME_SCOPE = std::string{"0y0m0d0h0m"};

Configuration::Configuration()
    : installer(new Installer{})
{
    // default config file path: ~/.mindforger.md
    char *home = getenv(ENV_VAR_HOME);
    userHomePath = string{home};
    configFilePath.assign(userHomePath);
    configFilePath += FILE_PATH_SEPARATOR;
    configFilePath += FILENAME_M8R_CONFIGURATION;

    clear();
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

void Configuration::clear()
{
    // repositories
    activeRepository = nullptr;
    for(auto& r:repositories) {
        delete r.second;
    }
    repositories.clear();

    // lib
    mindState = MindState::SLEEPING;
    writeMetadata = true;
    saveReadsMetadata = DEFAULT_SAVE_READS_METADATA;
    timeScopeAsString.assign(DEFAULT_TIME_SCOPE);
    tagsScope.clear();
    markdownQuoteSections = DEFAULT_MD_QUOTE_SECTIONS;

    // Markdown 2 HTML options
    md2HtmlOptions = 0
            // DISCOUNT options:
            | Configuration::MdToHtmlOption::AutolinkOption
            | Configuration::MdToHtmlOption::NoSuperscriptOption // if enabled it BREAKS MathJax

            // EXTRA LIBRARIES options:
            | Configuration::MdToHtmlOption::CodeHighlighting // source code highlighting via offline highlight.js - enabled by default
            //| Configuration::MdToHtmlOption::MathSupport // math expressions support via mathjax.js - disabled by default
            //| Configuration::MdToHtmlOption::DiagramSupport; // diagram support via mermaid.js - disabled by default
            ;

    aaAlgorithm = AssociationAssessmentAlgorithm::WEIGHTED_FTS;
    switch(aaAlgorithm) {
    case AssociationAssessmentAlgorithm::WEIGHTED_FTS:
        asyncMindThreshold = DEFAULT_ASYNC_MIND_THRESHOLD_WEIGHTED_FTS;
        break;
    case AssociationAssessmentAlgorithm::BOW:
        asyncMindThreshold = DEFAULT_ASYNC_MIND_THRESHOLD_BOW;
        break;
    }

    distributorSleepInterval = DEFAULT_DISTRIBUTOR_SLEEP_INTERVAL;

    // GUI
    uiNerdTargetAudience = false;
    uiViewerShowMetadata = true;
    uiEditorSyntaxHighlighting = true;
    uiEditorAutocomplete = true;
    uiEditorLineNumbers = true;
    uiEditorTabsAsSpaces = DEFAULT_EDITOR_TABS_AS_SPACES;
    uiEditorTabWidth = DEFAULT_EDITOR_TAB_WIDTH;
    uiThemeName.assign(UI_DEFAULT_THEME);
    uiHtmlCssPath.assign(UI_DEFAULT_HTML_CSS_THEME);
    uiFontPointSize = UI_DEFAULT_FONT_POINT_SIZE;    
    uiEnableDiagramsInMd = JavaScriptLibSupport::NO;
    uiShowToolbar = DEFAULT_UI_SHOW_TOOLBAR;
    uiNerdTargetAudience = DEFAULT_UI_NERD_MENU;
    navigatorMaxNodes = DEFAULT_NAVIGATOR_MAX_GRAPH_NODES;
}

Repository* Configuration::addRepository(Repository* repository)
{
    Repository* clash;
    if((clash=repositories[repository->getPath()]) != nullptr) {
        // deleting clashing repository and update active repository (if needed)
        repositories.erase(repository->getPath());
        if(activeRepository == clash) {
            activeRepository = repository;
        }
        delete clash;
    }

    repositories[repository->getPath()] = repository;

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
        if(repositories.find(repository->getPath()) != repositories.end()) {
            activeRepository = repository;

            memoryPath.clear();
            memoryPath += activeRepository->getDir();

            limboPath.clear();
            limboPath += activeRepository->getDir();

            if(repository->getType()==Repository::RepositoryType::MINDFORGER && repository->getMode()==Repository::RepositoryMode::REPOSITORY) {
                memoryPath+=FILE_PATH_SEPARATOR;
                memoryPath+=FILE_PATH_MEMORY;

                limboPath+=FILE_PATH_SEPARATOR;
                limboPath+=FILE_PATH_LIMBO;
            }
        } else {
            throw MindForgerException{"Active repository must be one of repositories known to Configuration!"};
        }
    } else {
        activeRepository = nullptr;
    }
}

bool Configuration::createEmptyMarkdownFile(const string& file)
{
    if(file.size() && file.find(FILE_PATH_SEPARATOR)==string::npos && RepositoryIndexer::fileHasMarkdownExtension(file)) {
        // as it is filename w/o path I can try to create empty O in the current directory
        stringToFile(file, DEFAULT_NEW_OUTLINE);
        return true;
    } else {
        return false;
    }
}

void Configuration::findOrCreateDefaultRepository()
{
    if(!activeRepository || activeRepository->getDir().empty()) {
        string defaultRepositoryPath{userHomePath};
        defaultRepositoryPath += FILE_PATH_SEPARATOR;
        defaultRepositoryPath += DIRNAME_M8R_REPOSITORY;
        MF_DEBUG("Checking for default repository existence: " << defaultRepositoryPath << endl);
        if(isDirectoryOrFileExists(defaultRepositoryPath.c_str())) {
            setActiveRepository(addRepository(RepositoryIndexer::getRepositoryForPath(defaultRepositoryPath)));
        } else {
            // create default repository w/ default content using Installer class
            MF_DEBUG("  Creating a default MF repository in " << defaultRepositoryPath << endl);
            if(installer->createEmptyMindForgerRepository(defaultRepositoryPath)) {
                installer->initMindForgerRepository(true, true, defaultRepositoryPath.c_str());
            }
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

} // m8r namespace
