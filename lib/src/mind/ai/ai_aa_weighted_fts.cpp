/*
 ai_aa_weighted_fts.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "ai_aa_weighted_fts.h"

namespace m8r {

using namespace std;

AiAaWeightedFts::AiAaWeightedFts(Memory& memory, Mind& mind)
    : mind(mind),
      memory(memory),
      commonWords{}
{
    lastMindDeleteWatermark = mind.getDeleteWatermark();
}

AiAaWeightedFts::~AiAaWeightedFts()
{
}

void AiAaWeightedFts::refreshNotes(bool checkWatermark)
{
#ifdef DO_MF_DEBUG
    MF_DEBUG("AA.FTS Ns refresh - check watermark " << boolalpha << checkWatermark << endl);
    auto begin = chrono::high_resolution_clock::now();
#endif

    if(checkWatermark && lastMindDeleteWatermark==mind.getDeleteWatermark()) {
        return;
    }
    lastMindDeleteWatermark = mind.getDeleteWatermark();
    notes.clear();
    memory.getAllNotes(notes);

#ifdef DO_MF_DEBUG
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG("AA.FTS Ns refreshed in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms" << endl);
#endif

}

shared_future<bool> AiAaWeightedFts::dream()
{
    MF_DEBUG("AA.FTS: LEARNING memory..." << endl);

    refreshNotes(false);
    mind.persistMindState(Configuration::MindState::THINKING);

    std::promise<bool> p{};
    p.set_value(true);
    return shared_future<bool>(std::move(p.get_future()));
}

/*
 * WORDS -> Ns
 */

bool weightedMatchesComparator(const std::pair<Note*,float>& p1, const std::pair<Note*,float>& p2)
{
    return p1.second > p2.second;
}

void AiAaWeightedFts::tokenizeAndStripString(string s, const bool ignoreCase, vector<string>& words)
{
    size_t pos = 0;
    string r{};
    vector<string> tokens{};

    // tokenize
    while((pos = s.find(" ")) != string::npos) {
        tokens.push_back(s.substr(0, pos));
        s.erase(0, pos + 1); // delimiter length ~ 1
    }
    if(s.size()) {
        tokens.push_back(s);
    }

    // strip
    for(string token:tokens) {
        r.clear();
        if(ignoreCase) {
            stringToLower(token, r);
        } else {
            r.assign(token);
        }
        if(r.size()>1 && !commonWords.findWord(r)) {
            MF_DEBUG("AA.FTS.fallback   stripping word: '" << r << "'" << endl);
            StringCharProvider cp{r};
            r = MarkdownTokenizer::stripNonAlpha(cp);
            if(r.size()) {
                MF_DEBUG("AA.FTS.fallback   adding word: '" << r << "'" << endl);
                words.push_back(r);
            }
        }
    }
}

vector<pair<Note*,float>>* AiAaWeightedFts::assessNotesWithFallback(const string& regexp, Outline* scope, const Note* self)
{                         
    vector<pair<Note*,float>>* result = new vector<pair<Note*,float>>();
    if(regexp.empty()) return result;

    // case is *always* ignored to get more matches (more matches vs. precision trade-off)
    const bool ignoreCase = true;
    vector<string> words{};
    string r{};

    // case (in)sensitivity
    if(ignoreCase) {
        stringToLower(regexp, r);
    } else {
        r += regexp;
    }
    r = MarkdownTokenizer::stripFrontBackNonAlpha(r);
    if(r.size()) words.push_back(r);

    // exact match
    if(scope) {
        assessNotesInOutline(scope, result, words, ignoreCase);
    } else {
        const vector<m8r::Outline*> outlines = memory.getOutlines();
        for(Outline* outline:outlines) {
            assessNotesInOutline(outline, result, words, ignoreCase);
        }
    }
    // remove self in case that result can become empty
    if(self && result->size() == 1 && result->begin()->first == self) {
        result->clear();
    }

    // FALLBACK: if exact match failed, split regexp to words (if it's multi-word) and try FTS assessment word by word
    // IMPROVE this may take longer than single search > implement ASYNC run w/ distributor based refresh
    if(result->empty()) {
        MF_DEBUG("AA.FTS.fallback for '" << regexp << "'" << endl);
        words.clear();
        tokenizeAndStripString(regexp, ignoreCase, words);

        // search using words
        MF_DEBUG("AA.FTS.fallback words: " << words.size() << endl);
        if(words.size()) {
            // IMPROVE: iterate 3 *most valuable* words (now the first 3 words are considered, value is ignored)
            words.resize(FTS_SEARCH_THRESHOLD_MULTIWORD);
            // search using words
            if(scope) {
                assessNotesInOutline(scope, result, words, ignoreCase);
            } else {
                const vector<m8r::Outline*> outlines = memory.getOutlines();
                for(Outline* outline:outlines) {
                    assessNotesInOutline(outline, result, words, ignoreCase);
                }
            }
        }
    }

    // sort to have the best match in head
    if(result->size()) {
        std::sort(result->begin(), result->end(), weightedMatchesComparator);
    }

    return result;
}

