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
#include <map>

#include "ai_aa.h"
#include "../mind.h"
#include "../../gear/hash_map.h"
#include "./nlp/common_words_blacklist.h"

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
    // in case that FTS for name fails, name is split to words - too many words would take too much time
    static constexpr int FTS_SEARCH_THRESHOLD_MULTIWORD = 3;

private:
    Mind& mind;
    Memory& memory;
    CommonWordsBlacklist commonWords;

    std::vector<Note*> notes;

public:
    explicit AiAaWeightedFts(Memory& memory, Mind& mind);
    AiAaWeightedFts(const AiAaWeightedFts&) = delete;
    AiAaWeightedFts(const AiAaWeightedFts&&) = delete;
    AiAaWeightedFts &operator=(const AiAaWeightedFts&) = delete;
    AiAaWeightedFts &operator=(const AiAaWeightedFts&&) = delete;
    ~AiAaWeightedFts();

    virtual std::shared_future<bool> dream();
    virtual std::shared_future<bool> getAssociatedNotes(const Note* note, std::vector<std::pair<Note*,float>>& associations);
    virtual std::shared_future<bool> getAssociatedNotes(const std::string& words, std::vector<std::pair<Note*,float>>& associations) {
        return getAssociatedNotes(words, associations, nullptr);
    }

    virtual bool sleep() {
        notes.clear();
        return true;
    }

    virtual bool amnesia() {
        return sleep();
    }

private:
    std::vector<std::pair<Note*,float>>* findAndWeightNoteExactMatch(const std::string& regexp, const bool ignoreCase, Outline* scope);
    std::vector<std::pair<Note*,float>>* findAndWeightNote(const std::string& regexp, const bool ignoreCase, Outline* scope, const Note* self);
    void findAndWeightNote(std::vector<std::pair<Note*,float>>* result, const std::string& regexp, const bool ignoreCase, Outline* outline);
    std::shared_future<bool> getAssociatedNotes(const std::string& words, std::vector<std::pair<Note*,float>>& associations, const Note* self);
};

}
#endif // M8R_AI_AA_WEIGHTED_FTS_H
