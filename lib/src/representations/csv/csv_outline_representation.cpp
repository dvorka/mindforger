/*
 csv_outline_representation.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "csv_outline_representation.h"

#include <algorithm>
#include <cstdio>

using namespace std;
using namespace m8r::filesystem;

namespace m8r {

const std::string CsvOutlineRepresentation::DELIMITER_CSV_HEADER = string{","};

CsvOutlineRepresentation::CsvOutlineRepresentation()
{
}

CsvOutlineRepresentation::~CsvOutlineRepresentation()
{
}

/**
 * @brief Serialize O to CSV in "Recent view" style
 *
 * O is serialized as N descriptor, only O/N shared fields are serialized
 * to avoid sparse lines.
 */
bool CsvOutlineRepresentation::to(
    const vector<Outline*>& os,
    const map<const Tag*,int>& tagsCardinality,
    const File& sourceFile,
    int oheTagEncodingCardinality,
    ProgressCallbackCtx* callbackCtx
) {
    static const size_t ROWS_PER_STEP = 1000;

    CsvOutlinesExport csvExport{
        os, tagsCardinality, sourceFile.getName(), oheTagEncodingCardinality
    };
    if(!csvExport.start()) {
        return false;
    }
    while(csvExport.step(ROWS_PER_STEP)) {
        if(callbackCtx) {
            callbackCtx->updateProgress(csvExport.getProgress());
        }
    }

    return csvExport.getStatus() == CsvOutlinesExport::Status::FINISHED;
}

void CsvOutlineRepresentation::toHeader(std::ofstream& out, const vector<string>& extraColumns)
{

    // O/N CSV line
    // id,     type, title, offset, depth, reads, writes, created, modified, read, description
    // string, o/n,  int,   int,    int,   int,   int,    long,    long,     long, string

    string header{};

    string columns[] = {
        "id",
        "type",
        "title",
        "offset",
        "depth",
        "reads",
        "writes",
        "created",
        "modified",
        "read",
        "description"
    };
    for(auto c:columns) {
        header += c;
        header += DELIMITER_CSV_HEADER;
    }
    for(auto c:extraColumns) {
        header += c;
        header += DELIMITER_CSV_HEADER;
    }
    header.pop_back();
    header += "\n";

    out << header;
}

void CsvOutlineRepresentation::to(
    Outline* o, const vector<const Tag*>& oheTags, ofstream& out
) {
    toOutlineRow(o, oheTags, out);

    // Ns: offset <1,inf>
    const vector<Note*>& ns = o->getNotes();
    for(size_t i = 0; i < ns.size(); i++) {
        toNoteRow(ns[i], i+1, oheTags, out);
    }
}

void CsvOutlineRepresentation::toOutlineRow(
    Outline* o, const vector<const Tag*>& oheTags, ofstream& out
) {
    MF_DEBUG("  Exporting O to CSV: " << o->getName() << " / " << o->getKey() << endl);

    string s{};

    out << o->getKey() << ",";
    out << "o,";
    quoteValue(o->getName(), s);
    out << s << ",";
    // O's offset and depth == 0
    out << "0,";
    out << "0,";
    out << o->getReads() << ",";
    out << o->getRevision() << ",";
    out << o->getCreated() << ",";
    out << o->getModified() << ",";
    out << o->getRead() << ",";
    s.clear(); quoteValue(o->getDescriptionAsString(" "), s);
    out << s;

    toOheTags(oheTags, o->getTags(), out);

    out << "\n";
}

void CsvOutlineRepresentation::toNoteRow(
    Note* n, int offset, const vector<const Tag*>& oheTags, ofstream& out
) {
    string s{};

    out << n->getKey() << ",";
    out << "n,";
    quoteValue(n->getName(), s);
    out << s << ",";
    // N's offset: <1,inf>
    out << offset << ",";
    // N's depth: <1,inf>
    out << (n->getDepth()+1) << ",";
    out << n->getReads() << ",";
    out << n->getRevision() << ",";
    out << n->getCreated() << ",";
    out << n->getModified() << ",";
    out << n->getRead() << ",";
    s.clear(); quoteValue(n->getDescriptionAsString(" "), s);
    out << s;

    toOheTags(oheTags, n->getTags(), out);

    out << "\n";
}

void CsvOutlineRepresentation::toOheTags(
    const vector<const Tag*>& oheTags, const vector<const Tag*>* tags, ofstream& out
) {
    for(auto t:oheTags) {
        if(tags && std::find(tags->begin(), tags->end(), t) != tags->end()) {
            out << ",1";
        } else {
            out << ",0";
        }
    }
}