void AiAaWeightedFts::assessNotesInOutline(Outline* outline, vector<pair<Note*,float>>* result, vector<string>& regexps, const bool ignoreCase)
{
    // IMPROVE make this faster - do NOT convert to lower case, but compare it in that method > will do less
    if(ignoreCase) {
        // case INSENSITIVE

        // O matches
        float oScore = 0.f;
        string s{};
        // O.title matches
        stringToLower(outline->getName(), s);
        for(auto& regexp:regexps) {
            if(s.find(regexp)!=string::npos) {
                oScore += 100.f;
            }
        }
        // O.description matches
        float matches = 0.f;
        for(string* d:outline->getDescription()) {
            if(d) {
                s.clear();
                stringToLower(*d, s);
                for(auto& regexp:regexps) {
                    // find all matches (regexp matched more than once)
                    size_t m = s.find(regexp, 0);
                    while(m != string::npos) {
                        matches++;
                        m = s.find(regexp,m+1);
                    }
                }
            }
        }
        if(matches != 0.f) {
            oScore += 10.f*matches;
            result->push_back(std::make_pair(outline->getOutlineDescriptorAsNote(),oScore));
        }

        // O's score will contribute to N's score as a bonus > normalize it
        //MF_DEBUG(" AA.FTS O>N '" << outline->getName() << "' ~ " << oScore << endl);
        oScore /= 10.f;

        // O's N matches
        float nScore = 0.f;
        for(Note* note:outline->getNotes()) {
            nScore = oScore;
            // time scope @ AI
            if(mind.getScopeAspect().isOutOfScope(note)) {
                continue;
            }
            // N.title matches
            s.clear();
            stringToLower(note->getName(), s);
            for(auto& regexp:regexps) {
                if(s.find(regexp)!=string::npos) {
                    nScore += 100.f;
                }
            }
            // N.description matches
            float matches=0.;
            for(string* d:note->getDescription()) {
                if(d) {
                    s.clear();
                    stringToLower(*d, s);
                    for(auto& regexp:regexps) {
                        // find them all
                        size_t m = s.find(regexp, 0);
                        while(m != string::npos) {
                            matches++;
                            m = s.find(regexp,m+1);
                        }
                    }
                }
            }
            if(nScore!=0.f || matches!=0.f) {
                nScore += 10.f*matches;
                result->push_back(std::make_pair(note,nScore));
                //MF_DEBUG(" AA.FTS > N '" << note->getName() << "' ~ " << nScore << endl);
            }
        }
    } // IMPROVE case sensitive version is NOT needed - was removed - see FTS search
}

std::shared_future<bool> AiAaWeightedFts::getAssociatedNotes(
    const std::string& words,
    std::vector<std::pair<Note*,float>>& associations,
    Outline* self)
{
    // IMPROVE do O specific associations (instead of simple fallback to N associations)
    return getAssociatedNotes(words, associations, self->getOutlineDescriptorAsNote());
}

std::shared_future<bool> AiAaWeightedFts::getAssociatedNotes(
        const std::string& words,
        std::vector<std::pair<Note*,float>>& associations,
        const Note* self)
{
#ifdef DO_MF_DEBUG
    MF_DEBUG("AA.FTS.words for  '" << words << "'" << endl);
    auto begin = chrono::high_resolution_clock::now();
#endif

    // Ns mut be refreshed from Mind to consider O/N deletes and scope changes
    refreshNotes(true);

    // find matches
    vector<pair<Note*,float>>* m = assessNotesWithFallback(words, nullptr, self);
    unique_ptr<vector<pair<Note*,float>>> mKiller{m}; // auto delete

    // calculate leaderboard
    if(m->size()>0) {
        MF_DEBUG("AA.FTS.words '" << words << "' w/ " << m->size() << " matches" << endl);

        // build leaderboard
        std::vector<pair<Note*,float>>::iterator it;
        for(it = m->begin(); it != m->end(); ++it) {
            if(self && self==it->first) {
                continue;
            }
            associations.push_back(*it);
            if(associations.size() >= AA_LEADERBOARD_SIZE) {
                break;
            }
        }
        // recalculate % (and debug)
        MF_DEBUG("Leaderboard of '" << words << "' word(s)[" << associations.size() << "]:" << endl);
        if(associations.size()) {
            float pc = associations[0].second / 100.f;
#ifdef DO_MF_DEBUG
            int i=0;
#endif
            for(auto& p:associations) {
                p.second = p.second/pc/100; // <0,1>
                MF_DEBUG("  #" << ++i << " " << p.first->getName() << " (" << p.first->getOutline()->getName() << ")" << " ~ " << p.second << endl);
            }
        } else {
            // there are no associations (there was ONLY self which was filtered out)
            std::promise<bool> p{};
            p.set_value(false);
            return std::shared_future<bool>(p.get_future());
        }

        // no need to CACHE as FTS matching is fast (~100ms on 1.000s Ns repos)

#ifdef DO_MF_DEBUG
        auto end = chrono::high_resolution_clock::now();
        MF_DEBUG("AA.FTS.words in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms" << endl);
#endif
        std::promise<bool> p{};
        p.set_value(true);
        return std::shared_future<bool>(p.get_future());
    } else {
        // there are no associations
        std::promise<bool> p{};
        p.set_value(false);
        return std::shared_future<bool>(p.get_future());
    }
}

} // m8r namespace
