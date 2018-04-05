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
#include "./aa_notes_feature.h"
#include "./aa_model.h"
#include "./ai_aa_weighted_fts.h"
#include "./ai_aa_bow.h"

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
    Configuration::MindState aiState;

    // asynchronous computations handling
    std::mutex exclusiveAi;

    Memory& memory;

    /*
     * Associations
     */

    // Associations assessment implemenations: AA @ weighted FTS, AA @ BoW
    AiAssociationsAssessment* aa;

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
     * @brief Get AI state.
     */
    Configuration::MindState state() { return aiState; }
    void state(Configuration::MindState state) { aiState = state; }

    /**
     * @brief Learn what's in memory to get ready for thinking.
     *
     * Can be LONG running on huge repositories.
     */
    std::future<bool> learnMemory()
    {
        std::lock_guard<std::mutex> criticalSection{exclusiveAi};
        aa->learnMemory();
    }

    /**
     * @brief Calculate or get from cache AA leaderboard for N ~ get best Note associations.
     * @return future w/ the *copy* of the leaderboard (valid even if AI sleep()/amnesia() invoked)
     *
     * Can be LONG running on big repositories.
     */
    std::future<std::vector<std::pair<Note*,float>>> calculateAssociationsLeaderboard(const Note* n) {
        std::lock_guard<std::mutex> criticalSection{exclusiveAi};
        return aa->calculateLeaderboard(n);
    }

    /**
     * @brief Clear, but don't deallocate.
     */
    bool sleep() {
        std::lock_guard<std::mutex> criticalSection{exclusiveAi};
        return aa->sleep();
    }

    /**
     * @brief Forget everything.
     */
    bool amnesia() {
        std::lock_guard<std::mutex> criticalSection{exclusiveAi};
        return aa->amnesia();
    }

private:

    /**
     * @brief Train associations assessment neural network once memory is learned.
     */
    void trainAaNn();
};

}
#endif // M8R_AI_H
