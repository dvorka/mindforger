/*
 ai_aa_weighted_fts.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
      memory(memory)
{
}

AiAaWeightedFts::~AiAaWeightedFts()
{
}

std::shared_future<bool> AiAaWeightedFts::dream()
{
    MF_DEBUG("AA.FTS: LEARNING memory..." << endl);

    // IMPROVE consider keeping Ns and flushing them only if memory was modified (Mem's watermark as indicator, writes to persist reads don't count)
    notes.clear();
    memory.getAllNotes(notes);

    mind.persistMindState(Configuration::MindState::THINKING);

    std::promise<bool> p{};
    p.set_value(true);
    return shared_future<bool>(std::move(p.get_future()));
}

/*
 * N -> Ns
 */

std::shared_future<bool> AiAaWeightedFts::getAssociatedNotes(const Note* note, std::vector<std::pair<Note*,float>>& associations)
{
    // the first OVERsimplified implementation: call WORDs version for N's title
    // IMPROVE consider tags, relationships, ... like in BoW to make it more sophisticated
    return getAssociatedNotes(note->getName(), associations, note);
}

/*
 * WORDS -> Ns
 */

set<pair<Note*,float>,AiAaWeightedFts::WeightedMatchesComparator>* AiAaWeightedFts::findAndWeightNoteExactMatch(
        const string& regexp,
        const bool ignoreCase,
        Outline* scope)
{
    set<pair<Note*,float>,WeightedMatchesComparator>* result = new set<pair<Note*,float>,WeightedMatchesComparator>();

    string r{};
    if(ignoreCase) {
        stringToLower(regexp, r);
    } else {
        r.append(regexp);
    }

    if(scope) {
        findAndWeightNote(result, r, ignoreCase, scope);
    } else {
        const vector<m8r::Outline*> outlines = memory.getOutlines();
        for(Outline* outline:outlines) {
            findAndWeightNote(result, r, ignoreCase, outline);
        }
    }

    return result;
}

set<pair<Note*,float>,AiAaWeightedFts::WeightedMatchesComparator>* AiAaWeightedFts::findAndWeightNote(
        const string& regexp,
        const bool ignoreCase,
        Outline* scope,
        const Note* self)
{
    // IMPROVE if whole search gives 0 associations, then check whether it's multi-word and do word-by-word search and combine scores
    // IMPROVE do NOT search associations for common words and <2 words

    set<pair<Note*,float>,WeightedMatchesComparator>* result
        = findAndWeightNoteExactMatch(regexp, ignoreCase, scope);

    // remove self in case that result can become empty
    if(result->size() == 1 && result->begin()->first == self) {
        result->clear();
    }
    // IMPROVE this may take longer than single search > implement ASYNC run w/ distributor based refresh
    if(result->empty()) {
        // whole regexp wasn't found - if it's multi-word try it word by word and combine result

        // IMPROVE make this faster
        vector<string> words{};
        size_t pos = 0;
        string s{regexp}, token{};
        while((pos = s.find(" ")) != string::npos) {
            token = s.substr(0, pos);
            words.push_back(token);
            s.erase(0, pos + 1); // delimiter length ~ 1
        }

        // IMPROVE for now it takes the first non-empty result for a word - improve it as described below
        if(words.size()) {
            set<pair<Note*,float>,WeightedMatchesComparator>* r;
            // TO BE FINISHED: iterate ALL words, track Note* in result, add & combine scores, ...
            // TODO de-duplication to be done using hashset (or auxiliary set w/ Note* as entry to indicate membership)
            for(string& w:words) {
                r = findAndWeightNoteExactMatch(w, ignoreCase, scope);
                if(r->size()) {
                    delete result;
                    return r;
                }
                // TODO to be finished
            }
            // TODO DELETE old result
        }
    }

    return result;
}

/* Find Ns w/ name and/or description matching (regexp) string - counted and weighted
 * matches give score (+ extra bonuses like for N's O having match as well).
 *
 * Matches are stored to set w/ comparator ensuring the result will be ordered by score.
 * Caller just trims sorted results to the size of leaderboard (iterate set).
 */
