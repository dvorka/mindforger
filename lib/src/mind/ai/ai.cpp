/*
 ai.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "ai.h"

namespace m8r {

using namespace std;

Ai::Ai(Memory& memory)
    :memory(memory),
     lexicon{},
     tokenizer{lexicon}
{
    // TODO AA NN
}

Ai::~Ai()
{
}

void Ai::trainAaNn()
{
    MF_DEBUG("START: training AA NN..." << endl);
    // TODO train NN using a reasonable number of features: createAaFeature()
    MF_DEBUG("FINISH: AA NN trained" << endl);
}

void Ai::learnMemory()
{
    memory.getAllNotes(notes);

    // build lexicon and BoW
    lexicon.clear();
    bow.clear();
    for(Note* n:notes) {
        NoteCharProvider chars{n};
        WordFrequencyList* wfl = new WordFrequencyList{&lexicon};
        tokenizer.tokenize(chars, *wfl);

        bow.add(n, wfl);
    }

    // prepare DATA to quickly create association assessment features
    lexicon.recalculateWeights();
#ifdef DO_M8F_DEBUG
    lexicon.print();
#endif
    bow.reorderDocVectorsByWeight();
#ifdef DO_M8F_DEBUG
    bow.print();
#endif

    // train NN
    trainAaNn();
}

float Ai::calculateSimilarityByWords(Note* n1, Note* n2)
{
    // TODO
    // formula: ... 10() ...
}

AssociationAssessmentNotesFeature* Ai::createAaFeature(Note* n1, Note* n2)
{
    AssociationAssessmentNotesFeature* result
        = new AssociationAssessmentNotesFeature{};

    // TODO replace foo value
    result->setHaveMutualRel(false);
    result->setTypeMatches(n1->getType()==n2->getType());
    // TODO replace foo value
    result->setSimilarityByTags(0.0);
    // TODO replace foo value
    result->setSimilarityByWords(0.0);

    return result;
}

} // m8r namespace
