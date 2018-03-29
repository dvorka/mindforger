/*
 ai.h     MindForger thinking notebook

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
#ifndef M8R_AI_H
#define M8R_AI_H

#include <vector>

#include "../../model/outline.h"
#include "../memory.h"
#include "./nlp/markdown_tokenizer.h"
#include "./nlp/note_char_provider.h"
#include "./nlp/bag_of_words.h"
#include "./association_assessment_notes_feature.h"

namespace m8r {

/**
 * @brief Mind's AI.
 */
class Ai
{
private:
    /**
     * @brief Word classifiers
     */
    enum WClassifier {
        POSITIVE = 1,
        NEGATIVE = 1<<1
    };

private:
    Memory& memory;

    Lexicon lexicon;
    BagOfWords bow;
    MarkdownTokenizer tokenizer;

    /*
     * Data sets
     */

    // TODO ... take from Memory

    // Os - vector index is used as ID through other data structures like similarity matrices
    std::vector<Outline*> outlines;
    // Ns - vector index is used as ID through other data structures
    std::vector<Note*> notes;

    /*
     * NN models
     */

    // TODO NN: AssociationAssessmentModel

    /*
     * Classification
     */

    /* IMPROVE:
    // long[Os] - O attributes: no description, no Ns, ...
    long* oClassification;
    // long[Os][Os] - mutual Os relationships: similarity (%), ...
    long* oHive;

    // long[Ns] - N attributes: no description, ...
    long* nClassification;
    // long[Ns][Ns] - mutual Ns relationships: similarity (%), ...
    long* nHive;

    // long[Ws] - W attributes: ..., Standford GloVe word vectors (https://nlp.stanford.edu/projects/glove/)
    long* wClassification;
    // long[Ws][Ws] - mutual Ws relationships: synonym, antonym, positive/negative, ...
    long* wHive;
    */

public:
    explicit Ai(Memory& memory);
    Ai(const Ai&) = delete;
    Ai(const Ai&&) = delete;
    Ai &operator=(const Ai&) = delete;
    Ai &operator=(const Ai&&) = delete;
    ~Ai();

    /**
     * @brief Learn what's in memory to get ready for thinking.
     */
    void learnMemory();

private:
    float calculateSimilarityByWords(Note* n1, Note* n2);

    AssociationAssessmentNotesFeature* createAaFeature(Note* n1, Note* n2);

    /**
     * @brief Train associations assessment neural network once memory is learnt.
     */
    void trainAaNn();

    // NN methods
    // TODO assessAssocByTags(N1,N2)
    // TODO assessAssocByDescriptions()
    // TODO assessAssocByTitles()
};

}
#endif // M8R_AI_H
