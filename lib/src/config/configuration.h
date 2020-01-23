/*
 configuration.h     M8r configuration management

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_CONFIGURATION_H_
#define M8R_CONFIGURATION_H_

#include <set>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <vector>

#include "repository.h"
#include "time_scope.h"
#include "../repository_indexer.h"
#include "../gear/lang_utils.h"
#include "../gear/file_utils.h"
#include "../exceptions.h"
#include "../model/tag.h"
#include "../install/installer.h"
#include "../representations/markdown/markdown_transcoder.h"

namespace m8r {

// const in constexpr makes value const
constexpr const auto ENV_VAR_HOME = "HOME";
constexpr const auto ENV_VAR_DISPLAY = "DISPLAY";
constexpr const auto ENV_VAR_M8R_REPOSITORY = "MINDFORGER_REPOSITORY";
constexpr const auto ENV_VAR_M8R_EDITOR = "MINDFORGER_EDITOR";

constexpr const auto DIRNAME_M8R_REPOSITORY = "mindforger-repository";
// IMPROVE :-Z C++
constexpr const auto FILE_PATH_M8R_REPOSITORY = "~/mindforger-repository";

constexpr const auto FILENAME_M8R_CONFIGURATION = ".mindforger.md";
constexpr const auto FILE_PATH_MEMORY = "memory";
constexpr const auto FILE_PATH_MIND = "mind";
constexpr const auto FILE_PATH_LIMBO = "limbo";
constexpr const auto FILE_PATH_STENCILS = "stencils";
constexpr const auto FILE_PATH_OUTLINES = "notebooks";
constexpr const auto FILE_PATH_NOTES = "notes";

constexpr const auto FILE_EXTENSION_HTML = ".html";
constexpr const auto FILE_EXTENSION_CSV= ".csv";

constexpr const auto FILE_EXTENSION_MD_MD = ".md";
constexpr const auto FILE_EXTENSION_MD_MARKDOWN = ".markdown";
constexpr const auto FILE_EXTENSION_MD_MDOWN = ".mdown";
constexpr const auto FILE_EXTENSION_MD_MKDN = ".mkdn";

constexpr const auto UI_THEME_DARK = "dark";
constexpr const auto UI_THEME_LIGHT = "light";
constexpr const auto UI_THEME_BLACK = "black";
constexpr const auto UI_THEME_NATIVE = "native";

constexpr const auto START_TO_DASHBOARD = "dashboard";
constexpr const auto START_TO_OUTLINES = "outlines";
constexpr const auto START_TO_TAGS = "tags";
constexpr const auto START_TO_RECENT = "recent";
constexpr const auto START_TO_EISENHOWER_MATRIX = "Eisehower";
constexpr const auto START_TO_HOME_OUTLINE = "home";
constexpr const auto DEFAULT_STARTUP_VIEW = START_TO_OUTLINES;

constexpr const auto UI_HTML_THEME_CSS_LIGHT = "qrc:/html-css/light.css";
constexpr const auto UI_HTML_THEME_CSS_LIGHT_COMPACT = "qrc:/html-css/light-compact.css";
constexpr const auto UI_HTML_THEME_CSS_DARK = "qrc:/html-css/dark.css";
constexpr const auto UI_HTML_THEME_CSS_RAW = "raw";

constexpr const auto UI_EDITOR_KEY_BINDING_EMACS = "emacs";
constexpr const auto UI_EDITOR_KEY_BINDING_VIM = "vim";
constexpr const auto UI_EDITOR_KEY_BINDING_WIN = "windows";

constexpr const auto UI_JS_LIB_ONLINE = "online";
constexpr const auto UI_JS_LIB_OFFLINE = "offline";
constexpr const auto UI_JS_LIB_NO = "no";

constexpr const auto UI_DEFAULT_THEME = UI_THEME_LIGHT;
constexpr const auto UI_DEFAULT_HTML_CSS_THEME = UI_HTML_THEME_CSS_LIGHT;
constexpr const auto UI_DEFAULT_EDITOR_FONT = "Monospace,10";
constexpr const auto UI_DEFAULT_FONT_POINT_SIZE = 10;

// improve platform/language specific
constexpr const auto DEFAULT_NEW_OUTLINE = "# New Markdown File\n\nThis is a new Markdown file created by MindForger.\n\n#Section 1\nThe first section.\n\n";

class Installer;

/**
 * @brief MindForger configuration.
 *
 * Configuration file (Markdown-based DSL) maintained by this class contains
 * location of repositories, active repository and user preferences (for GUI
 * and library).
 *
 * MindForger configuration file is stored in ~/.mindforger.md by default.
 *
 * This class is singleton. The reason to make it singleton is that it's used
 * through lib and GUI instances. Therefore passing of the configuration instance
 * to (almost) each and every application's component would be inefficient i.e. worse
 * than the use of singleton pattern.
 */
