/*
 autolinking_preprocessor.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "autolinking_preprocessor.h"

namespace m8r {

using namespace std;

AutolinkingPreprocessor::AutolinkingPreprocessor(Mind& mind)
    : mind(mind),
      noteNames{}
{
    mind.getAllNotes(notes);
    for(Note* n:notes) {
        noteNames.insert(n->getName());
    }
}

AutolinkingPreprocessor::~AutolinkingPreprocessor()
{
}

void AutolinkingPreprocessor::process(const std::vector<std::string*>& md, std::vector<std::string*>& amd)
{
    MF_DEBUG("Autolinker..." << endl);

    if(md.size()) {
        for(string* l:md) {
            // every line to be autolinked separately
            size_t found;
            if(l && l->size()) {
                MF_DEBUG("B>" << *l << endl);
                string* nl = new string{};

                // find longest match
                // IMPROVE Aho-Corasick @ trie
                // IMPROVE the first match is found
                for(Note* n:notes) {
                    if((found=l->find(n->getName())) != string::npos) {
                        // make it link
                        nl->append(l->substr(0,found));
                        nl->append("[");
                        nl->append(n->getName());
                        nl->append("](");
                        nl->append(n->getOutlineKey());
                        nl->append("#");
                        nl->append(n->getMangledName());
                        nl->append(")");

                        amd.push_back(nl);
                    }
                    // IMPROVE match > 1 on the line
                }
            }
        }
    }
}

} // m8r namespace
