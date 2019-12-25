/*
 ai_aa_bow.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_AI_ASSOCIATIONS_ASSESSMENT_BOW_H
#define M8R_AI_ASSOCIATIONS_ASSESSMENT_BOW_H

#include <future>

#include "../mind.h"
#include "ai_aa.h"
#include "./nlp/markdown_tokenizer.h"
#include "./nlp/note_char_provider.h"
#include "./nlp/bag_of_words.h"
#include "./nlp/common_words_blacklist.h"

namespace m8r {

class Mind;

/**
 * @brief BoW based associations assessment.
 */
// IMPROVE rewrite BoW to infer word WEIGHT from it's occurence in O/N title
// (not by frequency in Lexicon). Incorrect word weight causes this method didn't
// work. With correctly weighted words it will work (much better).
class AiAaBoW : public AiAssociationsAssessment
{
private:
    static constexpr int THREAD_POOL_SIZE = 2; // IMPROVE thread pool size to be either configured OR #CPU detected and 1/2 or 1/3 used
    static constexpr float AA_NOT_SET = -1.;
    static constexpr int AA_WORD_RELEVANCY_THRESHOLD = 10; // use 10 words w/ highest weight from vectors (and ignore others - irrelevant can bring noice with volume)
    static constexpr float AA_TITLE_WORD_BONUS = 0.2;

private:
    Mind& mind;
    Memory& memory;

    Lexicon lexicon; // IMPROVE merge Standford GloVe word vectors (https://nlp.stanford.edu/projects/glove/)
    CommonWordsBlacklist wordBlacklist;
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

    // associate Ns as you READ: N -> O/N
    // IMPROVE thing*,float - both O and N to be association
    std::map<const Note*,std::vector<std::pair<Note*,float>>> leaderboardCache;
    std::set<const Note*> leaderboardWip;

    // associate as you WRITE: word(s) -> O/N
    // IMPROVE std::map<const Note*,std::vector<std::pair<string*,float>>> leaderboardCache;

    // Associations assessment matrix w/ rankings for any N1/N2 tuple (diagonal symmetry).
    // Changed from float** to vector<vector>> to enable row/col extension on new N/O
    // (keep & avoid realloc)
    std::vector<std::vector<float>> aaMatrix; // IMPROVE: notesAA and outlinesAA ~ Notes assocications assessment

public:
    explicit AiAaBoW(Memory& memory, Mind& mind);
    AiAaBoW(const AiAaBoW&) = delete;
    AiAaBoW(const AiAaBoW&&) = delete;
    AiAaBoW &operator=(const AiAaBoW&) = delete;
    AiAaBoW &operator=(const AiAaBoW&&) = delete;
    virtual ~AiAaBoW();

    virtual std::shared_future<bool> dream();

    /**
     * @brief Get associated Notes.
     *
     * If future is valid and true, then associated N are copied to vector passed as arg,
     * else waith for future to become valid and then call this method again (i.e. async
     * variant does NOT copies computed associated Ns to provided vector).
     */
    virtual std::shared_future<bool> getAssociatedNotes(const Note* note, std::vector<std::pair<Note*,float>>& associations);

    // TODO to be implemented
    virtual std::shared_future<bool> getAssociatedNotes(Outline* outline, std::vector<std::pair<Note*,float>>& associations) {
        UNUSED_ARG(outline);
        UNUSED_ARG(associations);

        std::promise<bool> p{};
        p.set_value(false);
        return std::shared_future<bool>(p.get_future());
    }

    // TODO to be implemented
    virtual std::shared_future<bool> getAssociatedNotes(const std::string& words, std::vector<std::pair<Note*,float>>& associations, const Note* self) {
        UNUSED_ARG(words);
        UNUSED_ARG(associations);
        UNUSED_ARG(self);

        std::promise<bool> p{};
        p.set_value(false);
        return std::shared_future<bool>(p.get_future());
    }

    virtual bool sleep();

    virtual bool amnesia();

private:

    /*
     * Worker threads:
     *  1. packaged task is assigned to thread
     *  2. thread performs the task and calls detach() on self when finishes
     *     to indicate that it's ready to be deleted
     *  3. if main thread detects that !thread.joinable(), then it can be deleted;
     *     cleanup is done on any new thread launch AND/OR by this class destructor.
     */

    // IMPROVE introduce a LIMIT on number of running threads in configuration
    std::vector<std::thread*> runningWorkers;
    std::mutex runningWorkersMutex;

private:

    /**
     * @brief Learn Memory to start thinking.
     */
    bool learnMemorySync(std::thread* t = nullptr);

    /**
     * @brief Calculate leaderboard and indicate that it has been stored to cache.
     */
    bool calculateLeaderboardSync(const Note* n, std::thread* t = nullptr);

    /**
     * @brief Initialize blacklist using common words.
     *
     * Lexicon of common words is manually filtered first 200 words to 80 from:
     *   https://github.com/first20hours/google-10000-english
     */
    void initializeWordBlacklist();

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
     * @brief Check AA matrix symmetry.
     */
    void assertAaSymmetry();

    /**
     * @brief Get AA leaderboard from cache.
     */
    bool getCachedLeaderboard(const Note* n, std::vector<std::pair<Note*,float>>& leaderboard);

    /**
     * @brief Remove finished workers and add new one.
     */
    void addWorkerAndCleanZombies(std::thread* t);

public:
#ifdef DO_MF_DEBUG
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
#endif
};

}
#endif // M8R_AI_AA_BOW_H
