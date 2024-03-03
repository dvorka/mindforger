/*
 configuration.h     M8r configuration management

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

// TODO this class to be rewritten using templates and dynamic configuration
//   so that adding of new configuration is easier and more robust,
//   ideally there should be a configuration runtime configured by
//   a data structure ~ list of configuration items

#ifndef M8R_CONFIGURATION_H_
#define M8R_CONFIGURATION_H_

#include <set>
#include <string>
#include <cstdlib>
#include <cstdio>

#include "repository.h"
#include "time_scope.h"
#include "./repository_configuration.h"
#include "../repository_indexer.h"
#include "../gear/lang_utils.h"
#include "../gear/file_utils.h"
#include "../exceptions.h"
#include "../model/tag.h"
#include "../model/eisenhower_matrix.h"
#include "../persistence/configuration_persistence.h"
#include "../install/installer.h"
#include "../representations/markdown/markdown_transcoder.h"

namespace m8r {

enum WingmanLlmProviders {
    WINGMAN_PROVIDER_NONE,
    WINGMAN_PROVIDER_MOCK,
    WINGMAN_PROVIDER_OPENAI,
    WINGMAN_PROVIDER_OLLAMA
    // TODO WINGMAN_PROVIDER_GOOGLE,
};

// const in constexpr makes value const
constexpr const auto ENV_VAR_HOME = "HOME";
constexpr const auto ENV_VAR_DISPLAY = "DISPLAY";
constexpr const auto ENV_VAR_M8R_REPOSITORY = "MINDFORGER_REPOSITORY";
constexpr const auto ENV_VAR_M8R_EDITOR = "MINDFORGER_EDITOR";

constexpr const auto DIRNAME_M8R_REPOSITORY = "mindforger-repository";
// IMPROVE :-Z C++
constexpr const auto FILE_PATH_M8R_REPOSITORY = "~/mindforger-repository";

constexpr const auto FILENAME_M8R_CONFIGURATION = ".mindforger.md";
constexpr const auto FILENAME_OUTLINES_MAP = "outlines-map.md";
constexpr const auto DIRNAME_MEMORY = "memory";
constexpr const auto DIRNAME_MIND = "mind";
constexpr const auto DIRNAME_LIMBO = "limbo";
constexpr const auto DIRNAME_STENCILS = "stencils";
constexpr const auto DIRNAME_OUTLINES = "notebooks";
constexpr const auto DIRNAME_NOTES = "notes";

constexpr const auto UI_THEME_DARK = "dark";
constexpr const auto UI_THEME_LIGHT = "light";
constexpr const auto UI_THEME_LIGHT_WITH_FIXED_FONT = "light with fixed font";
constexpr const auto UI_THEME_BLACK = "black";
constexpr const auto UI_THEME_BLACK_WITH_FIXED_FONT = "black with fixed font";
constexpr const auto UI_THEME_NATIVE = "native";
constexpr const auto UI_THEME_NATIVE_WITH_FIXED_FONT = "native with fixed font";

constexpr const auto START_TO_OUTLINES_TREE = "outlines tree";
constexpr const auto START_TO_OUTLINES = "outlines";
constexpr const auto START_TO_TAGS = "tags";
constexpr const auto START_TO_RECENT = "recent";
constexpr const auto START_TO_EISENHOWER_MATRIX = "Eisenhower";
constexpr const auto START_TO_HOME_OUTLINE = "home";
constexpr const auto DEFAULT_STARTUP_VIEW = START_TO_OUTLINES;

constexpr const auto UI_HTML_THEME_CSS_LIGHT = "qrc:/html-css/light.css";
constexpr const auto UI_HTML_THEME_CSS_LIGHT_COMPACT = "qrc:/html-css/light-compact.css";
constexpr const auto UI_HTML_THEME_CSS_DARK = "qrc:/html-css/dark.css";
constexpr const auto UI_HTML_THEME_CSS_RAW = "raw";
constexpr const auto UI_HTML_THEME_CSS_CUSTOM = "custom";

constexpr const auto UI_EDITOR_KEY_BINDING_EMACS = "emacs";
constexpr const auto UI_EDITOR_KEY_BINDING_VIM = "vim";
constexpr const auto UI_EDITOR_KEY_BINDING_WIN = "windows";

constexpr const auto UI_JS_LIB_ONLINE = "online";
constexpr const auto UI_JS_LIB_OFFLINE = "offline";
constexpr const auto UI_JS_LIB_NO = "no";

constexpr const auto UI_OS_TABLE_SORT_ORDER_ASC = "ascending";
constexpr const auto UI_OS_TABLE_SORT_ORDER_DESC = "descending";

#ifdef __APPLE__
constexpr const auto UI_DEFAULT_THEME = UI_THEME_DARK;
#else
constexpr const auto UI_DEFAULT_THEME = UI_THEME_DARK;
#endif
constexpr const auto UI_DEFAULT_HTML_CSS_THEME = UI_HTML_THEME_CSS_LIGHT;
constexpr const auto UI_DEFAULT_EDITOR_FONT = "Monospace,10";
constexpr const auto UI_DEFAULT_FONT_POINT_SIZE = 10;

struct KnowledgeTool
{
    static const std::string TOOL_PHRASE;

    // knowledge tools IDs
    static constexpr const auto ARXIV = "arxiv";
    static constexpr const auto WIKIPEDIA = "wikipedia";
    static constexpr const auto GH_REPOS = "github-repositories";
    static constexpr const auto GH_CODE = "github-code";
    static constexpr const auto STACK_OVERFLOW = "stackoverflow";
    static constexpr const auto DUCKDUCKGO = "duckduckgo";
    static constexpr const auto GOOGLE = "google";
    static constexpr const auto CPP = "cpp";
    static constexpr const auto PYTHON = "python";

    std::string id;
    std::string name;
    std::string urlTemplate;

    KnowledgeTool(
        const std::string& id, const std::string& name, const std::string& urlTemplate)
        : id(id), name(name), urlTemplate(urlTemplate) {}

    static std::vector<std::string> getToolIds() {
        return {
            ARXIV,
            WIKIPEDIA,
            GH_REPOS,
            GH_CODE,
            STACK_OVERFLOW,
            DUCKDUCKGO,
            GOOGLE,
            CPP,
            PYTHON
        };
    }

    static std::string getUrlTemplateForToolId(const std::string& toolId) {
        if (toolId == ARXIV) {
            return "https://arxiv.org/search/?query=" + TOOL_PHRASE;
        } else if (toolId == WIKIPEDIA) {
            return "https://en.wikipedia.org/w/index.php?search=" + TOOL_PHRASE;
        } else if (toolId == GH_REPOS) {
            return "https://www.github.com/search?type=repositories&q=" + TOOL_PHRASE;
        } else if (toolId == GH_CODE) {
            return "https://www.github.com/search?type=code&q=" + TOOL_PHRASE;
        } else if (toolId == DUCKDUCKGO) {
            return "https://www.duckduckgo.com/?q=" + TOOL_PHRASE;
        } else if (toolId == GOOGLE) {
            return "https://www.google.com/search?q=" + TOOL_PHRASE;
        } else if (toolId == STACK_OVERFLOW) {
            return "https://stackoverflow.com/search?q=" + TOOL_PHRASE;
        } else if (toolId == CPP) {
            return "https://duckduckgo.com/?sites=cppreference.com&ia=web&q=" + TOOL_PHRASE;
        } else if (toolId == PYTHON) {
            return "https://docs.python.org/3.11/search.html?q=" + TOOL_PHRASE;
        }

        return "";
    }
};

// Wingman LLM models API keys
constexpr const auto ENV_VAR_OPENAI_API_KEY = "MINDFORGER_OPENAI_API_KEY";
constexpr const auto ENV_VAR_OPENAI_LLM_MODEL = "MINDFORGER_OPENAI_LLM_MODEL";

// improve platform/language specific
constexpr const auto DEFAULT_NEW_OUTLINE = "# New Markdown File\n\nThis is a new Markdown file created by MindForger.\n\n#Section 1\nThe first section.\n\n";

class Installer;
class RepositoryConfigurationPersistence;

/**
 * @brief MindForger configuration.
 *
 * Configuration file (Markdown-based DSL) maintained by this class contains
 * location of repositories, active repository and user preferences
 * (for GUIand library).
 *
 * MindForger configuration file is stored in ~/.mindforger.md by default.
 *
 * This class is singleton. The reason to make it singleton is that it is
 * used through lib and GUI instances. Therefore passing of the configuration
 * instance to (almost) each and every application's component would be
 * inefficient i.e. worse than the use of singleton pattern.
 */
