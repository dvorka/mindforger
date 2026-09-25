/*
 ai_aa_bm25.h     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_AI_ASSOCIATIONS_ASSESSMENT_BM25_H
#define M8R_AI_ASSOCIATIONS_ASSESSMENT_BM25_H

#include <future>
#include <vector>

#include "ai_aa.h"
#include "../mind.h"
#include "./nlp/common_words_blacklist.h"
#include "./nlp/markdown_tokenizer.h"

namespace m8r {

class Mind;
class Memory;

/**
 * @brief BM25 based associations assessment.
 *
 * Description:
 * - The query (O/N title or word(s) under cursor) is split to at most MAX_QUERY_TERMS
 *   terms. All O/N titles and descriptions are scanned ONCE to find whole word matches
 *   of the terms and to collect term/tag frequencies in the same pass.
 * - Matching Os/Ns are ranked using BM25 like score: rare terms weigh more than common
 *   ones (IDF), title hits weigh more than description hits and the occurrences in
 *   the description are saturated and normalized by its length (long Ns don't win).
 * - O/N associations (not word associations) are boosted by tags shared w/ self.
 * - N match is slightly boosted if its O matches as well.
 * - IMPROVE this class is designed to run SYNCHRONOUSLY - for ASYNC modus operandi Mind/AI/this class
 *   cooperation and synchronization protocols must be architected.
 */
class AiAaBm25 : public AiAssociationsAssessment
{
public:
    // query is split to terms - the longest terms are used as they tend to be the most specific
    static constexpr size_t MAX_QUERY_TERMS = 3;
    static constexpr size_t MIN_TERM_LENGTH = 2;

    // BM25 description saturation (K1) and description length normalization (B)
    static constexpr float BM25_K1 = 1.2f;
    static constexpr float BM25_B = 0.75f;

    // score weights
    static constexpr float W_TITLE = 3.0f;
    static constexpr float W_DESCRIPTION = 1.0f;
    static constexpr float W_ALL_TERMS_IN_TITLE = 1.0f;
    static constexpr float W_TAG = 1.0f;
    static constexpr float W_O_CONTEXT = 0.1f;

private:
    /**
     * @brief O/N which matches the query.
     */
    struct Candidate {
        Note* note;
        // title and description hits per query term
        std::vector<unsigned> titleHits;
        std::vector<unsigned> descriptionHits;
        size_t descriptionLength;
        // indices of self's tags which are shared by the O/N
        std::vector<size_t> sharedTags;
        // index of O descriptor candidate (N's O) or -1
        int outlineCandidate;
    };

    Mind& mind;
    Memory& memory;
    CommonWordsBlacklist commonWords;

public:
    explicit AiAaBm25(Memory& memory, Mind& mind);
    AiAaBm25(const AiAaBm25&) = delete;
    AiAaBm25(const AiAaBm25&&) = delete;
    AiAaBm25 &operator=(const AiAaBm25&) = delete;
    AiAaBm25 &operator=(const AiAaBm25&&) = delete;
    virtual ~AiAaBm25();

    virtual std::shared_future<bool> dream();

    virtual std::shared_future<bool> getAssociatedNotes(const Note* note, std::vector<std::pair<Note*,float>>& associations) {
        return associate(note->getName(), associations, note, true);
    }

    virtual std::shared_future<bool> getAssociatedNotes(Outline* outline, std::vector<std::pair<Note*,float>>& associations) {
        return associate(outline->getName(), associations, outline->getOutlineDescriptorAsNote(), true);
    }

    virtual std::shared_future<bool> getAssociatedNotes(const std::string& words, std::vector<std::pair<Note*,float>>& associations, const Note* self) {
        return associate(words, associations, self, false);
    }

    virtual bool sleep() {
        return true;
    }

    virtual bool amnesia() {
        return sleep();
    }

    /**
     * @brief Split the query to at most MAX_QUERY_TERMS unique lower case terms.
     *
     * Common words and words shorter than MIN_TERM_LENGTH are skipped, the longest
     * words are preferred. If no word passes, then the longest word is used.
     */
    static void selectQueryTerms(
        const std::string& query,
        const CommonWordsBlacklist& commonWords,
        std::vector<std::string>& terms);

private:
    std::shared_future<bool> associate(
        const std::string& query,
        std::vector<std::pair<Note*,float>>& associations,
        const Note* self,
        bool useTags);

    /**
     * @brief Scan Os/Ns, rank those which match terms/tags and store them to result.
     */
    void assessNotes(
        const std::vector<std::string>& terms,
        const Note* self,
        bool useTags,
        std::vector<std::pair<Note*,float>>& result);

    /**
     * @brief Collect term hits and shared tags of O/N (candidate is valid if it matches).
     */
    bool matchCandidate(
        const std::string& title,
        const std::vector<std::string*>& description,
        const std::vector<const Tag*>* tags,
        const std::vector<std::string>& terms,
        const std::vector<const Tag*>* selfTags,
        Candidate& candidate);

    /**
     * @brief BM25 like score of the candidate title and description.
     */
    static float textScore(const Candidate& c, const std::vector<float>& idfs, float avgDescriptionLength);
};

}
#endif // M8R_AI_AA_BM25_H
