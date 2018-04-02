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

#include "../../debug.h"
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
public:
    /**
     * @brief Indicates whether any computation is in progress.
     *
     * AI is singletion.
     *
     * IMPROVE this is just a quickfix - obviously WRONG. Must be rewritten to monitors
     * like approach where Mind state change vs. AI computations are critical sections.
     */
    volatile int activeThreads;

private:
    /**
     * @brief Word classifiers
     */
    enum WClassifier {
        POSITIVE = 1,
        NEGATIVE = 1<<1
    };

    static constexpr float AA_NOT_SET = -1.;
    static constexpr int AA_WORD_RELEVANCY_THRESHOLD = 10; // use 10 words w/ highest weight from vectors (and ignore others - irrelevant can bring noice with volume)
    static constexpr int AA_LEADERBOARD_SIZE = 10;

private:
    Memory& memory;

    Lexicon lexicon;
    Trie wordBlacklist;
    BagOfWords bow;
    MarkdownTokenizer tokenizer;

    /*
     * Data sets
     */

    // Os - vector index is used as ID through other data structures like similarity matrices
    std::vector<Outline*> outlines;
    // Ns - vector index is used as ID through other data structures
    std::vector<Note*> notes;

    /*
     * AA leaderboard(s)
     */

    // Association assessment matrix w/ rankings for any N1/N2 tuple (diagonal symmetry).
    // Changed from float** to vector<vector>> to enable row/col extension on new N/O (keep & avoid realloc)
    std::vector<std::vector<float>> aaMatrix;

    // leaderboard cache to avoid re-calculation
    std::map<const Note*,std::vector<std::pair<Note*,float>>> leaderboardCache;

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
     * @brief Is ready to think?
     */
    bool isConcious() { return lexicon.size()>0; }

    bool canSleep() {
        if(activeThreads) {
            std::cerr << "Mind learn CANCELLED becaue AI is thinking - wait until AI finishes and try again" << std::endl;
            return false;
        } else {
            return true;
        }
    }

    /**
     * @brief Learn what's in memory to get ready for thinking.
     */
    void learnMemory();

    /**
     * @brief Clear.
     *
     * Clear, but don't deallocate.
     */
    bool sleep() {
        if(canSleep()) {
            lexicon.clear();
            leaderboardCache.clear();
            notes.clear();
            outlines.clear();
            bow.clear();

            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief Forget everything.
     */
    void amnesia() {
        sleep();
        aaMatrix.clear();
    }

    /**
     * @brief Precalculate entire AA.
     */
    void precalculateAa();

    /**
     * @brief Calculate AA row/column cross.
     */
    void calculateAaRow(size_t y);

    /**
     * @brief Check AA matrix symmetry.
     */
    void assertAaSymmetry();

    /**
     * @brief Find Note associations.
     *
     * This calculates leaderboard on demand (uses cache and/or calculates AA row/column).
     */
    void getAssociationsLeaderboard(const Note* n, std::vector<std::pair<Note*,float>>& assocLeaderboard);
    bool getCachedAssociationsLeaderboard(const Note* n, std::vector<std::pair<Note*,float>>& leaderboard);
    void cacheAssociationsLeaderboard(const Note* n);

private:
    /**
     * @brief Initialize blacklist using common words.
     *
     * Lexicon of common words is manually filtered first 200 words to 80 from:
     *   https://github.com/first20hours/google-10000-english
     */
    void initializeWordBlacklist();

    /**
     * @brief Calculate similarity of two word vectors.
     */
    float calculateSimilarityByWords(WordFrequencyList& v1, WordFrequencyList& v2, int threshold=1000);

    /**
     * @brief Calculate similarity of two tag lists.
     */
    float calculateSimilarityByTags(const std::vector<const Tag*>* t1, const std::vector<const Tag*>* t2);

    /**
     * @brief Calculate similarity of two N/O names.
     */
    float calculateSimilarityByTitles(const std::string& t1, const std::string& t2);

    AssociationAssessmentNotesFeature* createAaFeature(Note* n1, Note* n2);

    /**
     * @brief Train associations assessment neural network once memory is learned.
     */
    void trainAaNn();

public:
#ifdef DO_M8F_DEBUG
    static void print(const Note* n, std::vector<std::pair<Note*,float>>& leaderboard) {
        std::cout << "Note '" << n->getName() << "' AA leaderboard("<< leaderboard.size() <<"):" << std::endl;
        int i=1;
        for(auto& nn:leaderboard) {
            std::cout << "  #" << i++ << " '" << nn.first->getName() << "' ~ " << nn.second << std::endl;
        }
    }
#endif
    void printAa() {
        std::cout << "AA Matrix:" << std::endl;
        for(size_t i=0; i<aaMatrix.size(); i++) {
            std::cout << "AA[" << i << "] = ";
            for(size_t j=0; j<aaMatrix.size(); j++) {
                if(aaMatrix[i][j] == -1) {
                    std::cout << "_ ";
                } else {
                    std::cout << aaMatrix[i][j] << " ";
                }
            }
            std::cout << std::endl;
        }
    }
};

}
#endif // M8R_AI_H