/*
 * IMPROVE:
 *
 * - polish class description ^ - this is objec representation, MD config is MD representation
 * - pen & paper: where are defined constants
 * - code review: polish config item names (UI, editor, runtime) - shorter and more consistent
 *
 */
class Configuration {
private:
    static RepositoryConfiguration* getDummyRepositoryConfiguration() {
        static RepositoryConfiguration DUMMY_REPOSITORY_CONFIG{};
        return &DUMMY_REPOSITORY_CONFIG;
    }

public:
    static Configuration& getInstance() {
        static Configuration SINGLETON{};
        return SINGLETON;
    }

    enum MindState {
        THINKING, // 0
        DREAMING, // 1
        SLEEPING  // 2
    };

    enum AssociationAssessmentAlgorithm {
        BOW,
        WEIGHTED_FTS
    };

    enum JavaScriptLibSupport {
        NO,         // 0
        ONLINE,     // 2
        OFFLINE     // 1
    };

    enum EditorKeyBindingMode {
        EMACS,
        WINDOWS
    };

    enum SortOrganizerBy {
        URGENCY,
        IMPORTANCE,
    };

    enum FilterOrganizerBy {
        OUTLINES,
        NOTES,
        OUTLINES_NOTES
    };

    static constexpr const int DEFAULT_ASYNC_MIND_THRESHOLD_BOW = 200;
    static constexpr const int DEFAULT_ASYNC_MIND_THRESHOLD_WEIGHTED_FTS = 20000;
    static constexpr const int DEFAULT_DISTRIBUTOR_SLEEP_INTERVAL = 500;

