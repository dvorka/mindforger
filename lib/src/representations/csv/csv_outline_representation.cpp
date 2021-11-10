/*
 csv_outline_representation.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
    MF_DEBUG("Exporting Memory to CSV "
        << sourceFile.getName()
        << " with OHE " << oheTagEncodingCardinality << " ..."
        << endl
    );

    if(sourceFile.getName().size()) {
        if(os.size()) {
            // prepare top tags: filter out entries w/ low cardinality
            vector<const Tag*> oheTags{};
            if(oheTagEncodingCardinality > -1) {
                for(auto t:tagsCardinality) {
                    if(t.second >= oheTagEncodingCardinality) {
                        oheTags.push_back(t.first);
                    }
                }
            }
            vector<string> escapedOheTags{};
            for(auto t:oheTags) {
                escapedOheTags.push_back(normalizeToNcName(t->getName(), '_'));
            }

            std::ofstream out{};
            try {
                out.open(sourceFile.getName());

                float exported{0.0};
                toHeader(out, escapedOheTags);
                for(Outline* o:os) {
                    MF_DEBUG("  Exporting O: " << o->getName() << " / " << o->getKey() << endl);
                    to(o, oheTags, out);

                    if(callbackCtx) {
                        callbackCtx->updateProgress(++exported/(float)os.size());
                    }
                }
            } catch(const std::ofstream::failure& e) {
                cerr << "Error: unable to open/write file "
                     << sourceFile.getName()
                     << " " << e.what();
                try {
                    out.close();
                } catch(const std::ofstream::failure& e) {}

                return false;
            }
            out.flush();
            out.close();

            MF_DEBUG("FINISHED export of MIND to CSV " << sourceFile.getName() << endl);
            return true;
        }
    } else {
        cerr << "Error: target file name is empty";
    }

    return false;
}

void CsvOutlineRepresentation::toHeader(std::ofstream& out, vector<string>& extraColumns)
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
    Outline* o, vector<const Tag*> oheTags, ofstream& out
) {
    MF_DEBUG("\n  " << o->getName());

    string s{};

    // O
    out << o->getKey() << ",";
    out << "o,";
    s.clear(); quoteValue(o->getName(), s);
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

    for(auto t:oheTags) {
        if(o->hasTag(t)) {
            out << ",1";
        } else {
            out << ",0";
        }
    }

    out << "\n";

    // Ns
    const vector<Note*>& ns = o->getNotes();
    int offset = 1;
    for(Note* n:ns) {
        MF_DEBUG("    " << n->getName());
        out << n->getKey() << ",";
        out << "n,";
        s.clear(); quoteValue(n->getName(), s);
        out << s << ",";
        // N's offset: <1,inf>
        out << offset++ << ",";
        // N's depth: <1,inf>
        out << (n->getDepth()+1) << ",";
        out << n->getReads() << ",";
        out << n->getRevision() << ",";
        out << n->getCreated() << ",";
        out << n->getModified() << ",";
        out << n->getRead() << ",";
        MF_DEBUG(" B ");
        s.clear(); quoteValue(n->getDescriptionAsString(" "), s);
        MF_DEBUG(" F ");
        out << s;

        for(auto t:oheTags) {
            if(n->hasTag(t)) {
                out << ",1";
            } else {
                out << ",0";
            }
        }

        out << "\n";
        MF_DEBUG(" ... DONE" << endl);
        out.flush();
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

} // m8r namespace
