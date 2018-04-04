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
#include <future>
#include <mutex>

#include "../../debug.h"
#include "../../model/outline.h"
#include "../memory.h"
#include "./nlp/markdown_tokenizer.h"
#include "./nlp/note_char_provider.h"
#include "./nlp/bag_of_words.h"
#include "./association_assessment_notes_feature.h"
#include "./association_assessment_model.h"

namespace m8r {

/**
 * @brief Mind's AI.
 *
 * IMPORTANT: AI cannot be called directory - its methods to be always invoked
 *            by Mind. Method visibility was not restricted in order to ensure
 *            AI testability.
 *
 * AI lifecycle:
 *
 *  SLEEPING            ... initial AI state
 *     |
 *   amnesia(), sleep()
 *     |
 *     V
 *  SLEEPING
 *     |
 *   learnMemory()
 *     |
 *     V
 *  DREAMING
 *     |                ... automatically switches to THINKING once Memory is learned
 *     V
 *  THINKING
 *     |
 *   amnesia(), sleep() ... if no computation is in progress
 *     |
 *     V
 *  SLEEPING
 *
 */
class Ai
{
private:
    static constexpr int THREAD_POOL_SIZE = 2; // IMPROVE thread pool size to be either configured OR #CPU detected and 1/2 or 1/3 used
    static constexpr float AA_NOT_SET = -1.;
    static constexpr int AA_WORD_RELEVANCY_THRESHOLD = 10; // use 10 words w/ highest weight from vectors (and ignore others - irrelevant can bring noice with volume)
    static constexpr int AA_LEADERBOARD_SIZE = 10;
    static constexpr int AA_TITLE_WORD_BONUS = 0.2;

private:
    Configuration::MindState aiState;
    std::mutex exclusiveAi;
    std::vector<std::future<bool>> activeComputations;

    Memory& memory;

    Lexicon lexicon; // IMPROVE merge Standford GloVe word vectors (https://nlp.stanford.edu/projects/glove/)
    Trie wordBlacklist;
    BagOfWords bow;
    MarkdownTokenizer tokenizer;

    /*
     * Data sets
     */

    // Os - vector index is used as ID through other data structures like similarity matrices
    std::vector<Outline*> outlines; // IMPROVE make O* pair where .second is O embedding w/ classifications/attributes
    // Ns - vector index is used as ID through other data structures
    std::vector<Note*> notes; // IMPROVE make N* pair where .second is N embedding w/ classifications/attributes

    /*
     * Associations
     */

    // Associations assessment matrix w/ rankings for any N1/N2 tuple (diagonal symmetry).
    // Changed from float** to vector<vector>> to enable row/col extension on new N/O
    // (keep & avoid realloc)
    std::vector<std::vector<float>> aaMatrix; // IMPROVE: notesAA and outlinesAA ~ Notes assocications assessment

    // leaderboard cache to avoid re-calculation
    std::map<const Note*,std::vector<std::pair<Note*,float>>> leaderboardCache;

    /*
     * Neural network models
     */

    AssociationAssessmentModel* aaModel;

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
    bool isThinking() { return aiState==Configuration::MindState::THINKING; }

    /**
     * @brief Learn what's in memory to get ready for thinking.
     *
     * LONG running method (huge repositories).
     */
    std::future<bool> learnMemory();

    /**
     * @brief Clear.
     *
     * Clear, but don't deallocate.
     */
    bool sleep() {
        std::lock_guard<std::mutex> criticalSection{exclusiveAi};
        return sleepCode();
    }

    /**
     * @brief Forget everything.
     */
    bool amnesia() {
        std::lock_guard<std::mutex> criticalSection{exclusiveAi};
        if(sleepCode()) {
            aaMatrix.clear();
        } else {
            return false;
        }
    }

    /**
     * @brief Get AA leaderboard for N ~ get best Note associations.
     * @return TRUE if leaderboard was created or is on the way (async computation in progress),
     *         FALSE if AI not initialized.
     *
     * This method calculates leaderboard - it uses cache and/or calculates AA cross.
     *
     * LONG running method (if result not cached).
     */
    std::future<bool> getAssociationsLeaderboard(const Note* n, std::vector<std::pair<Note*,float>>& assocLeaderboard);

private:

    /*
     * Tasks
     */

    std::packaged_task<bool> learnMemoryTask{learnMemoryCode};

private:
    /**
     * @brief Initialize blacklist using common words.
     *
     * Lexicon of common words is manually filtered first 200 words to 80 from:
     *   https://github.com/first20hours/google-10000-english
     */
    void initializeWordBlacklist();

    bool learnMemoryCode();


    bool sleepCode() {
        if(activeComputations.empty()) {
            lexicon.clear();
            leaderboardCache.clear();
            notes.clear();
            outlines.clear();
            bow.clear();

            aiState = Configuration::MindState::SLEEPING;
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief Precalculate entire AA.
     *
     * LONG running method.
     */
    void precalculateAa();

    /**
     * @brief Calculate AA row/column cross i.e. associations of N with *all* other Ns.
     *
     * LONG running method on bigger repositories.
     */
    void calculateAaRow(size_t y);

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

    /**
     * @brief Get AA leaderboard from cache.
     */
    bool getCachedAssociationsLeaderboard(const Note* n, std::vector<std::pair<Note*,float>>& leaderboard);
    /**
     * @brief (Re)calculate AA leaderboard.
     *
     * LONG running method.
     */
    void cacheAssociationsLeaderboard(const Note* n);

    /**
     * @brief Create NN input feature.
     */
    AssociationAssessmentNotesFeature* createAaFeature(Note* n1, Note* n2);

    /**
     * @brief Train associations assessment neural network once memory is learned.
     */
    void trainAaNn();

    /**
     * @brief Check AA matrix symmetry.
     */
    void assertAaSymmetry();

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