    static const std::string DEFAULT_ACTIVE_REPOSITORY_PATH;
    static const std::string DEFAULT_TIME_SCOPE;
    static const std::string DEFAULT_WINGMAN_LLM_MODEL_OPENAI;

    static constexpr const bool DEFAULT_AUTOLINKING = false;
    static constexpr const bool DEFAULT_AUTOLINKING_COLON_SPLIT = true;
    static constexpr const bool DEFAULT_AUTOLINKING_CASE_INSENSITIVE = true;
    static constexpr const WingmanLlmProviders DEFAULT_WINGMAN_LLM_PROVIDER = WingmanLlmProviders::WINGMAN_PROVIDER_NONE;
    static constexpr const bool DEFAULT_SAVE_READS_METADATA = true;

    static constexpr const bool UI_DEFAULT_NERD_TARGET_AUDIENCE = true;
    static const std::string DEFAULT_STARTUP_VIEW_NAME;
    static const std::string DEFAULT_UI_THEME_NAME;
    static constexpr const bool DEFAULT_UI_SHOW_TOOLBAR = true;
    static constexpr const bool DEFAULT_UI_EXPERT_MODE = false;
    static constexpr const int DEFAULT_UI_APP_FONT_SIZE = 0;
    static constexpr const bool DEFAULT_UI_LIVE_NOTE_PREVIEW = true;
    static constexpr const bool DEFAULT_UI_NERD_MENU = false;
    static const std::string DEFAULT_UI_HTML_CSS_THEME;
    static constexpr const int DEFAULT_UI_HTML_ZOOM = 100;
    static const std::string DEFAULT_EDITOR_FONT;
    static constexpr const int DEFAULT_EDITOR_TAB_WIDTH = 4;
    static constexpr const int DEFAULT_NAVIGATOR_MAX_GRAPH_NODES = 150;
    static constexpr const bool DEFAULT_EDITOR_SYNTAX_HIGHLIGHT = true;
    static constexpr const bool DEFAULT_EDITOR_AUTOCOMPLETE = true;
    static constexpr const bool DEFAULT_EDITOR_SMART_EDITOR = true;
    static constexpr const bool DEFAULT_EDITOR_SPACE_SECTION_ESCAPING = true;
    static constexpr const bool DEFAULT_EDITOR_TABS_AS_SPACES = true;
    static constexpr const bool DEFAULT_EDITOR_AUTOSAVE = false;
    static constexpr const bool DEFAULT_FULL_O_PREVIEW = false;
    static constexpr const bool DEFAULT_MD_QUOTE_SECTIONS = true;
    static constexpr const bool DEFAULT_RECENT_INCLUDE_OS= false;
    static constexpr const bool DEFAULT_SPELLCHECK_LIVE = true;
    static constexpr const bool DEFAULT_MD_HIGHLIGHT = true;
    static constexpr const bool DEFAULT_MD_MATH = false;
    static constexpr const bool DEFAULT_ALLOW_ONLINE_JS_LIBS = false;
    static constexpr const bool DEFAULT_NAVIGATOR_SHOW_LEGEND = false;
    static constexpr const int DEFAULT_OS_TABLE_SORT_COLUMN = 7;
    static constexpr const bool DEFAULT_OS_TABLE_SORT_ORDER = false;
    static constexpr const bool DEFAULT_CLICK_NOTE_VIEW_TO_EDIT = true;