void CsvOutlineRepresentation::quoteValue(const std::string& is, std::string& os)
{
    if(is.size()) {
        os.append(" ");
        os.append(is);

        replaceAll("\"", "\"\"", os);

        os[0] = '\"';
        os.append("\"");
    }
}

/*
 * CsvOutlinesExport
 */

CsvOutlinesExport::CsvOutlinesExport(
    const vector<Outline*>& os,
    const map<const Tag*,int>& tagsCardinality,
    const string& fileName,
    int oheTagEncodingCardinality
)
    : outlines{os},
      fileName{fileName},
      oheTags{},
      escapedOheTags{},
      out{},
      status{Status::READY},
      outlineIndex{0},
      rowInOutline{0},
      exportedRows{0},
      totalRows{0}
{
    // prepare top tags: filter out entries w/ low cardinality
    if(oheTagEncodingCardinality > -1) {
        for(auto t:tagsCardinality) {
            if(t.second >= oheTagEncodingCardinality) {
                oheTags.push_back(t.first);
            }
        }
    }
    for(auto t:oheTags) {
        escapedOheTags.push_back(normalizeToNcName(t->getName(), '_'));
    }

    // O row + N rows
    for(Outline* o:outlines) {
        totalRows += 1 + o->getNotesCount();
    }
}

CsvOutlinesExport::~CsvOutlinesExport()
{
    // never leave incomplete CSV behind
    if(status == Status::RUNNING) {
        cancel();
    }
}

bool CsvOutlinesExport::start()
{
    if(status != Status::READY) {
        return false;
    }

    MF_DEBUG("Exporting Memory to CSV " << fileName << " with " << oheTags.size() << " OHE tags ..." << endl);

    if(fileName.empty()) {
        cerr << "Error: target CSV file name is empty" << endl;
        status = Status::FAILED;
        return false;
    }

    out.open(fileName);
    if(!out.is_open()) {
        cerr << "Error: unable to open CSV file " << fileName << " for writing" << endl;
        status = Status::FAILED;
        return false;
    }

    status = Status::RUNNING;
    representation.toHeader(out, escapedOheTags);
    if(!out) {
        fail();
        return false;
    }

    // nothing to export > finish immediately
    step(0);
    return true;
}

bool CsvOutlinesExport::step(size_t maxRows)
{
    if(status != Status::RUNNING) {
        return false;
    }

    size_t rows{0};
    while(rows < maxRows && outlineIndex < outlines.size()) {
        Outline* o = outlines[outlineIndex];
        const vector<Note*>& ns = o->getNotes();
        if(rowInOutline == 0) {
            representation.toOutlineRow(o, oheTags, out);
        } else if(rowInOutline <= ns.size()) {
            // N's offset: <1,inf>
            representation.toNoteRow(ns[rowInOutline-1], rowInOutline, oheTags, out);
        }
        rows++;

        // move to the next Outline once the last N was exported
        if(++rowInOutline > ns.size()) {
            outlineIndex++;
            rowInOutline = 0;
        }
    }
    exportedRows += rows;

    if(!out) {
        fail();
        return false;
    }

    if(outlineIndex >= outlines.size()) {
        out.flush();
        out.close();
        if(!out) {
            fail();
            return false;
        }
        status = Status::FINISHED;
        MF_DEBUG("FINISHED export of Memory to CSV " << fileName << endl);
        return false;
    }

    return true;
}

void CsvOutlinesExport::cancel()
{
    if(status == Status::READY || status == Status::RUNNING) {
        MF_DEBUG("CANCELLED export of Memory to CSV " << fileName << endl);
        closeAndRemoveFile();
        status = Status::CANCELLED;
    }
}

float CsvOutlinesExport::getProgress() const
{
    if(status == Status::FINISHED) {
        return 1.0f;
    }
    if(totalRows == 0) {
        return 0.0f;
    }
    return static_cast<float>(exportedRows)/static_cast<float>(totalRows);
}

void CsvOutlinesExport::fail()
{
    cerr << "Error: unable to write CSV file " << fileName << endl;
    closeAndRemoveFile();
    status = Status::FAILED;
}

void CsvOutlinesExport::closeAndRemoveFile()
{
    if(out.is_open()) {
        out.close();
    }
    // remove only the file created by this export
    if(status == Status::RUNNING && isFile(fileName.c_str())) {
        std::remove(fileName.c_str());
    }
}

} // m8r namespace