class Configuration {
public:
    static Configuration& getInstance()
    {
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
        OFFLINE,    // 1
        ONLINE      // 2
    };

    enum EditorKeyBindingMode {
        EMACS,
        VIM,
        WINDOWS
    };

    static constexpr int DEFAULT_ASYNC_MIND_THRESHOLD_BOW = 200;
    static constexpr int DEFAULT_ASYNC_MIND_THRESHOLD_WEIGHTED_FTS = 10000;
    static constexpr int DEFAULT_DISTRIBUTOR_SLEEP_INTERVAL = 500;

    static const std::string DEFAULT_ACTIVE_REPOSITORY_PATH;
    static const std::string DEFAULT_TIME_SCOPE;

    static constexpr const bool DEFAULT_AUTOLINKING = false;
    static constexpr const bool DEFAULT_AUTOLINKING_COLON_SPLIT = true;
    static constexpr const bool DEFAULT_AUTOLINKING_CASE_INSENSITIVE = true;
    static constexpr const bool DEFAULT_SAVE_READS_METADATA = true;

    static constexpr const bool UI_DEFAULT_NERD_TARGET_AUDIENCE = true;
    static const std::string DEFAULT_STARTUP_VIEW_NAME;
    static const std::string DEFAULT_UI_THEME_NAME;
    static constexpr const bool DEFAULT_UI_SHOW_TOOLBAR = true;
    static constexpr const bool DEFAULT_UI_EXPERT_MODE = false;
    static constexpr const bool DEFAULT_UI_NERD_MENU = false;
    static const std::string DEFAULT_UI_HTML_CSS_THEME;
    static const int DEFAULT_UI_HTML_ZOOM = 100;
    static const std::string DEFAULT_EDITOR_FONT;
    static constexpr int DEFAULT_EDITOR_TAB_WIDTH = 4;
    static constexpr int DEFAULT_NAVIGATOR_MAX_GRAPH_NODES = 150;
    static constexpr const bool DEFAULT_EDITOR_SYNTAX_HIGHLIGHT = true;
    static constexpr const bool DEFAULT_EDITOR_AUTOCOMPLETE = true;
    static constexpr const bool DEFAULT_EDITOR_TABS_AS_SPACES = true;
    static constexpr const bool DEFAULT_EDITOR_AUTOSAVE = false;
    static constexpr const bool DEFAULT_MD_QUOTE_SECTIONS = true;
    static constexpr const bool DEFAULT_MD_HIGHLIGHT = true;
    static constexpr const bool DEFAULT_MD_MATH = false;
    static constexpr const bool DEFAULT_ALLOW_ONLINE_JS_LIBS = false;

    static constexpr const bool DEFAULT_NAVIGATOR_SHOW_LEGEND = false;

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
    // Some platforms, e.g. Windows, distinquishes user home and user documents
    std::string userDocPath;
    std::string configFilePath;

    Repository* activeRepository;
    std::map<const std::string, Repository*> repositories;

    // active repository memory, limbo, ... paths (efficiency)
    std::string memoryPath;
    std::string limboPath;

    // lib configuration
    bool writeMetadata; // write metadata to MD - enabled in case of MINDFORGER_REPO only by default (can be disabled for all repository types)
    bool saveReadsMetadata; // persist count of Outline and Note reads (requires write to disc on every O/N view)
    bool autolinking; // enable MD autolinking
    bool autolinkingColonSplit;
    bool autolinkingCaseInsensitive;
    TimeScope timeScope;
    std::string timeScopeAsString;
    std::vector<std::string> tagsScope;
    unsigned int md2HtmlOptions;
    AssociationAssessmentAlgorithm aaAlgorithm;
    int distributorSleepInterval;
    bool markdownQuoteSections;

