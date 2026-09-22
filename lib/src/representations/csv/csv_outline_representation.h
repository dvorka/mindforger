/*
 csv_outline_representation.h     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_CSV_OUTLINE_REPRESENTATION_H
#define M8R_CSV_OUTLINE_REPRESENTATION_H

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "../../model/outline.h"
#include "../../gear/async_utils.h"
#include "../../gear/file_utils.h"
#include "../../gear/string_utils.h"

namespace m8r {

/**
 * @brief The primary CSV purpose is to be bridge to machine learning world.
 *
 * CSV format is therefore designed to make loading of CSVs as datasets to ML frameworks.
 * No library is used to make things simple - also parsing is not needed, just serialization.
 *
 * @see https://tools.ietf.org/html/rfc4180
 */
class CsvOutlineRepresentation
{
private:
    static const std::string DELIMITER_CSV_HEADER;

public:
    explicit CsvOutlineRepresentation();
    CsvOutlineRepresentation(const CsvOutlineRepresentation&) = delete;
    CsvOutlineRepresentation(const CsvOutlineRepresentation&&) = delete;
    CsvOutlineRepresentation& operator =(const CsvOutlineRepresentation&) = delete;
    CsvOutlineRepresentation& operator =(const CsvOutlineRepresentation&&) = delete;
    virtual ~CsvOutlineRepresentation();

    /**
     * @brief Serialize given Outlines to CSV.
     *
     * @param os                            Outlines to be serialized.
     * @param tagsCardinality               map with Tags cardinality.
     * @param sourceFile                    file where to write CSV.
     * @param oheTagEncodingCardinality     save tags with cardinality equal
     *                                      or higher to given number (0 or bigger),
     *                                      -1 no OHE.
     * @param callbackCtx                   callback instance to report progress.
     * @return                              `true` on success.
     */
    bool to(
        const std::vector<Outline*>& os,
        const std::map<const Tag*,int>& tagsCardinality,
        const filesystem::File& sourceFile,
        int oheTagEncodingCardinality,
        ProgressCallbackCtx* callbackCtx = nullptr
    );

    void toHeader(std::ofstream& out, const std::vector<std::string>& extraColumns);
    void to(Outline* o, const std::vector<const Tag*>& oheTags, std::ofstream& out);
    void toOutlineRow(Outline* o, const std::vector<const Tag*>& oheTags, std::ofstream& out);
    void toNoteRow(Note* n, int offset, const std::vector<const Tag*>& oheTags, std::ofstream& out);

private:
    void quoteValue(const std::string& is, std::string& os);
    void toOheTags(const std::vector<const Tag*>& oheTags, const std::vector<const Tag*>* tags, std::ofstream& out);

};

/**
 * @brief Incremental (resumable) export of Outlines to CSV.
 *
 * Export is split to small steps (CSV rows) so that the caller - like UI event
 * loop - can interleave it with other work, report progress and cancel it.
 * Each Outline and each Note is exported as one CSV row.
 */
class CsvOutlinesExport
{
public:
    enum class Status {
        READY,
        RUNNING,
        FINISHED,
        CANCELLED,
        FAILED
    };

private:
    CsvOutlineRepresentation representation;

    // copy of Outlines vector: Outlines forgotten during export stay allocated in limbo
    const std::vector<Outline*> outlines;
    const std::string fileName;
    std::vector<const Tag*> oheTags;
    std::vector<std::string> escapedOheTags;

    std::ofstream out;
    Status status;

    size_t outlineIndex;
    // 0 ~ Outline row, N ~ (N-1)th Note row
    size_t rowInOutline;
    size_t exportedRows;
    size_t totalRows;

public:
    /**
     * @param os                            Outlines to be serialized.
     * @param tagsCardinality               map with Tags cardinality.
     * @param fileName                      file where to write CSV.
     * @param oheTagEncodingCardinality     save tags with cardinality equal
     *                                      or higher to given number (0 or bigger),
     *                                      -1 no OHE.
     */
    explicit CsvOutlinesExport(
        const std::vector<Outline*>& os,
        const std::map<const Tag*,int>& tagsCardinality,
        const std::string& fileName,
        int oheTagEncodingCardinality
    );
    CsvOutlinesExport(const CsvOutlinesExport&) = delete;
    CsvOutlinesExport(const CsvOutlinesExport&&) = delete;
    CsvOutlinesExport& operator =(const CsvOutlinesExport&) = delete;
    CsvOutlinesExport& operator =(const CsvOutlinesExport&&) = delete;
    virtual ~CsvOutlinesExport();

    /**
     * @brief Open target file and write CSV header.
     * @return `true` on success, `false` if export failed (status is FAILED).
     */
    bool start();

    /**
     * @brief Export at most given number of CSV rows.
     * @return `true` if there is more rows to export, `false` if the export
     *         is not running anymore (finished, failed or cancelled).
     */
    bool step(size_t maxRows);

    /**
     * @brief Cancel running export and delete incomplete target file.
     */
    void cancel();

    Status getStatus() const { return status; }
    const std::string& getFileName() const { return fileName; }
    size_t getExportedRows() const { return exportedRows; }
    size_t getTotalRows() const { return totalRows; }
    /**
     * @brief Get progress in the interval [0.0, 1.0].
     */
    float getProgress() const;

private:
    void fail();
    void closeAndRemoveFile();
};

}
#endif // M8R_CSV_OUTLINE_REPRESENTATION_H
