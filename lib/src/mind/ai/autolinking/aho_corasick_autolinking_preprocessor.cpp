/*
 aho_corasick_autolinking_preprocessor.cpp     MindForger thinking notebook

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
#include "aho_corasick_autolinking_preprocessor.h"

namespace m8r {

using namespace std;

AhoCorasickAutolinkingPreprocessor::AhoCorasickAutolinkingPreprocessor(Mind& mind)
    : AutolinkingPreprocessor{},
      mind(mind),
      things{}
{
}

AhoCorasickAutolinkingPreprocessor::~AhoCorasickAutolinkingPreprocessor()
{
}

void AhoCorasickAutolinkingPreprocessor::rebuildSearchStructure()
{
    Trie* trie = new Trie{};


    // TODO rewrite...

    things.clear();

    // Os
    std::vector<Outline*> outlines;
    const vector<Outline*>& os=mind.getOutlines();
    for(Outline* o:os) outlines.push_back(o);
    std::sort(outlines.begin(), outlines.end(), aliasSizeComparator);
    for(Thing* t:outlines) things.push_back(t);

    // Ns
    std::vector<Note*> notes;
    mind.getAllNotes(notes);
    // sort names from longest to shortest (to have best ~ longest matches)
    std::sort(notes.begin(), notes.end(), aliasSizeComparator);
    for(Thing* t:notes) things.push_back(t);

    // TODO build trie
}

} // m8r namespace
