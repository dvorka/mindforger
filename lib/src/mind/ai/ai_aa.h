/*
 ai_aa.h     MindForger thinking notebook

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
public:
    explicit AiAssociationsAssessment() {}
    AiAssociationsAssessment(const AiAssociationsAssessment&) = delete;
    AiAssociationsAssessment(const AiAssociationsAssessment&&) = delete;
    AiAssociationsAssessment &operator=(const AiAssociationsAssessment&) = delete;
    AiAssociationsAssessment &operator=(const AiAssociationsAssessment&&) = delete;
    ~AiAssociationsAssessment() {}

    /**
     * @brief Learn what's in memory to get ready for thinking.
     *
     * Can be LONG running (asynchronous execution handled by AI).
     */
    virtual std::future<bool> dream() = 0;

    /**
     * @brief Get AA leaderboard for N ~ get best Note associations.
     * @return *copy* of the leaderboard (valid even if AI sleep()/amnesia() invoked)
     *
     * Can be LONG running (asynchronous execution handled by AI).
     */
    virtual std::future<std::vector<std::pair<Note*,float>>> calculateLeaderboard(const Note* n) = 0;

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
