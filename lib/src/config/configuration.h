/*
 configuration.h     M8r configuration management

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

namespace m8r {

// const in constexpr makes value const
constexpr const auto ENV_VAR_HOME = "HOME";
constexpr const auto ENV_VAR_TERM = "TERM";
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
constexpr const auto FILE_PATH_OUTLINES = "outlines";
constexpr const auto FILE_PATH_NOTES = "notes";

constexpr const auto FILE_EXTENSION_MD_MD = ".md";
constexpr const auto FILE_EXTENSION_MD_MARKDOWN = ".markdown";
constexpr const auto FILE_EXTENSION_MD_MDOWN = ".mdown";
constexpr const auto FILE_EXTENSION_MD_MKDN = ".mkdn";

constexpr const auto UI_THEME_DARK = "dark";
constexpr const auto UI_THEME_LIGHT = "light";
constexpr const auto UI_THEME_BLACK = "black";

constexpr const auto UI_HTML_THEME_CSS_LIGHT = "qrc:/html-css/light.css";
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
constexpr const auto UI_DEFAULT_EDITOR_KEY_BINDING = UI_EDITOR_KEY_BINDING_EMACS;
constexpr const auto UI_DEFAULT_FONT_POINT_SIZE = 10;

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

    enum MdToHtmlOption {
        // Discount options: https://www.pell.portland.or.us/~orc/Code/discount/
        NoLinksOption          = 1<<0, /* don't do link processing, block <a> tags  */
        NoImagesOption         = 1<<1, /* don't do image processing, block <img> */
        NoSmartypantsOption    = 1<<2, /* don't run smartypants() */
        NoHtmlOption           = 1<<3, /* don't allow raw html through AT ALL */
        NoSuperscriptOption    = 1<<4, /* don't process a^2 as superscript(<sup>) */
        NoTablesOption         = 1<<5, /* disallow tables */
        NoStrikethroughOption  = 1<<6, /* forbid ~~strikethrough~~ */
        TableOfContentsOption  = 1<<7, /* do table-of-contents processing */
        AutolinkOption         = 1<<8, /* make http://foo.com link even without <>s */
        NoHeaderOption         = 1<<9, /* don't process header blocks */
        NoDivQuoteOption       = 1<<10, /* forbid >%class% blocks */
        NoAlphaListOption      = 1<<11, /* forbid alphabetic lists */
        NoDefinitionListOption = 1<<12, /* forbid definition lists */
        ExtraFootnoteOption    = 1<<13, /* enable markdown extra-style footnotes */
        NoStyleOption          = 1<<14, /* don't extract <style> blocks */

        // postprocessing - makes HTML rendering slow
        MathSupport            = 1<<15,
        CodeHighlighting       = 1<<16,
        DiagramSupport         = 1<<17
    };

    enum EditorKeyBindingMode {
        EMACS,
        VIM,
        WINDOWS
    };

    static constexpr int DEFAULT_ASYNC_MIND_THRESHOLD_BOW = 200;
    static constexpr int DEFAULT_ASYNC_MIND_THRESHOLD_WEIGHTED_FTS = 10000;
    static constexpr int DEFAULT_DISTRIBUTOR_SLEEP_INTERVAL = 3000;

    static const std::string DEFAULT_ACTIVE_REPOSITORY_PATH;
    static const std::string DEFAULT_TIME_SCOPE;
    static constexpr const bool DEFAULT_SAVE_READS_METADATA = true;

    static const std::string DEFAULT_UI_THEME_NAME;
    static const std::string DEFAULT_UI_HTML_CSS_THEME;
    static const std::string DEFAULT_EDITOR_KEY_BINDING;
    static constexpr int DEFAULT_EDITOR_TAB_WIDTH = 4;
    static constexpr const bool DEFAULT_EDITOR_SYNTAX_HIGHLIGHT = true;
    static constexpr const bool DEFAULT_EDITOR_AUTOCOMPLETE = true;
    static constexpr const bool DEFAULT_MD_HIGHLIGHT = true;
    static constexpr const bool DEFAULT_MD_MATH = false;
    static constexpr const bool DEFAULT_ALLOW_ONLINE_JS_LIBS = false;

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
    std::string configFilePath;

    Repository* activeRepository;
    std::map<const std::string, Repository*> repositories;

    // active repository memory, limbo, ... paths (efficiency)
    std::string memoryPath;
    std::string limboPath;

    // lib configuration
    bool writeMetadata; // write metadata to MD - enabled in case of MINDFORGER_REPO only by default (can be disabled for all repository types)
    bool saveReadsMetadata; // persist count of Outline and Note reads (requires write to disc on every O/N view)
    TimeScope timeScope;
    std::string timeScopeAsString;
    std::vector<std::string> tagsScope;
    unsigned int md2HtmlOptions;
    AssociationAssessmentAlgorithm aaAlgorithm;
    int distributorSleepInterval;

    // GUI configuration
    std::string uiThemeName;
    std::string uiHtmlCssPath; // use a CSS (size>0) or render raw MD (size==0)    
    EditorKeyBindingMode uiEditorKeyBinding;
    int uiFontPointSize;
    bool uiShowBreadcrump; // show breadcrump path
    bool uiViewerShowMetadata; // show reads/writes/... when viewing Outlines and/or Notes.
    int uiEditorTabWidth;
    bool uiEditorShowLineNumbers; // show line numbers
    bool uiEditorEnableSyntaxHighlighting; // toggle syntax highlighting    
    bool uiEditorAutocomplete; // toggle autocompletion
    JavaScriptLibSupport uiEnableDiagramsInMd; // MD: diagrams

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
    unsigned int getMd2HtmlOptions() const { return md2HtmlOptions; }
    AssociationAssessmentAlgorithm getAaAlgorithm() const { return aaAlgorithm; }
    void setAaAlgorithm(AssociationAssessmentAlgorithm aaa) { aaAlgorithm = aaa; }
    int getDistributorSleepInterval() const { return distributorSleepInterval; }
    void setDistributorSleepInterval(int sleepInterval) { distributorSleepInterval = sleepInterval; }

    /*
     * GUI
     */

    EditorKeyBindingMode getEditorKeyBinding() const { return uiEditorKeyBinding; }
    const char* getEditorKeyBindingAsString() const {
        if(uiEditorKeyBinding==EditorKeyBindingMode::EMACS) return UI_EDITOR_KEY_BINDING_EMACS; else
            if(uiEditorKeyBinding==EditorKeyBindingMode::WINDOWS) return UI_EDITOR_KEY_BINDING_WIN; else return UI_EDITOR_KEY_BINDING_VIM;
    }
    void setEditorKeyBinding(EditorKeyBindingMode keyBinding) { this->uiEditorKeyBinding=keyBinding; }
    void setEditorKeyBindingByString(const std::string& binding) {
        if(!binding.compare(UI_EDITOR_KEY_BINDING_WIN)) uiEditorKeyBinding=EditorKeyBindingMode::WINDOWS;
        else if (!binding.compare(UI_EDITOR_KEY_BINDING_VIM)) uiEditorKeyBinding=EditorKeyBindingMode::VIM;
        else uiEditorKeyBinding=EditorKeyBindingMode::EMACS;
    }
    int getUiFontPointSize() const { return uiFontPointSize; }
    const std::string& getUiThemeName() const { return uiThemeName; }
    void setUiThemeName(const std::string theme) { uiThemeName = theme; }
    bool isUiEditorShowLineNumbers() const { return uiEditorShowLineNumbers; }
    void setUiEditorShowLineNumbers(bool show) { uiEditorShowLineNumbers = show; }
    bool isUiEditorEnableSyntaxHighlighting() const { return uiEditorEnableSyntaxHighlighting; }
    void setUiEditorEnableSyntaxHighlighting(bool enable) { uiEditorEnableSyntaxHighlighting = enable; }
    bool isUiEditorEnableAutocomplete() const { return uiEditorEnableAutocomplete; }
    void setUiEditorEnableAutocomplete(bool enable) { uiEditorEnableAutocomplete = enable; }
    int getUiEditorTabWidth() const { return uiEditorTabWidth; }
    void setUiEditorTabWidth(int tabWidth) { uiEditorTabWidth = tabWidth; }
    bool isUiShowBreadcrump() const { return uiShowBreadcrump; }
    bool isUiHtmlTheme() const { return !uiHtmlCssPath.empty(); }
    const char* getUiHtmlCssPath() const {
        return uiHtmlCssPath.size()?uiHtmlCssPath.c_str():UI_HTML_THEME_CSS_RAW;
    }
    void setUiHtmlCssPath(const std::string path) {
        if(!path.compare(UI_HTML_THEME_CSS_RAW)) uiHtmlCssPath.clear(); else uiHtmlCssPath = path;
    }

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
};

} // namespace

#endif /* M8R_CONFIGURATION_H_ */
