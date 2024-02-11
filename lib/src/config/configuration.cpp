/*
 configuration.cpp     M8r configuration management

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

#include "configuration.h"

#ifdef _WIN32
  #include <ShlObj.h>
  #include <KnownFolders.h>
#endif // _WIN32

using namespace std;
using namespace m8r::filesystem;

namespace m8r {

const string KnowledgeTool::TOOL_PHRASE = string{"<<PHRASE>>"};

// non-primitive constants initializations
const string Configuration::DEFAULT_ACTIVE_REPOSITORY_PATH = string{FILE_PATH_M8R_REPOSITORY};
const string Configuration::DEFAULT_STARTUP_VIEW_NAME = string{DEFAULT_STARTUP_VIEW};
const string Configuration::DEFAULT_UI_THEME_NAME = string{UI_DEFAULT_THEME};
const string Configuration::DEFAULT_UI_HTML_CSS_THEME = string{UI_DEFAULT_HTML_CSS_THEME};
const string Configuration::DEFAULT_EDITOR_FONT= string{UI_DEFAULT_EDITOR_FONT};
const string Configuration::DEFAULT_TIME_SCOPE = string{"0y0m0d0h0m"};
const string Configuration::DEFAULT_WINGMAN_LLM_MODEL_OPENAI = string{"gpt-3.5-turbo"};

Configuration::Configuration()
    : asyncMindThreshold{},
      activeRepository{},
      repositories{},
      repositoryConfiguration{getDummyRepositoryConfiguration()},
      writeMetadata{},
      saveReadsMetadata{},
      autolinking{DEFAULT_AUTOLINKING},
      autolinkingColonSplit{},
      autolinkingCaseInsensitive{},
      wingmanProvider{DEFAULT_WINGMAN_LLM_PROVIDER},
      wingmanApiKey{},
      wingmanLlmModel{DEFAULT_WINGMAN_LLM_MODEL_OPENAI},
      md2HtmlOptions{},
      distributorSleepInterval{DEFAULT_DISTRIBUTOR_SLEEP_INTERVAL},
      markdownQuoteSections{},
      recentIncludeOs{DEFAULT_RECENT_INCLUDE_OS},
      uiNerdTargetAudience{DEFAULT_UI_NERD_MENU},
      uiHtmlZoom{},
      externalEditorCmd{},
      uiFontPointSize{},
      uiShowBreadcrump{},
      uiViewerShowMetadata{},
      uiEditorTabWidth{DEFAULT_EDITOR_TAB_WIDTH},
      uiEditorLineNumbers{},
      uiEditorSyntaxHighlighting{DEFAULT_EDITOR_SYNTAX_HIGHLIGHT},
      uiEditorLiveSpellCheck{DEFAULT_SPELLCHECK_LIVE},
      uiEditorSpellCheckLanguage{},
      uiEditorSpellCheckLanguages{},
      uiEditorAutocomplete{DEFAULT_EDITOR_AUTOCOMPLETE},
      uiEditorSmartEditor{DEFAULT_EDITOR_SMART_EDITOR},
      uiEditorSpaceSectionEscaping{DEFAULT_EDITOR_SPACE_SECTION_ESCAPING},
      navigatorMaxNodes{},
      uiEditorTabsAsSpaces{},
      uiEditorAutosave{DEFAULT_EDITOR_AUTOSAVE},
      uiFullOPreview{DEFAULT_FULL_O_PREVIEW},
      uiShowToolbar{DEFAULT_UI_SHOW_TOOLBAR},
      uiExpertMode{DEFAULT_UI_EXPERT_MODE},
      uiAppFontSize{DEFAULT_UI_APP_FONT_SIZE},
      uiDistractionFreeMode{},
      uiHoistedMode{},
      uiLiveNotePreview{DEFAULT_UI_LIVE_NOTE_PREVIEW},
      uiOsTableSortColumn{DEFAULT_OS_TABLE_SORT_COLUMN},
      uiOsTableSortOrder{DEFAULT_OS_TABLE_SORT_ORDER},
      uiDoubleClickNoteViewToEdit{DEFAULT_CLICK_NOTE_VIEW_TO_EDIT},
      installer(new Installer{})
{
    // default config file path: ~/.mindforger.md

    userHomePath = getHomeDirectoryPath();

    configFilePath.assign(userHomePath);
    configFilePath += FILE_PATH_SEPARATOR;
    configFilePath += FILENAME_M8R_CONFIGURATION;

#ifdef _WIN32
    PWSTR wpath;
    size_t num;

    SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &wpath);
    char* docPath = new char[MAX_PATH];
    wcstombs_s(&num, docPath, MAX_PATH, wpath, MAX_PATH);
    CoTaskMemFree(wpath);
    userDocPath = string{docPath};

    delete [] docPath;
#else
    userDocPath = string{userHomePath};
#endif //_WIN32

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

    clearRepositoryConfiguration();
    repositoryConfiguration = nullptr;
}

void Configuration::clear()
{
    // repositories
    activeRepository = nullptr;
    for(auto& r:repositories) {
        delete r.second;
    }
    repositories.clear();

    clearRepositoryConfiguration();

    // lib
    mindState = MindState::SLEEPING;
    writeMetadata = true;
    saveReadsMetadata = DEFAULT_SAVE_READS_METADATA;
    autolinking = DEFAULT_AUTOLINKING;
    autolinkingColonSplit = DEFAULT_AUTOLINKING_COLON_SPLIT;
    autolinkingCaseInsensitive = DEFAULT_AUTOLINKING_CASE_INSENSITIVE;
    wingmanProvider = DEFAULT_WINGMAN_LLM_PROVIDER;
    wingmanApiKey.clear();
    wingmanLlmModel.clear();
    timeScopeAsString.assign(DEFAULT_TIME_SCOPE);
    tagsScope.clear();
    markdownQuoteSections = DEFAULT_MD_QUOTE_SECTIONS;
    recentIncludeOs = DEFAULT_RECENT_INCLUDE_OS;

    // Markdown 2 HTML options
    md2HtmlOptions = 0
        // DISABLED highlighting and extensions support is consequence of HTML preview flickering when =-v is enabled
        //| MdToHtmlOption::CodeHighlighting // source code highlighting via offline highlight.js - disabled by default
        //| MdToHtmlOption::MathSupport // math expressions support via mathjax.js - disabled by default
        //| MdToHtmlOption::DiagramSupport; // diagram support via mermaid.js - disabled by default
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
    uiEditorLiveSpellCheck = DEFAULT_SPELLCHECK_LIVE;
    uiEditorAutocomplete = true;
    uiEditorLineNumbers = true;
    uiEditorTabsAsSpaces = DEFAULT_EDITOR_TABS_AS_SPACES;
    uiEditorAutosave = DEFAULT_EDITOR_AUTOSAVE;
    uiEditorTabWidth = DEFAULT_EDITOR_TAB_WIDTH;
    uiEditorSmartEditor = DEFAULT_EDITOR_SMART_EDITOR;
    uiEditorSpaceSectionEscaping = DEFAULT_EDITOR_SPACE_SECTION_ESCAPING;
#ifdef _WIN32
    uiEditorKeyBinding = EditorKeyBindingMode::WINDOWS;
#else
    uiEditorKeyBinding = EditorKeyBindingMode::EMACS;
#endif
    startupView.assign(DEFAULT_STARTUP_VIEW);
    uiThemeName.assign(UI_DEFAULT_THEME);
    uiHtmlCssPath.assign(UI_DEFAULT_HTML_CSS_THEME);
    uiHtmlZoom = DEFAULT_UI_HTML_ZOOM;
    externalEditorCmd = "";
    uiFontPointSize = UI_DEFAULT_FONT_POINT_SIZE;
    uiEnableDiagramsInMd = JavaScriptLibSupport::NO;
    uiNerdTargetAudience = DEFAULT_UI_NERD_MENU;
    navigatorMaxNodes = DEFAULT_NAVIGATOR_MAX_GRAPH_NODES;
    uiShowToolbar = DEFAULT_UI_SHOW_TOOLBAR;
    uiExpertMode = DEFAULT_UI_EXPERT_MODE;
    uiAppFontSize = DEFAULT_UI_APP_FONT_SIZE;
    uiDistractionFreeMode = false;
    uiHoistedMode = false;
    uiLiveNotePreview = DEFAULT_UI_LIVE_NOTE_PREVIEW;
    uiOsTableSortColumn = DEFAULT_OS_TABLE_SORT_COLUMN;
    uiOsTableSortOrder = DEFAULT_OS_TABLE_SORT_ORDER;
    uiDoubleClickNoteViewToEdit = DEFAULT_CLICK_NOTE_VIEW_TO_EDIT;
}

bool Configuration::hasRepositoryConfiguration() const {
    return this->repositoryConfiguration != getDummyRepositoryConfiguration();
}

RepositoryConfiguration& Configuration::initRepositoryConfiguration(
    Organizer* defaultOrganizer
) {
    clearRepositoryConfiguration();
    this->repositoryConfiguration = new RepositoryConfiguration{};
    if(defaultOrganizer) {
        this->repositoryConfiguration->addOrganizer(defaultOrganizer);
    }
    return *this->repositoryConfiguration;
}

void Configuration::clearRepositoryConfiguration() {
    if(repositoryConfiguration
       && repositoryConfiguration != getDummyRepositoryConfiguration()
    ) {
        delete repositoryConfiguration;
    } else {
        getDummyRepositoryConfiguration()->clear();
    }
    repositoryConfiguration = getDummyRepositoryConfiguration();
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
    }

    throw MindForgerException{"Active repository not set!"};
}

std::map<const std::string,Repository*>& Configuration::getRepositories()
{
    return repositories;
}

void Configuration::setActiveRepository(
    Repository* repository,
    RepositoryConfigurationPersistence& persistence
) {
    if(repository) {
        if(repositories.find(repository->getPath()) != repositories.end()) {
            activeRepository = repository;

            memoryPath.clear();
            memoryPath += activeRepository->getDir();

            mindPath.clear();
            mindPath += activeRepository->getDir();

            outlinesMapPath.clear();

            limboPath.clear();
            limboPath += activeRepository->getDir();

            if(repository->getType()==Repository::RepositoryType::MINDFORGER
                 &&
               repository->getMode()==Repository::RepositoryMode::REPOSITORY)
            {
                memoryPath+=FILE_PATH_SEPARATOR;
                memoryPath+=DIRNAME_MEMORY;

                mindPath+=FILE_PATH_SEPARATOR;
                mindPath+=DIRNAME_MIND;

                outlinesMapPath+=mindPath;
                outlinesMapPath+=FILE_PATH_SEPARATOR;
                outlinesMapPath+=FILENAME_OUTLINES_MAP;

                limboPath+=FILE_PATH_SEPARATOR;
                limboPath+=DIRNAME_LIMBO;

                // setting ACTIVE repository means that repository SPECIFIC configuration must be loaded
                this->initRepositoryConfiguration(EisenhowerMatrix::createEisenhowerMatrixOrganizer());
                persistence.load(*this);
            } else {
                this->clearRepositoryConfiguration();
            }
        } else {
            throw MindForgerException{"Active repository must be one of repositories known to the configuration!"};
        }
    } else {
        activeRepository = nullptr;
        clearRepositoryConfiguration();
    }
}

bool Configuration::createEmptyMarkdownFile(const string& file)
{
    if(!file.empty()
       && file.find(FILE_PATH_SEPARATOR)==string::npos
       && File::fileHasMarkdownExtension(file)
    ) {
        // as it is filename w/o path I can try to create empty O in the current directory
        stringToFile(file, DEFAULT_NEW_OUTLINE);
        return true;
    }

    return false;
}

void Configuration::findOrCreateDefaultRepository(RepositoryConfigurationPersistence& persistence)
{
    if(!activeRepository || activeRepository->getDir().empty()) {
        string defaultRepositoryPath{userDocPath};
        defaultRepositoryPath += FILE_PATH_SEPARATOR;
        defaultRepositoryPath += DIRNAME_M8R_REPOSITORY;
        MF_DEBUG("Checking for default repository existence: " << defaultRepositoryPath << endl);
        if(isDirectoryOrFileExists(defaultRepositoryPath.c_str())) {
            setActiveRepository(
                addRepository(RepositoryIndexer::getRepositoryForPath(defaultRepositoryPath)),
                persistence
            );
        } else {
            // create default repository w/ default content using Installer class
            MF_DEBUG("  Creating default MF repository in " << defaultRepositoryPath << endl);
            if(installer->createEmptyMindForgerRepository(defaultRepositoryPath)) {
                installer->initMindForgerRepository(true, true, defaultRepositoryPath.c_str());
            }
            if(!activeRepository) {
                setActiveRepository(
                    addRepository(new Repository(defaultRepositoryPath)),
                    persistence
                );
            }
        }
    }
}

string Configuration::getRepositoryConfigFilePath() const {
    if(activeRepository
       && activeRepository->getType() == Repository::RepositoryType::MINDFORGER
       && activeRepository->getMode() == Repository::RepositoryMode::REPOSITORY
    ) {
        std::string path{activeRepository->getPath()};
        path += FILE_PATH_SEPARATOR;
        path += FILENAME_M8R_REPOSITORY_CONFIGURATION;
        return path;
    }

    return "";
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

bool Configuration::canWingmanOpenAi()
{
    return std::getenv(ENV_VAR_OPENAI_API_KEY) != nullptr?true:false;
}

void Configuration::setWingmanLlmProvider(WingmanLlmProviders provider)
{
    MF_DEBUG(
        "Configuration::setWingmanLlmProvider(): "
        << std::to_string(provider) << endl);

    wingmanProvider = provider;

    // try to initialize Wingman @ given LLM provider,
    // if it fails, then set it to false ~ disabled Wingman
    initWingman();
}

bool Configuration::initWingmanMock()
{
    if(canWingmanMock()) {
        wingmanApiKey.clear();
        wingmanLlmModel.clear();
        return true;
    }

    return false;
}

/**
 * @brief Check whether OpenAI Wingman requirements are satisfied.
 *
 * In case that the requirements are satisfied (OpenAI API key provided),
 * then Wingman @ OpenAI can be choosen @ Mindforger configuration.
*/
bool Configuration::initWingmanOpenAi() {
    MF_DEBUG("  Configuration::initWingmanOpenAi()" << endl);
    if(canWingmanOpenAi()) {
        MF_DEBUG(
            "    Wingman OpenAI API key found in the shell environment variable "
            "MINDFORGER_OPENAI_API_KEY" << endl);
        const char* apiKeyEnv = std::getenv(ENV_VAR_OPENAI_API_KEY);
        MF_DEBUG("    Wingman API key loaded from the env: " << apiKeyEnv << endl);
        wingmanApiKey = apiKeyEnv;
        wingmanLlmModel = DEFAULT_WINGMAN_LLM_MODEL_OPENAI;
        wingmanProvider = WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI;
        return true;
    }

    MF_DEBUG(
        "    Wingman OpenAI API key NOT found in the environment variable "
        "MINDFORGER_OPENAI_API_KEY" << endl);
    wingmanApiKey.clear();
    wingmanLlmModel.clear();
    wingmanProvider = WingmanLlmProviders::WINGMAN_PROVIDER_NONE;
    return false;
}

