/*
 autolinking_mind.cpp     MindForger thinking notebook

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
#include "autolinking_mind.h"

namespace m8r {

AutolinkingMind::AutolinkingMind(Mind& mind)
    : mind{mind}
{
    if(trie) {
        delete trie;
        trie = nullptr;
    }
}

AutolinkingMind::~AutolinkingMind()
{
}

bool AutolinkingMind::aliasSizeComparator(const Thing* t1, const Thing* t2)
{
    return t1->getAutolinkingAlias().size() > t2->getAutolinkingAlias().size();
}

void Mind::autolinkUpdateThingsIndex()
{
    // IMPROVE update indices only if an O/N is modified (except writing read timestamps)

#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] Updating indices..." << endl);
    auto begin = chrono::high_resolution_clock::now();
#endif

    autolinkThings.clear();

    // Os
    std::vector<Outline*> outlines;
    const vector<Outline*>& os=mind.getOutlines();
    for(Outline* o:os) outlines.push_back(o);
    std::sort(outlines.begin(), outlines.end(), aliasSizeComparator);
    for(Thing* t:outlines) autolinkThings.push_back(t);

    // Ns
    std::vector<Note*> notes;
    mind.getAllNotes(notes);
    // sort names from longest to shortest (to have best ~ longest matches)
    std::sort(notes.begin(), notes.end(), aliasSizeComparator);
    for(Thing* t:notes) autolinkThings.push_back(t);

#ifdef DO_MF_DEBUG
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG("  Indices updated in: " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000000.0 << "ms" << endl);
#endif
}

void Mind::autolinkUpdateTrieIndex()
{
    // IMPROVE update indices only if an O/N is modified (except writing read timestamps)

#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] Updating trie index..." << endl);
    auto begin = chrono::high_resolution_clock::now();
    int size{};
#endif

    clear();

    // Os
    const vector<Outline*>& os=mind.getOutlines();
#ifdef DO_MF_DEBUG
    size = os.size();
#endif
    for(Outline* o:os) {
        autolinkAddThingToTrie(o);
    }

    // Ns
    std::vector<Note*> notes;
    mind.getAllNotes(notes);
#ifdef DO_MF_DEBUG
    size += notes.size();
#endif
    for(Note* n:notes) {
        autolinkAddThingToTrie(n);
    }

    // IMPROVE: add also tags

#ifdef DO_MF_DEBUG
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG("[Autolinking] trie w/ " << size << " things updated in: " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000000.0 << "ms" << endl);
#endif
}

void Mind::autolinkAddThingToTrie(const Thing *t) {
    // name
    autolinkTrie->addWord(t->getAutolinkingName());
    // name w/ lowercase 1st letter
    string lowerName{t->getAutolinkingName()};
    lowerName[0] = std::tolower(t->getAutolinkingName()[0]);
    autolinkTrie->addWord(lowerName);
    // abbrev (if present)
    autolinkTrie->addWord(t->getAutolinkingAbbr());
}

void Mind::autolinkClear()
{
    autolinkThings.clear();

    if(autolinkTrie) {
        delete autolinkTrie;
    }
    autolinkTrie = new Trie{};

    MF_DEBUG("[Autolinking] indices CLEARed" << endl);
}

} // m8r namespace
