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
      notes{},
      noteNames{}
{
}

AutolinkingPreprocessor::~AutolinkingPreprocessor()
{
}

bool nameSizeComparator(const string& n1, const string& n2)
{
    return n1.size() > n2.size();
}

void AutolinkingPreprocessor::updateIndices()
{
    notes.clear();
    noteNames.clear();

    mind.getAllNotes(notes);

    // deduplicate names
    set<string> noteSet;
    for(Note* n:notes) {
        noteSet.insert(n->getName());
    }
    // sort names from longest to shortest (to have best ~ longest matches)
    for(string n:noteSet) {
        noteNames.push_back(n);
    }
    std::sort(noteNames.begin(), noteNames.end(), nameSizeComparator);
}

void AutolinkingPreprocessor::process(const std::vector<std::string*>& md, std::vector<std::string*>& amd)
{
    MF_DEBUG("Autolinker:" << endl);

    // IMPROVE consider sync (in case that it's really needed)
    if(!notes.size()) {
        updateIndices();
    }

    // IMPROVE ORDER of Ns determines what will be found > have active O Ns in head, etc.

    if(md.size()) {
        for(string* l:md) {
            // every line is autolinked SEPARATELY
            string* nl = new string{};
            if(l && l->size()) {
                string w{*l}, chop{};
                MF_DEBUG(">>" << w << ">>" << endl);

                while(w.size()>0) {
                    // find match which is PREFIX of chopped line
                    size_t found;
                    bool linked = false;
                    // IMPROVE loop to be changed to Aho-Corasic trie
                    for(Note* n:notes) {
                        if((found=w.find(n->getName())) != string::npos
                              &&
                            !found)
                        {
                            // avoid word PREFIX matches ~ ensure that WHOLE world is matched

                            string m{" \t,:;.!?<>{}&()-+/*"};
                            char c{w.size()==n->getName().size()?' ':w.at(n->getName().size())};
                            MF_DEBUG("  c: '" << c << "'" << endl);
                            if(w.size()==n->getName().size()
                                 ||
                               m.find(c)!=string::npos)
                            {
                                linked = true;

                                nl->append(l->substr(0,found));
                                nl->append("[");
                                nl->append(n->getName());
                                nl->append("](");
                                nl->append(n->getOutlineKey());
                                nl->append("#");
                                nl->append(n->getMangledName());
                                nl->append(")");

                                *nl += c;

                                // chop linked prefix word
                                w = w.substr(n->getName().size()+(w.size()==n->getName().size()?0:1));

                                break;
                            }
                        }
                    }

                    // chop one world from the beginning
                    MF_DEBUG("   l>" << std::boolalpha << linked << endl);
                    if(linked) {
                        // prefix has been linked + chopped
                        // IMPROVE SPACE vs. TAB
                        nl->append(" ");
                    } else {
                        // current w prefix was NOT linked > chop it and append it
                        size_t begin = w.find_first_of(" \t");
                        if(begin != string::npos) {
                            chop = w.substr(0, begin);
                            w = w.substr(begin+1);
                            nl->append(chop);
                            nl->append(" ");

                            MF_DEBUG("  -c>" << chop << endl);
                            MF_DEBUG("   w>" << w << endl);
                            MF_DEBUG("   <<" << *nl << endl);
                        } else {
                            // no more words (prefix already checked) > DONE
                            nl->append(w);

                            MF_DEBUG("   w>" << w << endl);
                            MF_DEBUG("   <<" << *nl << endl);

                            break;
                        }
                    }
                }

                MF_DEBUG("<<" << *nl << "<<" << endl);
                amd.push_back(nl);
            } else {
                amd.push_back(nl);
            }
        }
    }
}

} // m8r namespace