bool Configuration::initWingman()
{
    MF_DEBUG(
        "  BEFORE Configuration::initWingman():" << endl <<
        "    LLM provider: " << wingmanProvider << endl <<
        "    OpenAI API key env var name: " << ENV_VAR_OPENAI_API_KEY << endl <<
        "    Wingman provider API key   : " << wingmanApiKey << endl
    );

    bool initialized = false;

    switch (wingmanProvider) {
    case WingmanLlmProviders::WINGMAN_PROVIDER_NONE:
        MF_DEBUG("  NONE Wingman CONFIGURED" << endl);
        return true;
#ifdef MF_WIP
    case WingmanLlmProviders::WINGMAN_PROVIDER_MOCK:
        MF_DEBUG("  MOCK Wingman provider CONFIGURED" << endl);
        initialized = initWingmanMock();
        break;
#endif
    case WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI:
        MF_DEBUG("  OpenAI Wingman provider CONFIGURED" << endl);
        initialized = initWingmanOpenAi();
        break;
    default:
        MF_DEBUG(
            "  ERROR: unable to CONFIGURE UNKNOWN Wingman provider: "
            << wingmanProvider << endl);
        initialized = false;
    }

    if(!initialized) {
        wingmanProvider = WingmanLlmProviders::WINGMAN_PROVIDER_NONE;
        wingmanApiKey.clear();
        wingmanLlmModel.clear();
    }

    MF_DEBUG(
        "  BEFORE Configuration::initWingman():" << endl <<
        "    LLM provider: " << wingmanProvider << endl <<
        "    OpenAI API key env var name: " << ENV_VAR_OPENAI_API_KEY << endl <<
        "    Wingman provider API key   : " << wingmanApiKey << endl
    );

    return initialized;
}

/**
 * @brief Re-initialize Wingman using configured LLM provider and return
 * whether it is ready to be used.
 */
bool Configuration::isWingman() {
    return WingmanLlmProviders::WINGMAN_PROVIDER_NONE==wingmanProvider?false:true;
}

} // m8r namespace
