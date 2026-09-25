/*
 ai_aa_bm25.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "ai_aa_bm25.h"

#include <algorithm>
#include <cmath>

namespace m8r {

using namespace std;

// C++11 requires out of class definitions of static constexpr members which are ODR-used
constexpr size_t AiAaBm25::MAX_QUERY_TERMS;
constexpr size_t AiAaBm25::MIN_TERM_LENGTH;
constexpr float AiAaBm25::BM25_K1;
constexpr float AiAaBm25::BM25_B;
constexpr float AiAaBm25::W_TITLE;
constexpr float AiAaBm25::W_DESCRIPTION;
constexpr float AiAaBm25::W_ALL_TERMS_IN_TITLE;
constexpr float AiAaBm25::W_TAG;
constexpr float AiAaBm25::W_O_CONTEXT;

AiAaBm25::AiAaBm25(Memory& memory, Mind& mind)
    : mind(mind),
      memory(memory),
      commonWords{}
{
}

AiAaBm25::~AiAaBm25()
{
}

shared_future<bool> AiAaBm25::dream()
{
    // nothing to learn - Os/Ns are scanned on every query to reflect O/N changes, deletes and scope
    MF_DEBUG("AA.BM25: LEARNING memory..." << endl);

    mind.persistMindState(Configuration::MindState::THINKING);

    std::promise<bool> p{};
    p.set_value(true);
    return shared_future<bool>(std::move(p.get_future()));
}

/*
 * WORDS -> Ns
 */

// sort by score and then by name to make the order of equally scored Ns deterministic
static bool bm25MatchesComparator(const std::pair<Note*,float>& p1, const std::pair<Note*,float>& p2)
{
    if(p1.second != p2.second) {
        return p1.second > p2.second;
    }
    return p1.first->getName() < p2.first->getName();
}

void AiAaBm25::selectQueryTerms(
    const string& query,
    const CommonWordsBlacklist& commonWords,
    vector<string>& terms)
{
    vector<string> words{};
    stringToLowerWords(query, words);

    for(const string& word:words) {
        if(word.size() >= MIN_TERM_LENGTH
             && !commonWords.findWord(word)
             && std::find(terms.begin(), terms.end(), word) == terms.end())
        {
            terms.push_back(word);
        }
    }

    // query w/ common/short words only (like "The" or "C") - use the longest word
    if(terms.empty() && words.size()) {
        terms.push_back(
            *std::max_element(
                words.begin(),
                words.end(),
                [](const string& w1, const string& w2) { return w1.size() < w2.size(); }));
    }

    // longer words tend to be more specific ~ keep the longest ones (stable ~ query order for ties)
    std::stable_sort(
        terms.begin(),
        terms.end(),
        [](const string& w1, const string& w2) { return w1.size() > w2.size(); });
    if(terms.size() > MAX_QUERY_TERMS) {
        terms.resize(MAX_QUERY_TERMS);
    }
}

bool AiAaBm25::matchCandidate(
    const string& title,
    const vector<string*>& description,
    const vector<const Tag*>* tags,
    const vector<string>& terms,
    const vector<const Tag*>* selfTags,
    Candidate& candidate)
{
    candidate.titleHits.assign(terms.size(), 0);
    candidate.descriptionHits.assign(terms.size(), 0);
    candidate.descriptionLength = 0;
    candidate.sharedTags.clear();

    size_t matches = countWordsIgnoreCase(title, terms, candidate.titleHits);
    for(const string* d:description) {
        if(d) {
            candidate.descriptionLength += d->size();
            matches += countWordsIgnoreCase(*d, terms, candidate.descriptionHits);
        }
    }

    if(selfTags && tags) {
        for(size_t i=0; i<selfTags->size(); ++i) {
            if(std::find(tags->begin(), tags->end(), selfTags->at(i)) != tags->end()) {
                candidate.sharedTags.push_back(i);
            }
        }
    }

    return matches || candidate.sharedTags.size();
}

float AiAaBm25::textScore(const Candidate& c, const vector<float>& idfs, float avgDescriptionLength)
{
    // BM25: saturate description occurrences and normalize them by description length
    const float lengthNorm = avgDescriptionLength > 0.f
        ? 1.f - BM25_B + BM25_B * c.descriptionLength/avgDescriptionLength
        : 1.f;

    float score = 0.f;
    for(size_t t=0; t<idfs.size(); ++t) {
        float termScore = 0.f;
        if(c.titleHits[t]) {
            termScore += W_TITLE;
        }
        if(c.descriptionHits[t]) {
            const float tf = static_cast<float>(c.descriptionHits[t]);
            termScore += W_DESCRIPTION * tf * (BM25_K1 + 1.f) / (tf + BM25_K1 * lengthNorm);
        }
        score += idfs[t] * termScore;
    }
    return score;
}

