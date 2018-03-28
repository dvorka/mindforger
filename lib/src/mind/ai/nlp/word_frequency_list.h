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

namespace m8r {

/**
 * @brief Word frequency list.
 *
 * See:
 *   https://en.wikipedia.org/wiki/Word_lists_by_frequency
 */
class WordFrequencyList
{
private:
    /**
     * @brief List of words ordered by frequences.
     */
    std::vector<std::pair<std::string,int> list;

    /**
     * @brief TRANSIENT map used for quick inserts (can be cleared once list is built).
     */
    std::map<std::string,int> word2Frequency;

public:
    explicit WordFrequencyList();
    WordFrequencyList(const WordFrequencyList&) = delete;
    WordFrequencyList(const WordFrequencyList&&) = delete;
    WordFrequencyList &operator=(const WordFrequencyList&) = delete;
    WordFrequencyList &operator=(const WordFrequencyList&&) = delete;
    ~WordFrequencyList();

    /**
     * @brief Sort word list and clear auxiliary map - nothing can be added after finalize().
     */
    void finalize() {}

    /**
     * @brief Sort word list.
     */
    void sort() {}

    int& operator[](const std::string& key) { return word2Frequency[key]; }
    size_t size() { return word2Frequency.size(); }
    const std::map<std::string,int> iterable() { return word2Frequency; }
};

}
#endif // M8R_WORD_FREQUENCY_LIST_H
