/*
 filesystem_information.h     MindForger thinking notebook

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
#ifndef M8R_FILESYSTEM_INFORMATION_H
#define M8R_FILESYSTEM_INFORMATION_H

#include "information.h"
#include "../../config/configuration.h"
#include "../../gear/file_utils.h"
#include "../../gear/string_utils.h"
#include "../../persistence/filesystem_persistence.h"
#include "../../representations/markdown/markdown_document_representation.h"

namespace m8r {

class FilesystemInformationSource : InformationSource
{
private:
    // TXT
    std::set<const std::string*> txts;
    // PDF
    std::set<const std::string*> pdfs;
    // HTML
    std::set<const std::string*> htmls;

    // MS Office Word
    std::set<const std::string*> msOfficeWords;
    // MS Office Powerpoint
    std::set<const std::string*> msOfficePowerpoint;
    // MS Office Excel
    std::set<const std::string*> msOfficeExcel;

    // Open Office Word
    std::set<const std::string*> openOfficeWords;
    // Open Office Powerpoint
    std::set<const std::string*> openOfficePowerpoint;
    // Open Office Excel
    std::set<const std::string*> openOfficeExcel;

    FilesystemPersistence& persistence;
    MarkdownDocumentRepresentation& mdDocumentRepresentation;

public:
    explicit FilesystemInformationSource(
       std::string& sourcePath,
       FilesystemPersistence& persistence,
       MarkdownDocumentRepresentation& mdDocumentRepresentation
    );
    FilesystemInformationSource(const FilesystemInformationSource&) = delete;
    FilesystemInformationSource(const FilesystemInformationSource&&) = delete;
    FilesystemInformationSource &operator=(const FilesystemInformationSource&) = delete;
    FilesystemInformationSource &operator=(const FilesystemInformationSource&&) = delete;
    ~FilesystemInformationSource();

    /**
     * @brief Index this information source to memory.
     *
     * Scan given filesystem directory, find know files
     * and convert them to Os/Ns/Ts/... in memory so that
     * the information might be used.
     *
     * For instance index given filesystem information by creating Markdown
     * descriptor in $MINDFORGER_REPOSITORY/memory for each file
     * found in given information source.
     *
     * Orphan detection and management:
     * - list of library's Os in memory is scanned before documents indexation
     * - if a document does NOT have valid path to document, then it is TAGGED
     *   with `orphan` tag
     * - if there is clash of files i.e. O already exists on the filesystem,
     *   then it is not rewritten (avoid loss of user remarks).
     *
     * @return true if source was successfully indexed, false otherwise.
     */
    bool indexToMemory(Repository& repository);

    std::set<const std::string*> getPdfs() const { return this->pdfs; }

private:
    void indexDirectoryToMemory(const std::string& directory, const std::string& memoryPath);
};

}
#endif // M8R_FILESYSTEM_INFORMATION_H