void AiAaBm25::assessNotes(
    const vector<string>& terms,
    const Note* self,
    bool useTags,
    vector<pair<Note*,float>>& result)
{
    const vector<const Tag*>* selfTags
        = useTags && self && self->getTags()->size() ? self->getTags() : nullptr;
    if(terms.empty() && !selfTags) {
        return;
    }

    /*
     * scan: collect matching Os/Ns and frequencies of terms and tags in ONE pass
     */

    vector<Candidate> candidates{};
    vector<unsigned> termFrequencies(terms.size(), 0);
    vector<unsigned> tagFrequencies(selfTags ? selfTags->size() : 0, 0);
    size_t documents = 0;
    size_t descriptionsLength = 0;

    Candidate c{};
    auto account = [&](const Candidate& a) {
        ++documents;
        descriptionsLength += a.descriptionLength;
        for(size_t t=0; t<terms.size(); ++t) {
            if(a.titleHits[t] || a.descriptionHits[t]) {
                ++termFrequencies[t];
            }
        }
        for(size_t i:a.sharedTags) {
            ++tagFrequencies[i];
        }
    };

    // IMPORTANT: Mind::getOutlines() is NOT used as it rebuilds shared static vector (not thread safe)
    for(Outline* outline:memory.getOutlines()) {
        // mind scope @ AI - out of scope Os/Ns must not affect terms/tags frequencies
        if(mind.getScopeAspect().isOutOfScope(outline)) {
            continue;
        }

        // O
        int outlineCandidate = -1;
        bool matches = matchCandidate(
            outline->getName(), outline->getDescription(), outline->getTags(), terms, selfTags, c);
        account(c);
        const bool isSelf
            = self && self->getOutline()==outline && outline->getOutlineDescriptorAsNote()==self;
        if(matches && !isSelf) {
            c.note = outline->getOutlineDescriptorAsNote();
            c.outlineCandidate = -1;
            outlineCandidate = static_cast<int>(candidates.size());
            candidates.push_back(c);
        }

        // O's Ns
        for(Note* note:outline->getNotes()) {
            // time scope @ AI
            if(mind.getScopeAspect().isOutOfScope(note)) {
                continue;
            }
            matches = matchCandidate(
                note->getName(), note->getDescription(), note->getTags(), terms, selfTags, c);
            account(c);
            if(matches && note!=self) {
                c.note = note;
                c.outlineCandidate = outlineCandidate;
                candidates.push_back(c);
            }
        }
    }

    if(candidates.empty()) {
        return;
    }

    /*
     * rank: rare terms and tags weigh more (IDF)
     */

    const float n = static_cast<float>(documents);
    vector<float> termIdfs(terms.size());
    float termIdfsSum = 0.f;
    for(size_t t=0; t<terms.size(); ++t) {
        termIdfs[t] = std::log(1.f + (n - termFrequencies[t] + 0.5f) / (termFrequencies[t] + 0.5f));
        termIdfsSum += termIdfs[t];
    }
    vector<float> tagIdfs(tagFrequencies.size());
    for(size_t i=0; i<tagFrequencies.size(); ++i) {
        tagIdfs[i] = std::log(1.f + n / std::max(tagFrequencies[i], 1u));
    }
    const float avgDescriptionLength = static_cast<float>(descriptionsLength) / n;

    vector<float> textScores(candidates.size());
    for(size_t i=0; i<candidates.size(); ++i) {
        textScores[i] = textScore(candidates[i], termIdfs, avgDescriptionLength);
    }

    for(size_t i=0; i<candidates.size(); ++i) {
        const Candidate& candidate = candidates[i];
        float score = textScores[i];

        // multi-word query: bonus if title contains all the terms
        if(terms.size() > 1
             && std::find(candidate.titleHits.begin(), candidate.titleHits.end(), 0u) == candidate.titleHits.end())
        {
            score += W_ALL_TERMS_IN_TITLE * termIdfsSum;
        }
        // shared tags
        for(size_t t:candidate.sharedTags) {
            score += W_TAG * tagIdfs[t];
        }
        // N which matches is boosted by its O match (but O match alone doesn't make N associated)
        if(textScores[i] > 0.f && candidate.outlineCandidate >= 0) {
            score += W_O_CONTEXT * textScores[candidate.outlineCandidate];
        }

        if(score > 0.f) {
            result.push_back(std::make_pair(candidate.note, score));
        }
    }
}

std::shared_future<bool> AiAaBm25::associate(
        const std::string& query,
        std::vector<std::pair<Note*,float>>& associations,
        const Note* self,
        bool useTags)
{
#ifdef DO_MF_DEBUG
    MF_DEBUG("AA.BM25.words for  '" << query << "'" << endl);
    auto begin = chrono::high_resolution_clock::now();
#endif

    // find matches
    vector<string> terms{};
    selectQueryTerms(query, commonWords, terms);
    vector<pair<Note*,float>> matches{};
    assessNotes(terms, self, useTags, matches);

    if(matches.empty()) {
        // there are no associations
        std::promise<bool> p{};
        p.set_value(false);
        return std::shared_future<bool>(p.get_future());
    }

    // build leaderboard - self is NOT among matches
    MF_DEBUG("AA.BM25.words '" << query << "' w/ " << matches.size() << " matches" << endl);
    const size_t leaderboardSize = std::min(matches.size(), static_cast<size_t>(AA_LEADERBOARD_SIZE));
    std::partial_sort(
        matches.begin(), matches.begin()+leaderboardSize, matches.end(), bm25MatchesComparator);
    associations.insert(associations.end(), matches.begin(), matches.begin()+leaderboardSize);

    // recalculate % (and debug)
    MF_DEBUG("Leaderboard of '" << query << "' word(s)[" << associations.size() << "]:" << endl);
    const float best = associations[0].second;
#ifdef DO_MF_DEBUG
    int i=0;
#endif
    for(auto& p:associations) {
        p.second = p.second/best; // <0,1>
        MF_DEBUG("  #" << ++i << " " << p.first->getName() << " (" << p.first->getOutline()->getName() << ")" << " ~ " << p.second << endl);
    }

    // no need to CACHE as FTS matching is fast

#ifdef DO_MF_DEBUG
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG("AA.BM25.words in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms" << endl);
#endif
    std::promise<bool> p{};
    p.set_value(true);
    return std::shared_future<bool>(p.get_future());
}

} // m8r namespace
