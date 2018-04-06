/*
 ai_aa_weighted_fts.h     MindForger thinking notebook

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
#ifndef M8R_AI_ASSOCIATIONS_ASSESSMENT_WEIGHTED_FTS_H
#define M8R_AI_ASSOCIATIONS_ASSESSMENT_WEIGHTED_FTS_H

#include <future>
#include <vector>

#include "ai_aa.h"
#include "../mind.h"

namespace m8r {

class Mind;

/**
 * @brief Weighted FTS based associations assessment.
 *
 * Associate as you read ~ Nr associations intuitions:
 *
 * - find O by Nr name words
 * - find N by Nr name words
 * - order by...
 *
 * - Ns may have generic names (Introduction, Resources, User stories, ...) e.g. in
 *   case of GROW or an analysis, but in this case O has the name to associate like Simulator Analysis.
 *
 * Associate as you write ~ word (w/ context) associations intuitions:
 *
 * - find O by name using word
 * - find N by name using word
 * - order by...
 *
 * Closing remarks:
 *
 * - this method has own FTS implementation to compute weights and leverage O/N relationships
 *   while searching the best result
 */
class AiAaWeightedFts : public AiAssociationsAssessment
{
private:
    Mind& mind;
    Memory& memory;

public:
    explicit AiAaWeightedFts(Memory& memory, Mind& mind);
    AiAaWeightedFts(const AiAaWeightedFts&) = delete;
    AiAaWeightedFts(const AiAaWeightedFts&&) = delete;
    AiAaWeightedFts &operator=(const AiAaWeightedFts&) = delete;
    AiAaWeightedFts &operator=(const AiAaWeightedFts&&) = delete;
    ~AiAaWeightedFts();

    virtual std::future<bool> dream() {
        std::promise<bool> p{};
        p.set_value(false);
        return p.get_future();
    }

    virtual std::future<std::vector<std::pair<Note*,float>>> calculateLeaderboard(const Note* n) {
        UNUSED_ARG(n);

                                                           std::promise<std::vector<std::pair<Note*,float>>> p{};
                                                           p.set_value(std::vector<std::pair<Note*,float>>{});
                                                           return p.get_future();
    }

    bool sleep() {
        return false;
    }

    bool amnesia() {
        return false;
    }
};

}
#endif // M8R_AI_AA_WEIGHTED_FTS_H
