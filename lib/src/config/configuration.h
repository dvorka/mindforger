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
constexpr const auto FILE_EXTENSION_MARKDOWN = ".md";

constexpr const auto UI_THEME_DARK = "dark";
constexpr const auto UI_THEME_LIGHT = "light";
constexpr const auto UI_THEME_BLACK = "black";
constexpr const auto UI_DEFAULT_THEME = UI_THEME_LIGHT;
constexpr const auto UI_DEFAULT_FONT_POINT_SIZE = 10;

class Installer;

struct TimeScope {
    u_int8_t years;
    u_int8_t months;
    u_int8_t days;
    u_int8_t hours;
    u_int8_t minutes;

    explicit TimeScope() {
        years=months=days=hours=minutes=0;
    }

    explicit TimeScope(u_int8_t years, u_int8_t months, u_int8_t days, u_int8_t hours, u_int8_t minutes) {
        this->years = years;
        this->months = months;
        this->days = days;
        this->hours = hours;
        this->minutes = minutes;
    }
};

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
        THINKING,
        DREAMING,
        SLEEPING
    };

    static const std::string DEFAULT_ACTIVE_REPOSITORY_PATH;
    static const std::string DEFAULT_TIME_SCOPE;
    static constexpr const bool DEFAULT_SHOW_NOTEBOOK_EDIT_BUTTON = true;
    static constexpr const bool DEFAULT_SAVE_READS_METADATA = true;

    static const std::string DEFAULT_UI_THEME_NAME;

private:
    explicit Configuration();

private:
    MindState mindState;

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
    std::string externalEditorPath; // path to external MD editor e.g. Emacs or Remarkable
    TimeScope timeScope;
    std::string timeScopeAsString;

    // GUI configuration
    std::string uiThemeName;
    int uiFontPointSize;
    bool uiShowBreadcrump; // show breadcrump path
    bool uiViewerShowMetadata; // show reads/writes/... when viewing Outlines and/or Notes.
    bool uiEditorShowLineNumbers; // show line numbers
    bool uiEditorEnableSyntaxHighlighting; // toggle syntax highlighting
    bool uiShowNotebookEditButton;

private:
    Installer* installer;

public:
    Configuration(const Configuration&) = delete;
    Configuration(const Configuration&&) = delete;
    Configuration &operator=(const Configuration&) = delete;
    Configuration &operator=(const Configuration&&) = delete;
    virtual ~Configuration();

    void reset();

    Installer* getInstaller() const { return installer; }
    MindState getMindState() const { return mindState; }
    void setMindState(MindState mindState);

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
    const std::string& getExternalEditorPath() const { return externalEditorPath; }
    void setTimeScope(const TimeScope& timeScope) { this->timeScope = timeScope; }
    const TimeScope& getTimeScope() const { return timeScope; }
    bool isSaveReadsMetadata() const { return saveReadsMetadata; }
    void setSaveReadsMetadata(bool saveReadsMetadata) { this->saveReadsMetadata=saveReadsMetadata; }

    /*
     * GUI
     */

    int getUiFontPointSize() const { return uiFontPointSize; }
    const std::string& getUiThemeName() const { return uiThemeName; }
    void setUiThemeName(const std::string theme) { uiThemeName = theme; }
    bool isUiEditorShowLineNumbers() const { return uiEditorShowLineNumbers; }
    void setUiEditorShowLineNumbers(bool show) { uiEditorShowLineNumbers = show; }
    bool isUiEditorEnableSyntaxHighlighting() const { return uiEditorEnableSyntaxHighlighting; }
    void setUiEditorEnableSyntaxHighlighting(bool enable) { uiEditorEnableSyntaxHighlighting = enable; }
    bool isUiShowNotebookEditButton() const { return uiShowNotebookEditButton; }
    void setUiShowNotebookEditButton(bool show) { uiShowNotebookEditButton = show; }
};

} // namespace

#endif /* M8R_CONFIGURATION_H_ */
