/*
 lexicon.h     MindForger thinking notebook

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
#ifndef M8R_LEXICON_H
#define M8R_LEXICON_H

#include <map>
#include <vector>
#include <string>

#ifdef DO_MF_DEBUG
#include <iostream>
#endif

#include "../../../debug.h"
#include "../../../gear/lang_utils.h"

namespace m8r {

/**
 * @brief Lexicon of all words w/ global frequencies.
 *
 * Lexicon is the *only* data structure in MF's AI that keeps words by *value*.
 * Other data structures use either pointers or references to be memory efficient.
 *
 */
// IMPROVE Stanford GloVe lexicon w/ word attributes & semantic domains (configure > check existence > use OR skip)
class Lexicon
{
public:

    struct WordEmbedding {
        // IMPROVE consider use of ptr to map's key
        std::string word;
        int frequency;
        float weight;

        explicit WordEmbedding() {
            frequency = 0;
            weight = 0.;
        }
        explicit WordEmbedding(const std::string& ww, int f, float w) {
            word = ww;
            frequency = f;
            weight = w;
        }
    };


private:
    // map of word-to-frequency pairs for fast lookup and duplicity detection
    // IMPROVE C++ SL uses map's comparator for string to do comparisons - thefore insert
    //         complexity is O(log(n)) string comparisons. Consider TRIE data structure
    //         instead.
    std::map<std::string,WordEmbedding> m;

    // sorted/ordered words by weight/frequency/... referencing/pointing to map
    //std::vector<WordEmbedding*> words;

    // keeping max word frequency for efficient weighs calculation
    int maxFrequency;

public:
    explicit Lexicon();
    Lexicon(const Lexicon&) = delete;
    Lexicon(const Lexicon&&) = delete;
    Lexicon &operator=(const Lexicon&) = delete;
    Lexicon &operator=(const Lexicon&&) = delete;
    ~Lexicon();

    size_t size() const { return m.size(); }
    void clear() { m.clear(); }
    const std::map<std::string,WordEmbedding>& get() const { return m; }

    WordEmbedding* get(const std::string& word) {
        std::map<std::string,WordEmbedding>::iterator i = m.find(word);
        if(i != m.end()) {
            return &i->second;
        } else {
            return nullptr;
        }
    }
    WordEmbedding* get(const std::string* word) {
        return get(*word);
    }

    WordEmbedding* add(const std::string& word) {
        WordEmbedding* result;
        if((result=get(word)) != nullptr) {
            ++result->frequency;
            if(result->frequency>maxFrequency) maxFrequency=result->frequency;
            return result;
        } else {
            WordEmbedding we{word,1,0};
            m[word] = we;
            return &(m[word]);
        }
    }
    WordEmbedding* add(const std::string* word) {
        return add(*word);
    }

    /**
     * @brief Recalculate word weights.
     *
     * Formula:
     *   maxFrequency()       ... maximum frequency across all words in lexicon
     *   %(w)                 ... w frequency is %(w) percent of maxFrequency()
     *   weight(w) = 1 - %(w) ... weight definition
     *     dom(weight) = word
     *     range(weight) = [0,1]
     *
     * See also:
     *   https://www.youtube.com/watch?v=KIT-LbvNt_I&list=PLBv09BD7ez_77rla9ZYx-OAdgo2r9USm4
     *
     */
    void recalculateWeights() {
        for(auto& e:m) {
            e.second.weight =  1. - ((((float)e.second.frequency)/100.) / (((float)maxFrequency)/100.));

            // IMPROVE fixed constant is eight too big or small
            // ensure max(w)'s weigh to be > 0
            if(!e.second.weight) e.second.weight = 0.01;
        }
    }

#ifdef DO_MF_DEBUG
    void print() const {
        MF_DEBUG("Lexicon[" << m.size() << "]:" << std::endl);
        for(auto& e:m) {
            MF_DEBUG("  " << e.second.word << "  " << e.second.frequency << "  " << e.second.weight << std::endl);
        }
    }
#endif
};

}
#endif // M8R_LEXICON_H