    static constexpr int EDITOR_MAX_AUTOCOMPLETE_LINES = 1000;

private:
    explicit Configuration();

private:
    // configured Mind state where user wants Mind to be
    MindState desiredMindState;
    // current Mind state on the way to desired state
    MindState mindState;
    // if count(N) > asyncMindTreshold then long-running mind computations should be run in async
    unsigned int asyncMindThreshold;

    std::string userHomePath;
    // some platforms, e.g. Windows, distinquishes user home and user documents
    std::string userDocPath;
    std::string configFilePath;

    Repository* activeRepository;
    std::map<const std::string, Repository*> repositories;

    // active repository memory, limbo, ... paths (efficiency)
    std::string memoryPath;
    std::string mindPath;
    std::string outlinesMapPath;
    std::string limboPath;

    // repository configuration (when in repository mode)
    RepositoryConfiguration* repositoryConfiguration;

    // lib configuration
    bool writeMetadata; // write metadata to MD - enabled in case of MINDFORGER_REPO only by default (can be disabled for all repository types)
    bool saveReadsMetadata; // persist count of Outline and Note reads (requires write to disc on every O/N view)
    bool autolinking; // enable MD autolinking
    bool autolinkingColonSplit;
    bool autolinkingCaseInsensitive;

    /*
    Wingman configuration, initialization and use:

    - CONFIGURATION:
        - configuration initialization:
            - configuration constructor():
                - configuration.llmProvider set to NONE
            - configuration load():
                - configuration.llmProvider is load first - is one of:
                    - NONE
                    - MOCK
                    - OPEN_AI
        - configuration detection whether particular Wingman provider is available:
            - bool can<provider>()
        - Wingman initialization from the configuration perspective
          (all fields, like API key, are set ...)
            - bool init<provider>()
        - Wingman CONFIGURATION AVAILABILITY to the runtime:
            - bool isWingman()
            - Wingman is available from the configuration perspective
    - MIND:
        - constructor:
          if Wingman configuration is available,
          then instantiate a Wingman @ configured provider
            - if configuration.isWingman()
              then mind.wingman = <provider>Wingman()
        - Wingman AVAILABILITY to the runtime:
            - Wingman* mind.getWingman()
                - nullptr || Wingman instance
        - configuration CHANGE detection:
            - mind.llmProvider used to detect configuration change
            - on change: switch Wingman instance
    - APP WINDOW / WINGMAN DIALOG:
        - configuration CHANGE detection:
            - appWindow.llmProvider used to detect configuration change
            - on change: re-init Wingman DIALOG (refresh pre-defined prompts)
    */
    WingmanLlmProviders wingmanProvider; // "none", "Mock", "OpenAI", ...
    std::string wingmanApiKey; // API key of the currently configured Wingman LLM provider
    std::string wingmanOpenAiApiKey; // OpenAI API specified by user in the config, env or UI
    std::string wingmanLlmModel; // preferred LLM model the currently configured provider, like "gpt-3.5-turbo"

    TimeScope timeScope;
    std::string timeScopeAsString;
    std::vector<std::string> tagsScope;
    unsigned int md2HtmlOptions;
    AssociationAssessmentAlgorithm aaAlgorithm;
    int distributorSleepInterval;

    bool markdownQuoteSections;
    /**
     * @brief Do include Outlines in the recent view or not.
     *
     * Subjectively it's better to show visited/editor Ns only as user
     * can always easily get to O description + automatically added
     * Os are increasing entropy of the recent view too much.
     */
    bool recentIncludeOs;

