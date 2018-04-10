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
    return getAssociatedNotes(note->getName(), associations);
}

/*
 * WORDS -> Ns
 */

set<pair<Note*,float>,AiAaWeightedFts::WeightedMatchesComparator>* AiAaWeightedFts::findAndWeightNote(const string& regexp, const bool ignoreCase, Outline* scope)
{
    // IMPROVE do NOT search associations for common words and <2 words

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

/* Matches through N's O (title and body) and its title and body are counted and weighted.
 *
 * Matches are stored to set w/ comparator ensuring the result will be ordered by score.
 * Caller just trims sorted results to the size of leaderboard (iterate set).
 */
void AiAaWeightedFts::findAndWeightNote(set<pair<Note*,float>,WeightedMatchesComparator>* result, const string& regexp, const bool ignoreCase, Outline* outline)
{
    float score{};

    // IMPROVE make this faster - do NOT convert to lower case, but compare it in that method > will do less
    if(ignoreCase) {
        // case INSENSITIVE
        string s{};
        stringToLower(outline->getName(), s);
        if(s.find(regexp)!=string::npos) {
            score += 100.;
            result->insert(std::make_pair(outline->getOutlineDescriptorAsNote(),score));
        } else {
            for(string* d:outline->getDescription()) {
                if(d) {
                    s.clear();
                    stringToLower(*d, s);
                    if(s.find(regexp)!=string::npos) {
                        score += 1.; // title did NOT matched, try description
                        result->insert(std::make_pair(outline->getOutlineDescriptorAsNote(),score));
                        // IMPROVE do NOT break and calculate HOW MANY times was matched (do NOT add to result more than once)
                        break;
                    }
                }
            }
        }
        for(Note* note:outline->getNotes()) {
            // time scope @ AI
            if(mind.isTimeScopeEnabled() && mind.getTimeScopeAspect().isOutOfScope(note)) {
                continue;
            }
            s.clear();
            stringToLower(note->getName(), s);
            if(s.find(regexp)!=string::npos) {
                score += 10.; // N's O contributed to it's score
                result->insert(std::make_pair(note,score));
            } else {
                for(string* d:note->getDescription()) {
                    if(d) {
                        s.clear();
                        stringToLower(*d, s);
                        if(s.find(regexp)!=string::npos) {
                            score += 1.; // N's O contributed to it's score
                            result->insert(std::make_pair(note,score));
                            // IMPROVE do NOT break and calculate HOW MANY times was matched (do NOT add to result more than once)
                            break;
                        }
                    }
                }
            }
        }
    } else {
        // case SENSITIVE
        if(outline->getName().find(regexp)!=string::npos) {
            score += 100.;
            result->insert(std::make_pair(outline->getOutlineDescriptorAsNote(),score));
        } else {
            for(string* d:outline->getDescription()) {
                if(d && d->find(regexp)!=string::npos) {
                    score += 1.;
                    result->insert(std::make_pair(outline->getOutlineDescriptorAsNote(),score));
                    // IMPROVE do NOT break and calculate HOW MANY times was matched (do NOT add to result more than once)
                    break;
                }
            }
        }
        for(Note* note:outline->getNotes()) {
            if(note->getName().find(regexp)!=string::npos) {
                score += 10.; // N's O contributed to it's score
                result->insert(std::make_pair(note,score));
            } else {
                for(string* d:note->getDescription()) {
                    if(d && d->find(regexp)!=string::npos) {
                        score += 1.; // N's O contributed to it's score
                        result->insert(std::make_pair(note,score));
                        // IMPROVE do NOT break and calculate HOW MANY times was matched (do NOT add to result more than once)
                        break;
                    }
                }
            }
        }
    }
}

std::shared_future<bool> AiAaWeightedFts::getAssociatedNotes(const std::string& words, std::vector<std::pair<Note*,float>>& associations)
{
    set<pair<Note*,float>,WeightedMatchesComparator>* m = findAndWeightNote(words, true, nullptr);
    unique_ptr<set<pair<Note*,float>,WeightedMatchesComparator>> mKiller{m}; // auto delete

    // calculate leaderboard
    vector<pair<Note*,float>> leaderboard{};
    if(m->size()>0) {
#ifdef DO_M8F_DEBUG
    MF_DEBUG("AA.FTS.words for  '" << words << "'" << endl);
    auto begin = chrono::high_resolution_clock::now();
#endif

    // TODO build and cache leaderboard



//        MF_DEBUG("Leaderboard of " << n->getName() << " (" << n->getOutline()->getName() << "):" << endl);
//        for(int i=0; i<AA_LEADERBOARD_SIZE && aaLeaderboard[i][0]!=AA_NOT_SET; i++) {
//            MF_DEBUG("  #" << i << " " <<
//                     notes[aaLeaderboard[i][0]]->getName() << " (" << notes[aaLeaderboard[i][0]]->getOutline()->getName() << ")" <<
//                     " ~ " << aaMatrix[aaLeaderboard[i][0]][aaLeaderboard[i][1]] << endl);
//            leaderboard.push_back(std::make_pair(notes[aaLeaderboard[i][0]],aaMatrix[aaLeaderboard[i][0]][aaLeaderboard[i][1]]));
//        }
//        // cache leaderboard (copied)
//        leaderboardCache[n] = leaderboard;

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
