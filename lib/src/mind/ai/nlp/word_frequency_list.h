/*
 word_frequency_list.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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

#ifdef DO_MF_DEBUG
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

public:
    static constexpr float UNDEF_WEIGHT = -1;

    static void evalUnion(WordFrequencyList& l1, WordFrequencyList& l2, WordFrequencyList& u)
    {
        // IMPROVE u.iterable().insert(l1.iterable().begin(),l1.iterable().end());
        for(auto& e:l1.iterable()) {
            u.add(e.first);
        }

        for(auto& e:l2.iterable()) {
            u.add(e.first);
        }
    }

    static void evalIntersection(WordFrequencyList& l1, WordFrequencyList& l2, WordFrequencyList& u)
    {
        // IMPROVE u.iterable().insert(l1.iterable().begin(),l1.iterable().end());
        for(auto& e:l1.iterable()) {
            if(l2.contains(e.first)) {
                u.add(e.first);
            }
        }
    }

private:
    Lexicon* lexicon;
    WordWeightComparator wordWeightComparator;

    float weight;

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
    const std::map<const std::string*,int>& iterable() const { return word2Frequency; }

    float getWeight() {
        if(weight==UNDEF_WEIGHT) {
            return recalculateWeight();
        } else {
            return weight;
        }
    }

    int contains(const std::string* word) {
        std::map<const std::string*,int>::iterator i = word2Frequency.find(word);
        if(i != word2Frequency.end()) {
            return true;
        } else {
            return false;
        }
    }

    int add(const std::string* word) {
        weight = UNDEF_WEIGHT;

        std::map<const std::string*,int>::iterator i = word2Frequency.find(word);
        if(i != word2Frequency.end()) {
            ++i->second;
            return word2Frequency[word];
        } else {
            word2Frequency[word] = 1;
            return 1;
        }
    }

    void set(const std::string* word, int frequency) {
        word2Frequency[word] = frequency;
    }

    /**
     * @brief Sort words by weight.
     */
    void sort();

    /**
     * @brief Get weight of vector words.
     */
    float recalculateWeight();

#ifdef DO_MF_DEBUG
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