    // GUI configuration
    bool uiNerdTargetAudience;
    std::string startupView;
    std::string uiThemeName;
    std::string uiHtmlCssPath; // use a CSS (size>0) or render raw MD (size==0)
    int uiHtmlZoom;
    std::string externalEditorCmd;
    EditorKeyBindingMode uiEditorKeyBinding;
    std::string editorFont;
    int uiFontPointSize;
    bool uiShowBreadcrump; // show breadcrump path
    bool uiViewerShowMetadata; // show reads/writes/... when viewing Outlines and/or Notes.
    int uiEditorTabWidth;
    bool uiEditorLineNumbers; // show line numbers
    bool uiEditorSyntaxHighlighting; // toggle syntax highlighting
    bool uiEditorLiveSpellCheck;
    std::string uiEditorSpellCheckLanguage;
    // transient: available languages loaded in runtime from environment and not persisted
    std::vector<std::string> uiEditorSpellCheckLanguages;
    bool uiEditorAutocomplete; // toggle text autocomplete
    bool uiEditorSmartEditor; // toggle smart editor: lists, blocks and {[(`_
    bool uiEditorSpaceSectionEscaping; // escape # in section with spaces (enabled), or HTML (disabled)
    JavaScriptLibSupport uiEnableDiagramsInMd; // MD: diagrams
    int navigatorMaxNodes;
    bool uiEditorTabsAsSpaces;
    bool uiEditorAutosave;
    bool uiFullOPreview;
    bool uiShowToolbar;
    bool uiExpertMode;
    int uiAppFontSize;
    bool uiDistractionFreeMode; // fullscreen, no split, hidden toolbar + menu
    bool uiHoistedMode; // no split
    bool uiLiveNotePreview;
    int uiOsTableSortColumn;
    bool uiOsTableSortOrder; // true if ascending, else descending
    bool uiDoubleClickNoteViewToEdit;

    // organizers
    std::vector<Organizer*> organizers;

private:
    Installer* installer;

public:
    Configuration(const Configuration&) = delete;
    Configuration(const Configuration&&) = delete;
    Configuration &operator=(const Configuration&) = delete;
    Configuration &operator=(const Configuration&&) = delete;
    virtual ~Configuration();

    void clear();

    Installer* getInstaller() const { return installer; }
    MindState getMindState() const { return mindState; }
    void setMindState(MindState mindState) { this->mindState = mindState; }
    MindState getDesiredMindState() const { return desiredMindState; }
    void setDesiredMindState(MindState mindState) { this->desiredMindState = mindState; }
    unsigned int getAsyncMindThreshold() const { return asyncMindThreshold; }

    std::string& getConfigFilePath() { return configFilePath; }
    void setConfigFilePath(const std::string customConfigFilePath) {
        configFilePath = customConfigFilePath;
    }

    const std::string& getMemoryPath() const { return memoryPath; }
    const std::string& getMindPath() const { return mindPath; }
    const std::string& getOutlinesMapPath() const { return outlinesMapPath; }
    const std::string& getLimboPath() const { return limboPath; }
    const char* getRepositoryPathFromEnv();
    /**
     * @brief Create empty Markdown file.
     *
     * Create empty Markdown file if its name was given on command line - no dirs and a supported MD extension.
     */
    bool createEmptyMarkdownFile(const std::string& file);
    /**
     * @brief Find or create default MindForger repository.
     *
     * On MF start active repository location is determined as follows:
     *
     * 1) if application has single arg which is dir OR --repository, then use it, else 2)
     * 2) if configuration file exist w/ repository specified, then use it, else 3)
     * 3) if environment variable MINDFORGER_REPOSITORY is set, then use it, else 4)
     * 4) if repository exist in default location ~/mindforger-repository, then use it, else start W/O repository
     */
    void findOrCreateDefaultRepository(RepositoryConfigurationPersistence& persistence);
    Repository* addRepository(Repository* repository);
    std::map<const std::string, Repository*>& getRepositories();
    /**
     * @brief Set active repository
     *
     * Note that activeRepository parameter must be one of the known repositories.
     */
    void setActiveRepository(Repository* activeRepository, RepositoryConfigurationPersistence& persistence);
    bool isActiveRepository() const { return activeRepository!=nullptr; }
    Repository* getActiveRepository() const;

    /*
     * repository configuration
     */

    bool hasRepositoryConfiguration() const;
    RepositoryConfiguration& initRepositoryConfiguration(Organizer* defaultOrganizer);
    void clearRepositoryConfiguration();
    std::string getRepositoryConfigFilePath() const;
    /**
     * @brief Get repository configuration.
     *
     * This method always returns reference to repository configuration
     * reference - even if there is no repository i.g. in case of single
     * file editation or Markdown documents directory indexation. In such
     * case, dummy configuration instance is returned to ensure runtime
     * robustness.
     *
     * @return repository configuration reference.
     */
    RepositoryConfiguration& getRepositoryConfiguration() const {
        return *this->repositoryConfiguration;
    }

    /*
     * lib
     */