    // GUI configuration
    bool uiNerdTargetAudience;
    std::string startupView;
    std::string uiThemeName;
    std::string uiHtmlCssPath; // use a CSS (size>0) or render raw MD (size==0)
    int uiHtmlZoom;
    EditorKeyBindingMode uiEditorKeyBinding;
    std::string editorFont;
    int uiFontPointSize;
    bool uiShowBreadcrump; // show breadcrump path
    bool uiViewerShowMetadata; // show reads/writes/... when viewing Outlines and/or Notes.
    int uiEditorTabWidth;
    bool uiEditorLineNumbers; // show line numbers
    bool uiEditorSyntaxHighlighting; // toggle syntax highlighting
    bool uiEditorAutocomplete; // toggle autocompletion
    JavaScriptLibSupport uiEnableDiagramsInMd; // MD: diagrams
    int navigatorMaxNodes;
    bool uiEditorTabsAsSpaces;
    bool uiEditorAutosave;
    bool uiShowToolbar;
    bool uiExpertMode;
    bool uiDistractionFreeMode; // fullscreen, no split, hidden toolbar + menu
    bool uiHoistedMode; // no split
    std::string uiOutlinesTableSortColumn;
    bool uiOutlinesTableSortAsc;

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
    void setConfigFilePath(const std::string customConfigFilePath) { configFilePath = customConfigFilePath; }
    const std::string& getMemoryPath() const { return memoryPath; }
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
    void findOrCreateDefaultRepository();
    Repository* addRepository(Repository* repository);
    std::map<const std::string, Repository*>& getRepositories();
    /**
     * @brief Set active repository
     *
     * Note that activeRepository parameter must be one of the known repositories.
     */
    void setActiveRepository(Repository* activeRepository);
    bool isActiveRepository() const { return activeRepository!=nullptr; }
    Repository* getActiveRepository() const;

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

    /*
     * GUI
     */

    EditorKeyBindingMode getEditorKeyBinding() const { return uiEditorKeyBinding; }
    static const char* editorKeyBindingToString(EditorKeyBindingMode keyBinding) {
        if(keyBinding==EditorKeyBindingMode::EMACS) return UI_EDITOR_KEY_BINDING_EMACS; else
            if(keyBinding==EditorKeyBindingMode::WINDOWS) return UI_EDITOR_KEY_BINDING_WIN; else return UI_EDITOR_KEY_BINDING_VIM;
    }
    const char* getEditorKeyBindingAsString() const {
        return editorKeyBindingToString(uiEditorKeyBinding);
    }
    void setEditorKeyBinding(EditorKeyBindingMode keyBinding) { this->uiEditorKeyBinding=keyBinding; }
    void setEditorKeyBindingByString(const std::string& binding) {
        if(!binding.compare(UI_EDITOR_KEY_BINDING_WIN)) uiEditorKeyBinding=EditorKeyBindingMode::WINDOWS;
        else if (!binding.compare(UI_EDITOR_KEY_BINDING_VIM)) uiEditorKeyBinding=EditorKeyBindingMode::VIM;
        else uiEditorKeyBinding=EditorKeyBindingMode::EMACS;
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
    bool isUiEditorEnableAutocomplete() const { return uiEditorAutocomplete; }
    void setUiEditorEnableAutocomplete(bool enable) { uiEditorAutocomplete = enable; }
    int getUiEditorTabWidth() const { return uiEditorTabWidth; }
    void setUiEditorTabWidth(int tabWidth) { uiEditorTabWidth = tabWidth; }
    int getRecentNotesUiLimit() const { return 150; }
    bool isUiShowBreadcrump() const { return uiShowBreadcrump; }
    bool isUiHtmlTheme() const { return !uiHtmlCssPath.empty(); }
    const char* getUiHtmlCssPath() const {
        return uiHtmlCssPath.size()?uiHtmlCssPath.c_str():UI_HTML_THEME_CSS_RAW;
    }
    void setUiHtmlCssPath(const std::string path) {
        if(!path.compare(UI_HTML_THEME_CSS_RAW)) uiHtmlCssPath.clear(); else uiHtmlCssPath = path;
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

    bool isUiShowToolbar() const { return uiShowToolbar; }
    void setUiShowToolbar(bool showToolbar){ this->uiShowToolbar = showToolbar; }
    bool isUiExpertMode() const { return uiExpertMode; }
    void setUiExpertMode(bool uiExpertMode){ this->uiExpertMode= uiExpertMode; }
    bool isUiDistractionFreeMode() const { return uiDistractionFreeMode; }
    void setUiDistractionFreeMode(bool distractionFreeMode){ this->uiDistractionFreeMode = distractionFreeMode; }
    bool isUiHoistedMode() const { return uiHoistedMode; }
    void setUiHoistedMode(bool hoisted){ this->uiHoistedMode = hoisted; }
};

} // namespace

#endif /* M8R_CONFIGURATION_H_ */
