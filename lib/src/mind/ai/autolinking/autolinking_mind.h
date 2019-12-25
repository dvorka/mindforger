/*
 autolinking_mind.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_AUTOLINKING_MIND_H
#define M8R_AUTOLINKING_MIND_H

#ifdef MF_MD_2_HTML_CMARK

#include <vector>
#include <chrono>

#include "../../../debug.h"
#include "../../ontology/thing_class_rel_triple.h"
#include "../../../gear/trie.h"

namespace m8r {

class Mind;

/**
 * @brief Autolinking indices and inferences.
 */
class AutolinkingMind
{
private:
    Mind& mind;

    Trie* trie;

public:
    explicit AutolinkingMind(Mind& mind);
    AutolinkingMind(const AutolinkingMind&) = delete;
    AutolinkingMind(const AutolinkingMind&&) = delete;
    AutolinkingMind &operator=(const AutolinkingMind&) = delete;
    AutolinkingMind &operator=(const AutolinkingMind&&) = delete;
    ~AutolinkingMind();

    /**
     * @brief Rebuild indices (like trie) e.g. on new MD/repository load.
     */
    void reindex() {
        updateTrieIndex();
    }

    /**
     * @brief Update indices on a thing rename.
     */
    void update(const std::string& oldName, const std::string& newName);

    /**
     * @brief Find longest autolinking match.
     */
    bool findLongestPrefixWord(std::string& s, std::string& r) const {
        return trie->findLongestPrefixWord(s, r);
    }

    /**
     * @brief Clear indices.
     */
   void clear();

protected:
    /**
     * @brief Comparator used to sort Os/Ns by name (w/ stripped abbreviation prefix).
     */
    static bool aliasSizeComparator(const Thing* t1, const Thing* t2);

    static std::string getLowerName(const std::string& name);

    /**
     * @brief Update trie-based Os and Ns names index.
     */
    void updateTrieIndex();

    /**
     * @brief Add thing's name (and abbrev) to trie.
     */
    void addThingToTrie(const Thing *t);

    /**
     * @brief Remove thing's name (and abbrev) from trie.
     */
    void removeThingFromTrie(const Thing *t);
};

}
#endif // MF_MD_2_HTML_CMARK
#endif // M8R_AUTOLINKING_MIND_H