    const char* getEditorFromEnv();
    void setTimeScope(const TimeScope& timeScope) { this->timeScope = timeScope; }
    TimeScope& getTimeScope() { return timeScope; }
    void setTagsScope(const std::vector<std::string>& tags) { tagsScope.assign(tags.begin(), tags.end()); }
    void setTagsScope(const std::vector<const Tag*>& tags) { tagsScope.clear(); if(tags.size()) for(const Tag* t:tags) tagsScope.push_back(t->getName()); }
    std::vector<std::string>& getTagsScope() { return tagsScope; }
    bool isSaveReadsMetadata() const { return saveReadsMetadata; }
    void setSaveReadsMetadata(bool saveReadsMetadata) { this->saveReadsMetadata=saveReadsMetadata; }
    bool isAutolinking() const { return autolinking; }
    void setAutolinking(bool autolinking) { this->autolinking=autolinking; }
    bool isAutolinkingColonSplit() const { return autolinkingColonSplit; }
    void setAutolinkingColonSplit(bool autolinkingColonSplit) { this->autolinkingColonSplit=autolinkingColonSplit; }
    bool isAutolinkingCaseInsensitive() const { return autolinkingCaseInsensitive; }
    void setAutolinkingCaseInsensitive(bool autolinkingCaseInsensitive) { this->autolinkingCaseInsensitive=autolinkingCaseInsensitive; }
    unsigned int getMd2HtmlOptions() const { return md2HtmlOptions; }
    AssociationAssessmentAlgorithm getAaAlgorithm() const { return aaAlgorithm; }
    void setAaAlgorithm(AssociationAssessmentAlgorithm aaa) { aaAlgorithm = aaa; }
    int getDistributorSleepInterval() const { return distributorSleepInterval; }
    void setDistributorSleepInterval(int sleepInterval) { distributorSleepInterval = sleepInterval; }
    bool isMarkdownQuoteSections() const { return markdownQuoteSections; }
    void setMarkdownQuoteSections(bool markdownQuoteSections) { this->markdownQuoteSections = markdownQuoteSections; }
    bool isRecentIncludeOs() const { return recentIncludeOs; }
    void setRecentIncludeOs(bool recentIncludeOs) { this->recentIncludeOs = recentIncludeOs; }

    /*
     * Wingman
     */
    void setWingmanLlmProvider(WingmanLlmProviders provider);
    WingmanLlmProviders getWingmanLlmProvider() const { return wingmanProvider; }
    static std::string getWingmanLlmProviderAsString(WingmanLlmProviders provider) {
        if(provider == WingmanLlmProviders::WINGMAN_PROVIDER_MOCK) {
            return "mock";
        } else if(provider == WingmanLlmProviders::WINGMAN_PROVIDER_OPENAI) {
            return "openai";
        }

        return "none";
    }
#ifdef MF_WIP
    bool canWingmanMock() { return true; }
#else
    bool canWingmanMock() { return false; }
#endif
    bool canWingmanOpenAi();
private:
    bool initWingmanMock();
    bool initWingmanOpenAi();
    /**
     * @brief Initialize Wingman's LLM provider.
     */
    bool initWingman();
public:
    std::string getWingmanOpenAiApiKey() const { return wingmanOpenAiApiKey; }
    void setWingmanOpenAiApiKey(std::string apiKey) { wingmanOpenAiApiKey = apiKey; }
    /**
     * @brief Get API key of the currently configured Wingman LLM provider.
     */
    std::string getWingmanApiKey() const { return wingmanApiKey; }
    /**
     * @brief Get preferred Wingman LLM provider model name.
     */
    std::string getWingmanLlmModel() const { return wingmanLlmModel; }
    /**
     * @brief Check whether a Wingman LLM provider is ready from
     * the configuration perspective.
     */
    bool isWingman();

    /*
     * GUI
     */

    bool isExternalEditorCmd() const { return externalEditorCmd.size(); }
    std::string getExternalEditorCmd() const { return externalEditorCmd; }
    void setExternalEditorCmd(std::string externalEditorCmd) {
        this->externalEditorCmd = externalEditorCmd;
    }

