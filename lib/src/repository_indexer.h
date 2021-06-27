/*
 repository_indexer.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_REPOSITORY_INDEXER_H_
#define M8R_REPOSITORY_INDEXER_H_

#include "config/config.h"
#include <sys/types.h>
#ifndef _WIN32
    #include <unistd.h>
#endif /* _WIN32 */

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <vector>

#include "debug.h"
#include "gear/file_utils.h"
#include "gear/string_utils.h"
#include "config/configuration.h"

namespace m8r {

/**
 * @brief MindForger/Markdown repository/file indexer.
 */
class RepositoryIndexer {
public:
    /**
     * @brief Check whether given directory contains a MindForger repository.
     */
    static bool isMindForgerRepository(const std::string& directory);

    /**
     * @brief Detect repository path and mode for given path.
     */
    static Repository* getRepositoryForPath(const std::string& path);

    /**
     * @brief Make path relative if it makes sense for given repository.
     *
     * If target absolute path is in the current repository, then construct
     * relative path from current absolute path to target absolute path.
     */
    static std::string makePathRelative(
            const Repository* currentRepository,
            const std::string& srcAbsoluteFile,
            const std::string& dstAbsolutePath,
            const bool dstIsFile=true);

    /**
     * @brief Does file has one of supported Markdown extensions?
     */

    // TODO instead of hard-coded extensions use MIME types to support/work
    //      support and work with (new) extensions dynamically
    // TODO extensions as content type parsers could be registered dynamically
    //      just by name specification
    static bool fileHasMarkdownExtension(const std::string& filename);
    // TODO PDF extension >> make it MIME
    static bool fileHasPdfExtension(const std::string& filename);
    // text extension >> make it MIME
    static bool fileHasTextExtension(const std::string& filename);

private:
    Repository* repository;

    /**
     * @brief Path to MindForger memory e.g. ~/my-mf-repo/memory
     */
    std::string memoryDirectory;
    std::string outlineStencilsDirectory;
    std::string noteStencilsDirectory;

    std::set<const std::string*> allFiles;
    std::set<const std::string*> markdowns;
    std::set<const std::string*> outlineStencils;
    std::set<const std::string*> noteStencils;

    /*
     * DIKW: information artifacts
     */

    // PDFs
    std::set<const std::string*> pdfs;
    // TXTs
    std::set<const std::string*> texts;

public:
    explicit RepositoryIndexer();
    RepositoryIndexer(const RepositoryIndexer&) = delete;
    RepositoryIndexer(const RepositoryIndexer&&) = delete;
    RepositoryIndexer& operator=(const RepositoryIndexer&) = delete;
    RepositoryIndexer& operator=(const RepositoryIndexer&&) = delete;
    virtual ~RepositoryIndexer();

    /**
     * @brief Index new repository - any type, any modes.
     */
    void index(Repository* repository);

    /**
     * @brief Smart (dirty) update of the repository index.
     */
    void updateIndex();

    /**
     * @brief Clear all fields.
     */
    void clear();

    Repository* getRepository() const { return repository; }

    const std::set<const std::string*> getMarkdownFiles() const;
    const std::set<const std::string*> getPdfFiles() const;
    const std::set<const std::string*> getTextFiles() const;
    const std::set<const std::string*> getAllOutlineFileNames() const;
    const std::set<const std::string*> getOutlineStencilsFileNames() const;
    const std::set<const std::string*> getNoteStencilsFileNames() const;
    char* getTagsFromPath();

private:
    void updateIndexMemory(const std::string& directory);
    void updateIndexStencils(const std::string& directory, std::set<const std::string*>& stencils);
};

} /* namespace */

#endif /* M8R_REPOSITORY_INDEXER_H_ */
