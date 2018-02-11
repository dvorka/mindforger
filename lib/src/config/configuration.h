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
#include "../gear/lang_utils.h"
#include "../gear/file_utils.h"
#include "../exceptions.h"
#include "../model/tag.h"
#include "../representations/markdown/markdown.h"
#include "../representations/markdown/markdown_ast_node.h"
#include "../install/installer.h"

namespace m8r {

constexpr const auto ENV_VAR_HOME = "HOME";
constexpr const auto ENV_VAR_M8R_REPOSITORY = "MINDFORGER_REPOSITORY";
constexpr const auto ENV_VAR_M8R_EDITOR = "MINDFORGER_EDITOR";

constexpr const auto DIRNAME_M8R_REPOSITORY = "mindforger-repository";

constexpr const auto FILENAME_M8R_CONFIGURATION = ".mindforger.md";
constexpr const auto FILE_PATH_MEMORY = "memory";
constexpr const auto FILE_PATH_MIND = "mind";
constexpr const auto FILE_PATH_LIMBO = "limbo";
constexpr const auto FILE_PATH_STENCILS = "stencils";
constexpr const auto FILE_PATH_OUTLINES = "outlines";
constexpr const auto FILE_PATH_NOTES = "notes";
constexpr const auto FILE_EXTENSION_MARKDOWN = ".md";

constexpr const auto DEFAULT_FONT_POINT_SIZE = 10;

class MarkdownAstNodeSection;
class Installer;

/**
 * @brief MindForger configuration.
 *
 * Configuration file (in Markdown format) contains location
 * of repositories, active repository and user preferences (for GUI and library).
 *
 * MindForger configuration file may be stored in ~/.mindforger.md
 */
class Configuration {
public:

private:
    Repository* activeRepository;

    std::string memoryPath;
    std::string limboPath;

    std::set<const Repository*> repositories;

    // lib configuration
    std::string userHomePath;
    bool writeMetadata; // write metadata to MD - enabled in case of MINDFORGER_REPO only by default (can be disabled for all repository types)
    bool rememberReads; // persist Outline and Note reads (writes to disc on every O/N view)
    std::string externalEditorPath; // path to external MD editor e.g. Emacs or Remarkable

    // GUI configuration
    int fontPointSize;
    bool viewerShowMetadata; // show reads/writes/... when viewing Outlines and/or Notes.
    bool editorShowLineNumbers;
    bool editorEnableSyntaxHighlighting;

private:
    Installer* installer;

public:
    explicit Configuration();
    /**
     * @brief Create Configuration and set active repository.
     */
    explicit Configuration(const std::string& activeRepository);
    virtual ~Configuration();

    Installer* getInstaller() const { return installer; }

    void load();
    void save() const;

    /* On MF start is active repository location is determined as follows:
     *
     * 1) if application has single arg which is dir OR --repository, then use it, else 2)
     * 2) if configuration file exist w/ repository specified, then use it, else 3)
     * 3) if environment variable MINDFORGER_REPOSITORY is set, then use it, else 4)
     * 4) if repository exist in default location ~/mindforger-repository, then use it, else start W/O repository
     */
    void findOrCreateDefaultRepository();
    const Repository* addRepository(const std::string& repositoryPath);
    std::set<const Repository*>& getRepositories();
    const Repository* getActiveRepository() const;
    /**
     * @brief Set active repository - activeRepository parameter must be one of known repositories.
     */
    void setActiveRepository(const Repository* activeRepository);

    const std::string& getMemoryPath() const { return memoryPath; }
    const std::string& getLimboPath() const { return limboPath; }

    const char* getRepositoryPathFromEnv();
    const char* getEditorFromEnv();
    const std::string& getExternalEditor() const { return externalEditor; }

    int getFontPointSize() const { return fontPointSize; }

private:
    void init();
    const std::string getConfigFileName();
    void load(const std::vector<MarkdownAstNodeSection*>* ast);
    const std::string* addRepository(const std::string* repositoryPath);
};

} // namespace

#endif /* M8R_CONFIGURATION_H_ */
