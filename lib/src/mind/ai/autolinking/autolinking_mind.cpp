/*
 autolinking_mind.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "../../mind.h"

#ifdef MF_MD_2_HTML_CMARK

namespace m8r {

using namespace std;

const vector<string> AutolinkingMind::excludedWords({"http", "https"});

AutolinkingMind::AutolinkingMind(Mind& mind)
    : mind{mind},
      trie{nullptr}
{
}

AutolinkingMind::~AutolinkingMind()
{
    if(trie) {
        delete trie;
        trie = nullptr;
    }
}

bool AutolinkingMind::aliasSizeComparator(const Thing* t1, const Thing* t2)
{
    return t1->getAutolinkingAlias().size() > t2->getAutolinkingAlias().size();
}

void AutolinkingMind::updateTrieIndex()
{
    // IMPROVE update indices only if an O/N is modified (except writing read timestamps)

#ifdef DO_MF_DEBUG
    MF_DEBUG("[Autolinking] Rebuilding trie index..." << endl);
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
        addThingToTrie(o);
    }

    // Ns
    std::vector<Note*> notes;
    mind.getAllNotes(notes);
#ifdef DO_MF_DEBUG
    size += notes.size();
#endif
    for(Note* n:notes) {
        addThingToTrie(n);
    }

    // remove excluded words whose autolinking breaks
    // Markdown structure (like e.g. http:// in links)
    for(const string& s:this->excludedWords) {
        trie->removeWord(s);
    }

    // IMPROVE: add also tags

#ifdef DO_MF_DEBUG
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG(
        "[Autolinking] trie w/ " << size << " things updated in: "
        << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000000.0 << "ms" << endl);
#endif
}

string AutolinkingMind::getLowerName(const std::string& name)
{
    string lowerName{name};
    lowerName[0] = std::tolower(name[0]);
    return lowerName;
}

void AutolinkingMind::addThingToTrie(const Thing *t) {
    // name
    trie->addWord(t->getAutolinkingName());
    // name w/ lowercase 1st letter
    trie->addWord(getLowerName(t->getAutolinkingName()));
    // abbrev (if present)
    trie->addWord(t->getAutolinkingAbbr());
}

void AutolinkingMind::removeThingFromTrie(const Thing *t) {
    trie->removeWord(t->getAutolinkingName());
    trie->removeWord(getLowerName(t->getAutolinkingName()));
    trie->removeWord(t->getAutolinkingAbbr());
}

void AutolinkingMind::update(const std::string& oldName, const std::string& newName)
{
    MF_DEBUG("Autolink update: '" << oldName << " > '" << newName << "'" << endl);

    if(oldName.compare(newName)) {
        if(oldName.size()) {
            Thing t{oldName};
            removeThingFromTrie(&t);
        }
        if(newName.size()) {
            Thing t{newName};
            addThingToTrie(&t);
        }
    }

    MF_DEBUG("DONE autolink update: '" << oldName << "' > '" << newName << "'" << endl);
}

void AutolinkingMind::clear()
{
    if(trie) {
        delete trie;
    }
    trie = new Trie{};

    MF_DEBUG("[Autolinking] indices CLEARed" << endl);
}

} // m8r namespace
#endif // MF_MD_2_HTML_CMARK