void AiAaWeightedFts::findAndWeightNote(
    set<pair<Note*,float>,WeightedMatchesComparator>* result,
    const string& regexp,
    const bool ignoreCase,
    Outline* outline)
{
    // IMPROVE make this faster - do NOT convert to lower case, but compare it in that method > will do less
    if(ignoreCase) {
        // case INSENSITIVE

        // O matches
        float oScore = 0;
        string s{};
        // O.title matches
        stringToLower(outline->getName(), s);
        if(s.find(regexp)!=string::npos) {
            oScore += 100.;
        }
        // O.description matches
        float matches = 0.;
        for(string* d:outline->getDescription()) {
            if(d) {
                s.clear();
                stringToLower(*d, s);
                // find them all
                size_t m = s.find(regexp, 0);
                while(m != string::npos) {
                    matches++;
                    m = s.find(regexp,m+1);
                }
            }
        }
        if(matches) {
            oScore += 10.*matches;
            result->insert(std::make_pair(outline->getOutlineDescriptorAsNote(),oScore));
        }

        // O's score will contribute to N's score as a bonus > normalize it
        //MF_DEBUG(" AA.FTS '" << regexp << "' O>N '" << outline->getName() << "' ~ " << oScore << endl);
        oScore /= 10.;

        // O's N matches
        float nScore = 0;
        for(Note* note:outline->getNotes()) {
            nScore = oScore;
            // time scope @ AI
            if(mind.isTimeScopeEnabled() && mind.getTimeScopeAspect().isOutOfScope(note)) {
                continue;
            }
            // N.title matches
            s.clear();
            stringToLower(note->getName(), s);
            if(s.find(regexp)!=string::npos) {
                nScore += 100.;
            }
            // N.description matches
            float matches=0.;
            for(string* d:note->getDescription()) {
                if(d) {
                    s.clear();
                    stringToLower(*d, s);
                    // find them all
                    size_t m = s.find(regexp, 0);
                    while(m != string::npos) {
                        matches++;
                        m = s.find(regexp,m+1);
                    }
                }
            }
            if(nScore || matches) {
                nScore += 10.*matches;
                result->insert(std::make_pair(note,nScore));
                //MF_DEBUG(" AA.FTS '" << regexp << "' > N '" << note->getName() << "' ~ " << nScore << endl);
            }
        }
    } else {

        // TODO to be REWRITTEN to match ALL occurences in the description (already done for case insensitive above)

        // case SENSITIVE
        float oScore{};
        if(outline->getName().find(regexp)!=string::npos) {
            oScore += 100.;
            result->insert(std::make_pair(outline->getOutlineDescriptorAsNote(),oScore));
        } else {
            for(string* d:outline->getDescription()) {
                if(d && d->find(regexp)!=string::npos) {
                    oScore += 1.;
                    result->insert(std::make_pair(outline->getOutlineDescriptorAsNote(),oScore));
                    // IMPROVE do NOT break and calculate HOW MANY times was matched (do NOT add to result more than once)
                    break;
                }
            }
        }

        // O's score will contribute to N's score as a bonus > normalize it
        oScore /= 10.;

        float nScore{};
        for(Note* note:outline->getNotes()) {
            nScore = oScore;
            if(note->getName().find(regexp)!=string::npos) {
                nScore += 100.; // N's O contributed to it's score
                result->insert(std::make_pair(note,nScore));
            } else {
                for(string* d:note->getDescription()) {
                    if(d && d->find(regexp)!=string::npos) {
                        nScore += 10.; // N's O contributed to it's score
                        result->insert(std::make_pair(note,nScore));
                        // IMPROVE do NOT break and calculate HOW MANY times was matched (do NOT add to result more than once)
                        break;
                    }
                }
            }
        }
    }
}

std::shared_future<bool> AiAaWeightedFts::getAssociatedNotes(
        const std::string& words,
        std::vector<std::pair<Note*,float>>& associations,
        const Note* self)
{
#ifdef DO_M8F_DEBUG
    MF_DEBUG("AA.FTS.words for  '" << words << "'" << endl);
    auto begin = chrono::high_resolution_clock::now();
#endif

    // find matches
    set<pair<Note*,float>,WeightedMatchesComparator>* m = findAndWeightNote(words, true, nullptr, self);
    unique_ptr<set<pair<Note*,float>,WeightedMatchesComparator>> mKiller{m}; // auto delete

    // calculate leaderboard
    if(m->size()>0) {
        MF_DEBUG("AA.FTS.words '" << words << "' w/ " << m->size() << " matches" << endl);

        // build leaderboard
        std::set<pair<Note*,float>,WeightedMatchesComparator>::iterator it;
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
            float pc = associations[0].second / 100.;
            int i=0;
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

#ifdef DO_M8F_DEBUG
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
