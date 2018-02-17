/*
 repository.h     MindForger thinking notebook

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
#ifndef M8R_REPOSITORY_H
#define M8R_REPOSITORY_H

#include <string>

#include "../debug.h"

namespace m8r {

/**
 * @brief A knowledge repository to be learned by MindForger.
 */
class Repository
{
public:
    /**
     * @brief Repository type.
     *
     * MindForger repository is a directory with sub-directories enabling advanced
     * knowledge organization and memory/ directory with Outlines written in Markdown-based
     * DSL (metadata enriched) format.
     *
     * Markdown repository is any directory structure containing Markdown files.
     */
    enum RepositoryType {
        MINDFORGER,
        MARKDOWN
    };

    /**
     * @brief Repository mode.
     *
     * Configuration has always set an active (MF or MD) repository.
     *
     * Typically MindForger manages the whole repository (REPOSITORY mode) i.e. all
     * files in it.
     *
     * Alternatively user may want to edit just one file (FILE mode) and in this case
     * the content of the repository is filtered to given file only (file field).
     */
    enum RepositoryMode {
        REPOSITORY,
        FILE
    };

private:
    std::string path;
    std::string file;
    RepositoryType type;
    RepositoryMode mode;
    bool readOnly;

public:
    explicit Repository(
        std::string path,
        RepositoryType type=RepositoryType::MINDFORGER,
        RepositoryMode mode=RepositoryMode::REPOSITORY,
        bool readOnly=false);
    Repository(const Repository&) = delete;
    Repository(const Repository&&) = delete;
    Repository &operator=(const Repository&) = delete;
    Repository &operator=(const Repository&&) = delete;
    ~Repository();

    const std::string& getPath() const { return path; }
    void setPath(std::string path) { this->path = path; }
    RepositoryType getType() const { return type; }
    void setType(RepositoryType type) { this->type = type; }
    void setRepositoryMode(RepositoryMode mode) { this->mode=mode; }
    RepositoryMode getRepositoryMode() const { return mode; }
    bool isReadOnly() const { return readOnly; }
    void setReadOnly(bool readOnly) { this->readOnly = readOnly; }

#ifdef DO_MF_DEBUG
    void print() const {
        MF_DEBUG(std::endl << "Path: " << path << ", file: " << file << ", type: " << type << ", mode: " << mode << ", RD_ONLY: " << readOnly);
    }
#endif
};

}
#endif // M8R_REPOSITORY_H