    EditorKeyBindingMode getEditorKeyBinding() const { return uiEditorKeyBinding; }
    static const char* editorKeyBindingToString(EditorKeyBindingMode keyBinding) {
        if(keyBinding==EditorKeyBindingMode::EMACS) return UI_EDITOR_KEY_BINDING_EMACS; else
            if(keyBinding==EditorKeyBindingMode::WINDOWS) return UI_EDITOR_KEY_BINDING_WIN; else
                return UI_EDITOR_KEY_BINDING_VIM;
    }
    const char* getEditorKeyBindingAsString() const {
        return editorKeyBindingToString(uiEditorKeyBinding);
    }
    void setEditorKeyBinding(EditorKeyBindingMode keyBinding) { this->uiEditorKeyBinding=keyBinding; }
    void setEditorKeyBindingByString(const std::string& binding) {
        if(!binding.compare(UI_EDITOR_KEY_BINDING_EMACS)) {
            uiEditorKeyBinding=EditorKeyBindingMode::EMACS;
        } else {
            uiEditorKeyBinding=EditorKeyBindingMode::WINDOWS;
        }
    }
    void setEditorFont(std::string font) { this->editorFont = font; }
    std::string getEditorFont() { return this->editorFont; }
    int getUiFontPointSize() const { return uiFontPointSize; }
    bool isUiNerdTargetAudience() const { return uiNerdTargetAudience; }
    void setUiNerdTargetAudience(bool nerdAudience) { uiNerdTargetAudience = nerdAudience; }
    const std::string& getStartupView() const { return startupView; }
    void setStartupView(const std::string view) { startupView = view; }
    const std::string& getUiThemeName() const { return uiThemeName; }
    void setUiThemeName(const std::string theme) { uiThemeName = theme; }
    bool isUiEditorShowLineNumbers() const { return uiEditorLineNumbers; }
    void setUiEditorShowLineNumbers(bool show) { uiEditorLineNumbers = show; }
    bool isUiEditorEnableSyntaxHighlighting() const { return uiEditorSyntaxHighlighting; }
    void setUiEditorEnableSyntaxHighlighting(bool enable) { uiEditorSyntaxHighlighting = enable; }
    bool isUiEditorLiveSpellCheck() const { return uiEditorLiveSpellCheck; }
    void setUiEditorLiveSpellCheck(bool enable) { uiEditorLiveSpellCheck= enable; }
    std::string getUiEditorSpellCheckDefaultLanguage() const {
        return uiEditorSpellCheckLanguage;
    }
    void setUiEditorSpellCheckDefaultLanguage(std::string lang) {
        uiEditorSpellCheckLanguage = lang;
    }
    void clearUiEditorSpellCheckDefaultLanguage() {
        uiEditorSpellCheckLanguage.clear();
    }
    std::vector<std::string> getUiEditorSpellCheckLanguages() const {
        return uiEditorSpellCheckLanguages;
    }
    void setUiEditorSpellCheckLanguages(std::vector<std::string>& langs) {
        clearUiEditorSpellCheckLanguages();
        for(std::string lang: langs) {
            uiEditorSpellCheckLanguages.push_back(lang);
        }
    }
    void addUiEditorSpellCheckLanguage(std::string lang) {
        return uiEditorSpellCheckLanguages.push_back(lang);
    }
    void clearUiEditorSpellCheckLanguages() {
        uiEditorSpellCheckLanguages.clear();
        clearUiEditorSpellCheckDefaultLanguage();
        setUiEditorLiveSpellCheck(false);
    }
    bool isUiEditorEnableAutocomplete() const { return uiEditorAutocomplete; }
    void setUiEditorEnableAutocomplete(bool enable) { uiEditorAutocomplete = enable; }
    bool isUiEditorEnableSmartEditor() const { return uiEditorSmartEditor; }
    void setUiEditorEnableSmartEditor(bool enable) { uiEditorSmartEditor = enable; }
    bool isUiEditorSpaceSectionEscaping() const { return uiEditorSpaceSectionEscaping; }
    void setUiEditorSpaceSectionEscaping(bool enableSpaces) { uiEditorSpaceSectionEscaping = enableSpaces; }
    int getUiEditorTabWidth() const { return uiEditorTabWidth; }
    void setUiEditorTabWidth(int tabWidth) { uiEditorTabWidth = tabWidth; }
    int getRecentNotesUiLimit() const { return 150; }
    bool isUiShowBreadcrump() const { return uiShowBreadcrump; }
    bool isUiHtmlTheme() const { return !uiHtmlCssPath.empty(); }
    const char* getUiHtmlCssPath() const {
        return uiHtmlCssPath.size()?uiHtmlCssPath.c_str():UI_HTML_THEME_CSS_RAW;
    }
    void setUiHtmlCssPath(const std::string path) {
        if(!path.compare(UI_HTML_THEME_CSS_RAW)) {
            uiHtmlCssPath.clear();
        } else {
            uiHtmlCssPath = path;
        }
    }
    int getUiHtmlZoom() const { return uiHtmlZoom; }
    void incUiHtmlZoom() { if(uiHtmlZoom<500) uiHtmlZoom += 10; }
    void decUiHtmlZoom() { if(uiHtmlZoom>40) uiHtmlZoom-= 10; }
    float getUiHtmlZoomFactor() const {
        return static_cast<float>(uiHtmlZoom)/100.f;
    }
    /**
     * @brief Set HTML zoom of Markdown viewer.
     * @param zoom  value between 25% and 500%
     */
    void setUiHtmlZoom(int zoom) { uiHtmlZoom = zoom; }

