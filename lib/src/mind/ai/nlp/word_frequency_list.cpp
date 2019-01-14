/*
 word_frequency_list.cpp     MindForger thinking notebook

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
#include "word_frequency_list.h"

#include <algorithm>

namespace m8r {

using namespace std;

WordFrequencyList::WordFrequencyList(Lexicon* lexicon)
    : lexicon(lexicon),
      wordWeightComparator{lexicon}
{
    weight = UNDEF_WEIGHT;
}

WordFrequencyList::~WordFrequencyList()
{
}

void WordFrequencyList::sort() {
    // reset array
    wordsByWeight.clear();

    for(auto& w:word2Frequency) {
        wordsByWeight.push_back(&w);
    }
    std::sort(wordsByWeight.begin(),wordsByWeight.end(),wordWeightComparator);
}

float WordFrequencyList::recalculateWeight() {
    weight = 0;
    for(auto& w:word2Frequency) {
        Lexicon::WordEmbedding* e = lexicon->get(w.first);
        // IMPROVE if(e) result += e->weight * ((float)w.second); ... means min of weights in UNION and INTERSECTION
        if(e) weight += e->weight;
    }
    return weight;
}

} // m8r namespace
