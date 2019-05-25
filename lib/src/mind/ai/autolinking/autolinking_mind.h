/*
 autolinking_mind.h     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <vector>

#include "../../../gear/trie.h"
#include "../../mind.h"

namespace m8r {

/**
 * @brief Things autolinking indices and inferences.
 */
class AutolinkingMind
{
private:
    std::vector<Thing*> autolinkThings;
    Trie* autolinkTrie;

    Mind& mind;

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
    virtual void reindex() = 0;

    /**
     * @brief Clear indices.
     */
    virtual void clear();

protected:
    /**
     * @brief Comparator used to sort Os/Ns by name (w/ stripped abbreviation prefix).
     */
    static bool aliasSizeComparator(const Thing* t1, const Thing* t2);

    /**
     * @brief Update indice of all Ns and Os.
     */
    void updateThingsIndex();
    /**
     * @brief Update trie-based Os and Ns names index.
     */
    void updateTrieIndex();
    /**
     * @brief Add thing's name (and abbrev) to trie.
     */
    void addThingToTrie(const Thing *t);
};

}
#endif // M8R_AUTOLINKING_MIND_H