    const char* getJsLibSupportAsString(JavaScriptLibSupport s) const {
        if(s==JavaScriptLibSupport::ONLINE) return UI_JS_LIB_ONLINE; else
            if(s==JavaScriptLibSupport::OFFLINE) return UI_JS_LIB_OFFLINE; else return UI_JS_LIB_NO;
    }

    bool isUiEnableSrcHighlightInMd() {
        return (md2HtmlOptions&MdToHtmlOption::CodeHighlighting)>0?true:false;
    }
    void setUiEnableSrcHighlightInMd(bool enable) {
        if(enable) {
            md2HtmlOptions |= MdToHtmlOption::CodeHighlighting;
        } else {
            md2HtmlOptions &= ~MdToHtmlOption::CodeHighlighting;
        }
    }
    bool isUiEnableMathInMd() {
        return (md2HtmlOptions&MdToHtmlOption::MathSupport)>0?true:false;
    }
    void setUiEnableMathInMd(bool enable) {
        if(enable) {
            md2HtmlOptions |= MdToHtmlOption::MathSupport;
        } else {
            md2HtmlOptions &= ~MdToHtmlOption::MathSupport;
        }
    }

    JavaScriptLibSupport getUiEnableDiagramsInMd() { return uiEnableDiagramsInMd; }
    void setUiEnableDiagramsInMd(JavaScriptLibSupport mode) { uiEnableDiagramsInMd = mode; }

    int getNavigatorMaxNodes() const { return navigatorMaxNodes; }
    void setNavigatorMaxNodes(int navigatorMaxNodes) { this->navigatorMaxNodes = navigatorMaxNodes; }
    bool isUiEditorTabsAsSpaces() const { return uiEditorTabsAsSpaces; }
    void setUiEditorTabsAsSpaces(bool uiEditorTabsAsSpaces){ this->uiEditorTabsAsSpaces = uiEditorTabsAsSpaces; }
    bool isUiEditorAutosave() const { return uiEditorAutosave; }
    void setUiEditorAutosave(bool uiEditorAutosave){ this->uiEditorAutosave = uiEditorAutosave; }
    bool isUiFullOPreview() const { return uiFullOPreview; }
    void setUiFullOPreview(bool fullPreview){ this->uiFullOPreview = fullPreview; }
    bool isUiShowToolbar() const { return uiShowToolbar; }
    void setUiShowToolbar(bool showToolbar){ this->uiShowToolbar = showToolbar; }
    bool isUiExpertMode() const { return uiExpertMode; }
    void setUiExpertMode(bool uiExpertMode){ this->uiExpertMode = uiExpertMode; }
    int getUiAppFontSize() const { return uiAppFontSize; }
    void setUiAppFontSize(int appFontSize){
        MF_DEBUG("Configuration::setUiAppFontSize: " << appFontSize << std::endl);
        this->uiAppFontSize = appFontSize;
    }
    bool isUiDistractionFreeMode() const { return uiDistractionFreeMode; }
    void setUiDistractionFreeMode(bool distractionFreeMode){ this->uiDistractionFreeMode = distractionFreeMode; }
    bool isUiHoistedMode() const { return uiHoistedMode; }
    void setUiHoistedMode(bool hoisted){ this->uiHoistedMode = hoisted; }
    bool isUiLiveNotePreview() const { return uiLiveNotePreview; }
    void setUiLiveNotePreview(bool isPreview){ this->uiLiveNotePreview = isPreview; }
    int getUiOsTableSortColumn() const { return uiOsTableSortColumn; }
    void setUiOsTableSortColumn(const int column) { this->uiOsTableSortColumn = column; }
    bool isUiOsTableSortOrder() const { return uiOsTableSortOrder; }
    void setUiOsTableSortOrder(const bool ascending) { this->uiOsTableSortOrder = ascending; }
    bool isUiDoubleClickNoteViewToEdit() const { return this->uiDoubleClickNoteViewToEdit; }
    void setUiDoubleClickNoteViewToEdit(bool enable) { this->uiDoubleClickNoteViewToEdit = enable; }
};

} // namespace

#endif /* M8R_CONFIGURATION_H_ */
