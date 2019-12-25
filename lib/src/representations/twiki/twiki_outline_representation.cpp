/*
 twiki_outline_representation.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "twiki_outline_representation.h"

namespace m8r {

using namespace std;

TWikiOutlineRepresentation::TWikiOutlineRepresentation(
        MarkdownOutlineRepresentation& markdownRepresentation,
        Persistence* persistence
)
    : markdownRepresentation(markdownRepresentation),
      persistence(persistence)
{
}

TWikiOutlineRepresentation::~TWikiOutlineRepresentation()
{
}

bool TWikiOutlineRepresentation::outline(const m8r::File& sourceFile, const m8r::File& outlineFile)
{
    // straightforward conversion: only sections are converted, the rest is kept intact
    MF_DEBUG("TWiki export of " << sourceFile.getName() << " to " << outlineFile.getName() << " ... " << endl);
    unsigned long int fileSize = 0;
    lines.clear();
    if(fileToLines(&sourceFile.getName(), lines, fileSize)) {
        if(lines.size()) {
            std::ofstream out(outlineFile.getName());
            string section{};
            unsigned offset;
            for(string* l:lines) {
                if(l) {
                    if(l->size()) {
                        if(stringStartsWith(*l,"---+")) {
                            // TWiki section > MD section
                            section.clear();
                            offset = 3;
                            while(l->at(offset)=='+') {
                                section.append("#");
                                if(l->size() < ++offset) {
                                    break;
                                }
                            }
                            if(l->size() > offset) {
                                section.append(l->substr(offset));
                            }
                            out << section << endl;
                        } else {
                            out << *l << endl;
                        }
                    }
                    delete l;
                }
            }
            out.close();
            lines.clear();
        } else {
            return false;
        }
    } else {
        return false;
    }

    MF_DEBUG("MD for TWiki written to file - loading MD to create new O..." << endl);
    Outline* o = markdownRepresentation.outline(outlineFile);

    // save outline to target destination
    persistence->save(o);

    // IMPORTANT: TWiki file is concerted, but NOT loaded to mind - repository must be RELOADED
    //            to ensure correct indexation of the new O + no need to solve MF modes (repo, single file, ...)

    delete o;

    return true;
}

} // m8r namespace
