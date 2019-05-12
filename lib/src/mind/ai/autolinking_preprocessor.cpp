/*
 autolinking_preprocessor.cpp     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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

const string AutolinkingPreprocessor::CODE_BLOCK = string{"```"};
const string AutolinkingPreprocessor::MATH_BLOCK = string{"$$"};

AutolinkingPreprocessor::AutolinkingPreprocessor(Mind& mind)
    : things{},
      trie{nullptr},
      insensitive{true},
      mind{mind}
{
}

AutolinkingPreprocessor::~AutolinkingPreprocessor()
{
}

bool AutolinkingPreprocessor::aliasSizeComparator(const Thing* t1, const Thing* t2)
{
    return t1->getAutolinkingAlias().size() > t2->getAutolinkingAlias().size();
}

void AutolinkingPreprocessor::updateIndices()
{
    // IMPROVE update indices only if an O/N is modified (except writing read timestamps)

#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] Updating indices..." << endl);
    auto begin = chrono::high_resolution_clock::now();
#endif

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

#ifdef DO_MF_DEBUG
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG("  Indices updated in: " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000000.0 << "ms" << endl);
#endif
}

void AutolinkingPreprocessor::updateTrieIndex()
{
    // IMPROVE update indices only if an O/N is modified (except writing read timestamps)

#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] Updating trie index..." << endl);
    auto begin = chrono::high_resolution_clock::now();
#endif

    string lowerName{};
    int size{};

    // clear
    if(trie) {
        delete trie;
    }
    trie = new Trie{};

    // Os
    const vector<Outline*>& os=mind.getOutlines();
    size = os.size();
    for(Outline* o:os) {
        // TODO add name (w/o abbrev), lower name and abbrev (if exists)
        // getAutolinkingName()
        // getAutolinkingAbbrev()
        // ... keep getAutolinkingAlias() as is

        // add name w/ 1st lower case letter
        lowerName.assign(o->getAutolinkingAlias());
        lowerName[0] = std::tolower(o->getAutolinkingAlias()[0]);
        trie->addWord(lowerName);
        //MF_DEBUG("  '" << lowerName << "'" << endl);
        // add abbrev (if present)
        trie->addWord(o->getAutolinkingAlias());
        //MF_DEBUG("  '" << o->getAutolinkingAlias() << "'" << endl);
    }

    // Ns
    std::vector<Note*> notes;
    mind.getAllNotes(notes);
    size += notes.size();
    for(Thing* n:notes) {
        // TODO make this function

        // add name w/ 1st lower case letter
        lowerName.assign(n->getAutolinkingAlias());
        lowerName[0] = std::tolower(n->getAutolinkingAlias()[0]);
        trie->addWord(lowerName);
        //MF_DEBUG("  '" << lowerName << "'" << endl);
        // add abbrev (if present)
        trie->addWord(n->getAutolinkingAlias());
        //MF_DEBUG("  '" << n->getAutolinkingAlias() << "'" << endl);
    }

#ifdef DO_MF_DEBUG
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG("[Autolinking] trie w/ " << size << " things updated in: " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000000.0 << "ms" << endl);
#endif
}

} // m8r namespace
