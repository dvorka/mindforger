/*
 ai_aa.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_AI_ASSOCIATIONS_ASSESSMENT_H
#define M8R_AI_ASSOCIATIONS_ASSESSMENT_H

#include <future>
#include <vector>

#include "../../model/outline.h"

namespace m8r {

/**
 * @brief Asssociations assessment interface.
 *
 * Interface enables implementation to perform calculations asychronously (future).
 * Obviously it can be performed synchronously (if it's fast enough) and just encapsulated
 * to promise/future.
 */
class AiAssociationsAssessment
{
protected:
    static constexpr int AA_LEADERBOARD_SIZE = 10;

public:
    explicit AiAssociationsAssessment() {}
    AiAssociationsAssessment(const AiAssociationsAssessment&) = delete;
    AiAssociationsAssessment(const AiAssociationsAssessment&&) = delete;
    AiAssociationsAssessment &operator=(const AiAssociationsAssessment&) = delete;
    AiAssociationsAssessment &operator=(const AiAssociationsAssessment&&) = delete;
    virtual ~AiAssociationsAssessment() {}

    /**
     * @brief Learn what's in memory to get ready for thinking.
     *
     * Can be LONG running (asynchronous execution handled by AI).
     */
    virtual std::shared_future<bool> dream() = 0;

    /**
     * @brief Get associated Ns for N.
     * @return return value explanation:
     *   - future !VALID ... associated Ns are on the way - wait for future to become valid
     *                       and then call this method AGAIN (i.e. ASYNC doesn't store associations
     *                       to provided vector to avoid race conditions in the future).
     *   - future VALID
     *     > true  ... associated Ns can be found in vector
     *     > false ... associated Ns will NOT be computed - Mind's not thinking, memory empty, ...
     *
     * Can be LONG running (asynchronous execution handled by AI).
     */
    virtual std::shared_future<bool> getAssociatedNotes(const Note* note, std::vector<std::pair<Note*,float>>& associations) = 0;
    virtual std::shared_future<bool> getAssociatedNotes(Outline* outline, std::vector<std::pair<Note*,float>>& associations) = 0;

    /**
     * @brief Get associated Ns for a set of words.
     */
    virtual std::shared_future<bool> getAssociatedNotes(const std::string& words, std::vector<std::pair<Note*,float>>& associations, const Note* self) = 0;

    /**
     * @brief Clear.
     */
    virtual bool sleep() = 0;

    /**
     * @brief Forget everything.
     */
    virtual bool amnesia() = 0;
};

}
#endif // M8R_AI_ASSOCIATIONS_ASSESSMENT_H
