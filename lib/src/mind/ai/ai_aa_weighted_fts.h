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
#include "./nlp/markdown_tokenizer.h"

namespace m8r {

class Mind;
class Memory;

/**
 * @brief Weighted FTS based associations assessment.
 *
 * Description:
 * - This method has own FTS implementation to compute weights and leverage O/N relationships
 *   while searching the best result.
 * - IMPROVE this class is designed to run SYNCHRONOUSLY - for ASYNC modus operandi Mind/AI/this class
 *   cooperation and synchronization protocols must be architected.
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

    // IMPROVE in addition to watermark also scope change should be tracked ~ mind.scopeWatermark
    int lastMindDeleteWatermark;

public:
    explicit AiAaWeightedFts(Memory& memory, Mind& mind);
    AiAaWeightedFts(const AiAaWeightedFts&) = delete;
    AiAaWeightedFts(const AiAaWeightedFts&&) = delete;
    AiAaWeightedFts &operator=(const AiAaWeightedFts&) = delete;
    AiAaWeightedFts &operator=(const AiAaWeightedFts&&) = delete;
    virtual ~AiAaWeightedFts();

    virtual std::shared_future<bool> dream();

    // This is initial OVERsimplified implementation - calls WORDs version for N's title.
    virtual std::shared_future<bool> getAssociatedNotes(const Note* note, std::vector<std::pair<Note*,float>>& associations) {
        // IMPROVE consider tags, relationships, ... like in BoW to make it more sophisticated
        return getAssociatedNotes(note->getName(), associations, note);
    }

    // This is initial OVERsimplified implementation - calls WORDs version for O's title.
    virtual std::shared_future<bool> getAssociatedNotes(Outline* outline, std::vector<std::pair<Note*,float>>& associations) {
        // IMPROVE consider tags, relationships, ... like in BoW to make it more sophisticated
        return getAssociatedNotes(outline->getName(), associations, outline);
    }

    virtual std::shared_future<bool> getAssociatedNotes(const std::string& words, std::vector<std::pair<Note*,float>>& associations, const Note* self);

    virtual bool sleep() {
        notes.clear();
        return true;
    }

    virtual bool amnesia() {
        return sleep();
    }

private:
    void refreshNotes(bool checkWatermark);
    void tokenizeAndStripString(std::string s, const bool ignoreCase, std::vector<std::string>& words);

    std::shared_future<bool> getAssociatedNotes(const std::string& words, std::vector<std::pair<Note*,float>>& associations, Outline* self);

    // getAssociatedNotes(){assessNs,leaderboard}
    //   -> assessNsWithFallback(){2lowercase,iterateOs,fallback}
    //     -> assessNs@O()
    std::vector<std::pair<Note*,float>>* assessNotesWithFallback(const std::string& regexp, Outline* scope, const Note* self);
    void assessNotesInOutline(Outline* outline, std::vector<std::pair<Note*,float>>* result, std::vector<std::string>& regexp, const bool ignoreCase);
};

}
#endif // M8R_AI_AA_WEIGHTED_FTS_H
