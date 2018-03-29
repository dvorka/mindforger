/*
 word_frequency_list.h     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_WORD_FREQUENCY_LIST_H
#define M8R_WORD_FREQUENCY_LIST_H

#include <map>
#include <vector>
#include <string>

#ifdef DO_M8F_DEBUG
#include <iostream>
#endif

#include "../../../debug.h"
#include "lexicon.h"

namespace m8r {

/**
 * @brief Word frequency list for a doc.
 *
 * See:
 *   https://en.wikipedia.org/wiki/Word_lists_by_frequency
 */
class WordFrequencyList
{
    struct WordWeightComparator
    {
        Lexicon *l;
        WordWeightComparator(Lexicon* l) : l(l) {}

        // functor to compare words by weight
        bool operator()(
                const std::pair<const std::string* const,int>*const& p1,
                const std::pair<const std::string* const,int>*const& p2
        ) {
            return l->get(p1->first)->weight > l->get(p2->first)->weight;
        }
    };

private:
    WordWeightComparator wordWeightComparator;

    /**
     * @brief List of words occuring in a Thing ordered by weight.
     */
    std::vector<std::pair<const std::string *const,int>*> wordsByWeight;

    /**
     * @brief TRANSIENT map used for quick inserts (can be cleared once list is built).
     */
    std::map<const std::string*,int> word2Frequency;

public:
    explicit WordFrequencyList(Lexicon* lexicon);
    WordFrequencyList(const WordFrequencyList&) = delete;
    WordFrequencyList(const WordFrequencyList&&) = delete;
    WordFrequencyList &operator=(const WordFrequencyList&) = delete;
    WordFrequencyList &operator=(const WordFrequencyList&&) = delete;
    ~WordFrequencyList();

    int& operator[](std::string* key) { return word2Frequency[key]; }
    size_t size() const { return word2Frequency.size(); }
    const std::map<const std::string*,int> iterable() const { return word2Frequency; }

    int add(const std::string* word) {
        int result;
        std::map<const std::string*,int>::iterator i = word2Frequency.find(word);
        if(i != word2Frequency.end()) {
            ++i->second;
            return result;
        } else {
            word2Frequency[word] = 1;
            return 1;
        }
    }

    /**
     * @brief Sort words by weight.
     */
    void sort();

#ifdef DO_M8F_DEBUG
    void print() const {
        std::cout << "WordFrequencyList[" << word2Frequency.size() << "]:" << std::endl;
        for(auto& w:wordsByWeight) {
            std::cout << "  " << (*w->first) << " [" << w->second << "] " << std::endl;
        }
    }
    void printFlat() const {
        for(auto& w:wordsByWeight) {
            std::cout << (*w->first) << " [" << w->second << "] ";
        }
    }
#endif
};

}
#endif // M8R_WORD_FREQUENCY_LIST_H
