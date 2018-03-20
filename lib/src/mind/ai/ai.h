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
    /*
     * Data sets
     */

    // Os - vector index is used as ID through other data structures
    std::vector<Outline*> outlines;
    // Ns - vector index is used as ID through other data structures
    std::vector<Note*> notes;

    // Ws - vector index of all words across Os/Ns names and descriptions (a, the, ... filtered out)
    std::vector<std::string> words;

    /*
     * Classification
     */

    // long[Os] - O attributes: no description, no Ns, ...
    long* oClassification;
    // long[Os][Os] - mutual Os relationships: similarity, ...
    long* oHive;

    // long[Ns] - N attributes: no description, ...
    long* nClassification;
    // long[Ns][Ns] - mutual Ns relationships: similarity, ...
    long* nHive;

    // long[Ws] - W attributes: ...
    long* wClassification;
    // long[Ws][Ws] - mutual Ws relationships: synonym, antonym, positive/negative, ...
    long* wHive;

public:
    explicit Ai();
    Ai(const Ai&) = delete;
    Ai(const Ai&&) = delete;
    Ai &operator=(const Ai&) = delete;
    Ai &operator=(const Ai&&) = delete;
    ~Ai();

    /**
     * @brief Get Outlines tuples [O1,O2] by similarity
     *
     * From two most similar to less similar ([O,O] identity filtered out).
     */
    void getOutlinesBySimilarity() {}

};

}
#endif // M8R_AI_H
