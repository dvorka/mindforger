/*
 ai_aa_bow.cpp     MindForger thinking notebook

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
#include "ai_aa_bow.h"

namespace m8r {

using namespace std;

AiAaBoW::AiAaBoW(Memory& memory, Mind& mind)
    : mind(mind),
      memory(memory),
      lexicon{},
      wordBlacklist{},
      tokenizer{lexicon,wordBlacklist}
{
}

AiAaBoW::~AiAaBoW()
{
    // delete dead threads
    auto zombieIterator
        = std::remove_if(runningWorkers.begin(),
                         runningWorkers.end(),
                         [](const thread* t) { if(!t->joinable()) { delete t; return true; } else return false; });
    runningWorkers.erase(zombieIterator, runningWorkers.end());
}

void AiAaBoW::addWorkerAndCleanZombies(thread* t)
{
    lock_guard<mutex> criticalSection{runningWorkersMutex};
    MF_DEBUG("AA.BoW: workers+zombies " << runningWorkers.size() << endl);

    // IMPROVE: if zombies is erased > future/promise is deleted as well > frontend that has future calls a method on it > crash
    //   - temporal SOLUTION: threads are deleted on MF exit (i.e. finished threads are kept in memory for whole MF run)
    //     (see destructor for dead threads code to move in here)
    //   - IDEA: frontend could use a thread hook to indicate that it got future and t can be freed

    // add new one
    runningWorkers.push_back(t);

    MF_DEBUG("AA.BoW: workers " << runningWorkers.size() << endl);
}

// it's presumed that caller ensures the correct Mind state & synchronization
shared_future<bool> AiAaBoW::dream() {
    if(memory.getNotesCount() > Configuration::getInstance().getAsyncMindThreshold()) {
        MF_DEBUG("AA.BoW: ASYNC dream..." << endl);
        mind.incActiveProcesses();

        // packaged task is parametrized by function/method signature (not return type)
        std::packaged_task<bool (AiAaBoW*,thread*)> learnMemoryTask([](AiAaBoW* a,thread* t) { return a->learnMemorySync(t); });
        future<bool> result = learnMemoryTask.get_future(); // move
        thread* t = new thread{};
        *t = thread(std::move(learnMemoryTask), this, t);
        addWorkerAndCleanZombies(t);

        return shared_future<bool>(std::move(result));
    } else {
        MF_DEBUG("AA.BoW: SYNC dream..." << endl);
        promise<bool> p{};
        bool status = learnMemorySync();
        p.set_value(status);

        mind.persistMindState(Configuration::MindState::THINKING);

        return shared_future<bool>(p.get_future());
    }
}

bool AiAaBoW::learnMemorySync(thread* t)
{
    MF_DEBUG("AA.BoW: LEARNING memory to BoW..." << endl);
    notes.clear();
    memory.getAllNotes(notes);
    // let N know it's indexed in AI
    for(size_t i=0; i<notes.size(); i++) {
        notes[i]->setAiAaMatrixIndex(static_cast<int>(i));
    }

    // build lexicon and BoW
    lexicon.clear();
    bow.clear();
    for(Note* n:notes) {
        NoteCharProvider chars{n};
        WordFrequencyList* wfl = new WordFrequencyList{&lexicon};
        tokenizer.tokenize(chars, *wfl);
        bow.add(n, wfl);
    }
    // prepare DATA to quickly create association assessment features
    lexicon.recalculateWeights();
    bow.reorderDocVectorsByWeight();

#ifdef DO_MF_DEBUG
    lexicon.print();
    bow.print();
#endif

    // AA to be built incrementally - just initialize it
    if(notes.size() != aaMatrix.size()) {
        aaMatrix.clear();
        aaMatrix.resize(notes.size());
        for(size_t i=0; i<aaMatrix.size(); ++i) {
            aaMatrix[i].resize(aaMatrix.size(),(float)AiAaBoW::AA_NOT_SET); // C++ :-Z constexpr w/ internal linkage does NOT have to be solved in compile time > workaround via temporary var
        }
    } else {
        for(size_t i=0; i<aaMatrix.size(); ++i) {
            std::fill(aaMatrix[i].begin(), aaMatrix[i].end(), (float)AiAaBoW::AA_NOT_SET); // C++ :-Z constexpr w/ internal linkage does NOT have to be solved in compile time > workaround via temporary var
        }
    }

    // NN to be trained on demand - just initialize it

    mind.persistMindState(Configuration::MindState::THINKING);
    mind.decActiveProcesses();
    if(t) t->detach(); // indicate that thread finished

    MF_DEBUG("AA.BoW: memory LEARNED!" << endl);
    return true;
}

// it's presumed that caller ensures the correct Mind state & synchronization
shared_future<bool> AiAaBoW::getAssociatedNotes(const Note* note, vector<pair<Note*,float>>& associations) {
    auto cachedLeaderboard = leaderboardCache.find(note);
    if(cachedLeaderboard != leaderboardCache.end()) {
        MF_DEBUG("AA.BoW: SYNC leaderboard calculation for '" << note->getName() << "'" << endl);
        // copy leaderboard to ENSURE it's validity even if Mind/AI will be cleared/asleep/...
        for(auto p:cachedLeaderboard->second) {
            associations.push_back(p);
        }
        // indicate that it's immediately available
        promise<bool> p{};
        p.set_value(true);
        return shared_future<bool>(p.get_future());
    } else {        
        MF_DEBUG("AA.BoW: ASYNC leaderboard calculation for '" << note->getName() << "'" << endl);
        if(leaderboardWip.find(note) != leaderboardWip.end()) {
            // calculation WIP & future OWNER will update what needs to be updated -> intentionally NOT sharing futures
            promise<bool> p{};
            p.set_value(false);
            MF_DEBUG("AA.BoW: leaderboard WIP for '" << note->getName() << "'" << endl);
            return p.get_future(); // move
        } else {
            mind.incActiveProcesses();
            MF_DEBUG("AA.BoW: starting THREAD for '" << note->getName() << "'" << endl);

            // packaged task is parametrized by function/method signature (not return type)
            packaged_task<bool (AiAaBoW*,const Note*,thread*)> calculateLeaderboardTask([](AiAaBoW* a, const Note* n, thread* t) { return a->calculateLeaderboardSync(n,t); });
            future<bool> result = calculateLeaderboardTask.get_future(); // move
            thread* t = new thread{};
            *t = thread(std::move(calculateLeaderboardTask), this, note, t); // run task w/ handle to self thread
            addWorkerAndCleanZombies(t);

            return shared_future<bool>(std::move(result));
        }
    }
}

// Pre-calculate/calculate code CANNOT be reused as pre-calculate relies on rows w/ lower index
// to fill the beginning of the line.
// This is a private method called from AI ~ AI state/async/critical sections handled by caller.
void AiAaBoW::calculateAaRow(size_t y)
{
    MF_DEBUG("AA.BoW: Calculating AA row " << y << "..." << endl);
    // calculate row and column that cross diagonal on [y][y]

    // check diagonal to find out whether the cross has been already calculated
    if(aaMatrix[y][y] == 1.f) {
        return;
    }

    float aa;
    AssociationAssessmentNotesFeature aaFeature{};

    notes[y]->setAiAaMatrixIndex(y);
    for(size_t x=0; x<aaMatrix.size(); x++) {
        // set diagonal at the end
        if(x!=y) {
            // skip if value has been already calculated
            if(aaMatrix[y][x] == AA_NOT_SET) {
                Note* n1 = notes[x];
                Note* n2 = notes[y];

                aaFeature.setHaveMutualRel(false); // TODO
                aaFeature.setTypeMatches(n1->getType()==n2->getType());
                aaFeature.setSimilaritySameOutline(n1->getOutline()==n2->getOutline());
                aaFeature.setSimilarityByTags(calculateSimilarityByTags(n1->getTags(),n2->getTags()));
                aaFeature.setSimilarityByTitles(calculateSimilarityByTitles(n1->getName(),n2->getName()));
                aaFeature.setSimilarityByDescription(calculateSimilarityByWords(*bow.get(n1),*bow.get(n2),AA_WORD_RELEVANCY_THRESHOLD));
                aaFeature.setSimilarityBySameTargetRels(0.0); // TODO nice

                aa = aaFeature.areNotesAssociatedMetric();

                // set AA ranking both below and above diagonal - detection will be faster later (no check x>y needed)
                aaMatrix[x][y] = aa;
                aaMatrix[y][x] = aa;
            }
        }
    }

    // set diagonal at the end to indicate calculation is done (consider reentrancy)
    aaMatrix[y][y] = 1.;

#ifdef DO_MF_DEBUG
    MF_DEBUG("AA.BoW: AA row calculated!" << endl);
    //printAa();
    //assertAaSymmetry();
#endif
}

// This is a private method called from AI ~ AI state/async/critical sections handled by caller.
void AiAaBoW::precalculateAa()
{
#ifdef DO_MF_DEBUG
    static const float UNIQUE_AA_CELLS = (float)(notes.size()*notes.size()/2.+notes.size()/2.);
    MF_DEBUG("  Building AA matrix w/ " << UNIQUE_AA_CELLS << " UNIQUE rankings..." << endl);
    float c=0;
    float p;
#endif

    // calculate FULL matrix of Ns associativity assessment for every N1 and N2 tuple
    float aa;
    AssociationAssessmentNotesFeature aaFeature{};
    for(size_t y=0; y<aaMatrix.size(); y++) {
        notes[y]->setAiAaMatrixIndex(y); // sets index for ALL notes in notes vector

#ifdef DO_MF_DEBUG
        p = c/(UNIQUE_AA_CELLS/100.);
        MF_DEBUG("    " << (int)p << "% AA matrix rankings for '" << notes[y]->getName() << "'" << endl);
#endif

        // calculate only values ABOVE diagonal i.e. initialize x=y
        for(size_t x=y; x<aaMatrix.size(); x++) {
#ifdef DO_MF_DEBUG
            c++;
#endif

            if(x==y) {
                aaMatrix[x][y] = 1.;
            } else {
                Note* n1 = notes[x];
                Note* n2 = notes[y];

                aaFeature.setHaveMutualRel(false); // TODO
                aaFeature.setTypeMatches(n1->getType()==n2->getType());
                aaFeature.setSimilaritySameOutline(n1->getOutline()==n2->getOutline());
                aaFeature.setSimilarityByTags(calculateSimilarityByTags(n1->getTags(),n2->getTags()));
                aaFeature.setSimilarityByTitles(calculateSimilarityByTitles(n1->getName(),n2->getName()));
                aaFeature.setSimilarityByDescription(calculateSimilarityByWords(*bow.get(n1),*bow.get(n2),AA_WORD_RELEVANCY_THRESHOLD));
                aaFeature.setSimilarityBySameTargetRels(0.0); // TODO nice

                aa = aaFeature.areNotesAssociatedMetric();

                // set AA ranking both below and above diagonal - detection will be faster later (no check x>y needed)
                aaMatrix[x][y] = aa;
                aaMatrix[y][x] = aa;
            }
        }
    }

#ifdef DO_MF_DEBUG
    MF_DEBUG("  AA matrix built!" << endl);
    //printAa();
    assertAaSymmetry();
#endif
}

float AiAaBoW::calculateSimilarityByTitles(const string& t1, const string& t2)
{
    StringCharProvider cp1{t1};
    WordFrequencyList v1{&lexicon};
    tokenizer.tokenize(cp1, v1, false, true, false);
    StringCharProvider cp2{t2};
    WordFrequencyList v2{&lexicon};
    tokenizer.tokenize(cp2, v2, false, true, false);

    // calculate overlap
    if(!v1.size() || !v2.size()) {
        return 0.;
    } else {
        // direct access for efficiency
        WordFrequencyList intersection{&lexicon};
        float iWeight=0, uWeight=0;

        for(auto& e:v1.iterable()) {
            uWeight += 1;
            if(v2.contains(e.first)) {
                iWeight += 1;
                intersection.add(e.first);
            }
        }

        for(auto& e:v2.iterable()) {
            if(!intersection.contains(e.first)) {
                uWeight += 1;
                if(v1.contains(e.first)) {
                    iWeight += 1;
                    // no need to update iVector as it won't be needed
                }
            }
        }

        //MF_DEBUG("  titleSimilarity = "<<iWeight<<" / "<<uWeight << endl);
        // intersection % of union
        return (iWeight/(uWeight/100.))/100;
    }
}

// algorithm is based on similarity by words (for now there are no weights - might be added later if needed by other lib functions)
float AiAaBoW::calculateSimilarityByTags(const vector<const Tag*>* t1, const vector<const Tag*>* t2)
{
    if(!t1->size()) {
        if(!t2->size()) {
            return 1.;
        } else {
            return 0.;
        }
    } else {
        // direct access for efficiency
        vector<const Tag*> intersection{};
        float iWeight=0, uWeight=0;

        // iterate at most *threshold* words from v1: all + to UNION, matching + to INTERSECTION
        for(auto& t:*t1) {
            uWeight += 1;
            if(std::find(t2->begin(),t2->end(),t) != t2->end()) {
                iWeight += 1;
                intersection.push_back(t);
            }
        }
        // uWeight contains weight of v1's tags, iWeight weight of v1 intersection v2

        // iterate tags from v2: w in intersection HANDLED both u&i, w in v2&v1 > intersection else union
        for(auto& t:*t2) {
            if(std::find(intersection.begin(),intersection.end(),t) == intersection.end()) {
                uWeight += 1;
                if(std::find(t1->begin(),t1->end(),t) != t1->end()) {
                    iWeight += 1;
                    // no need to update iVector as it won't be needed
                }
            }
        }

        //MF_DEBUG("  tagSimilarity = "<<iWeight<<" / "<<uWeight << endl);
        // intersection % of union
        return (iWeight/(uWeight/100.))/100;
    }
}

// consider ONLY most valuable words via threshold - many irrelevat words would kill the score (irrelevant words make noise)
float AiAaBoW::calculateSimilarityByWords(WordFrequencyList& v1, WordFrequencyList& v2, int threshold)
{
    if(!v1.size() || !v2.size()) {
        return 0.;
    } else {
        // direct access for efficiency
        WordFrequencyList intersection{&lexicon};
        float iWeight=0, uWeight=0;
        int t=0;

        // iterate at most *threshold* words from v1: all + to UNION, matching + to INTERSECTION
        for(auto& e:v1.iterable()) {
            if(t++>=threshold) break;

            float w = lexicon.get(e.first)->weight;
            uWeight += w;
            if(v2.contains(e.first)) {
                iWeight += w;
                intersection.add(e.first);
            }
        }
        // uWeight contains weight of 1st 10 v1's words, iWeight weight of v1 intersection v2

        // iterate at most *threshold* words from v2: w in intersection HANDLED both u&i, w in v2&v1 > intersection else union
        t=0;
        for(auto& e:v2.iterable()) {
            // consider at most threshold words from v2
            if(++t>=threshold) break;

            if(!intersection.contains(e.first)) {
                float w = lexicon.get(e.first)->weight;
                uWeight += w;
                if(v1.contains(e.first)) {
                    iWeight += w;
                    // no need to update iVector as it won't be needed
                }
            }
        }

        // intersection % of union
        float result = (iWeight/(uWeight/100.))/100;
        //MF_DEBUG("  wordSimilarity = "<<iWeight<<" / "<<uWeight <<" (t="<<t<<") -> " << result << endl);
        return result;
    }
}

bool AiAaBoW::calculateLeaderboardSync(const Note* n, thread* t)
{
    MF_DEBUG("AA.BoW: SYNC leaderboard calculation for '" << n->getName() << "' in thread " << t << endl);

    // If N was REMOVED, then nobody will ask for leaderboard.
    // If N was MODIFIED, then leaderboard will not be accurate (but it's not critical).
    // If N was ADDED, then I don't have data - no leaderboard provided.
    if(n->getAiAaMatrixIndex() != AA_NOT_SET) {
        // check cache
        auto cachedLeaderboard = leaderboardCache.find(n);
        if(cachedLeaderboard != leaderboardCache.end()) {
            return true;
        }

        // calculate row/column of AA matrix & build leaderboard
        calculateAaRow(n->getAiAaMatrixIndex());

        int aaLeaderboard[AA_LEADERBOARD_SIZE][2];
        for(int i=0; i<AA_LEADERBOARD_SIZE; i++) {
            aaLeaderboard[i][0] = aaLeaderboard[i][1] = AA_NOT_SET;
        }

        float aa;
        for(size_t x=0, y=n->getAiAaMatrixIndex(); x<notes.size(); x++) {
            if(x==y) continue; // self on diagonal

            aa = aaMatrix[x][y];

            if(aa > aaLeaderboard[AA_LEADERBOARD_SIZE-1][0]) { // covers also lb[][]==NOT_SET (== -1)
                // find target leaderboard row
                size_t target;
                for(target=0; target<AA_LEADERBOARD_SIZE; target++) {
                    if(aaLeaderboard[target][0] == AA_NOT_SET) {
                        break; // fill empty row -> no shift needed
                    } else {
                        if(aa > aaMatrix[aaLeaderboard[target][0]][aaLeaderboard[target][1]]) {
                            break;
                        }
                    }
                }

                /// empty row > no shift needed
                if(aaLeaderboard[target][0] != AA_NOT_SET) {
                    // shift leaderboard
                    int sx = aaLeaderboard[target][0];
                    int sy = aaLeaderboard[target][1];
                    for(size_t ll=target; ll<AA_LEADERBOARD_SIZE; ll++) {
                        if(aaLeaderboard[ll][0]!=AA_NOT_SET) {
                            int tx=aaLeaderboard[ll][0];
                            int ty=aaLeaderboard[ll][1];
                            aaLeaderboard[ll][0]=sx;
                            aaLeaderboard[ll][1]=sy;
                            sx=tx;
                            sy=ty;
                        } else {
                            aaLeaderboard[ll][0]=sx;
                            aaLeaderboard[ll][1]=sy;
                            break;
                        }
                    }
                }

                // assign value
                aaLeaderboard[target][0]=x;
                aaLeaderboard[target][1]=y;
            }
        }

        MF_DEBUG("Leaderboard of " << n->getName() << " (" << n->getOutline()->getName() << "):" << endl);
        vector<pair<Note*,float>> leaderboard{};
        for(int i=0; i<AA_LEADERBOARD_SIZE && aaLeaderboard[i][0]!=AA_NOT_SET; i++) {
            MF_DEBUG("  #" << i << " " <<
                     notes[aaLeaderboard[i][0]]->getName() << " (" << notes[aaLeaderboard[i][0]]->getOutline()->getName() << ")" <<
                     " ~ " << aaMatrix[aaLeaderboard[i][0]][aaLeaderboard[i][1]] << endl);
            leaderboard.push_back(std::make_pair(notes[aaLeaderboard[i][0]],aaMatrix[aaLeaderboard[i][0]][aaLeaderboard[i][1]]));
        }

        // cache leaderboard (copied)
        leaderboardCache[n] = leaderboard;
    }

    leaderboardWip.erase(n);
    mind.decActiveProcesses();
    if(t) t->detach(); // indicate that thread finished
    return true;
}

void AiAaBoW::assertAaSymmetry()
{
    MF_DEBUG("AI: checking AA symmetry..." << endl);
    for(size_t i=0; i<aaMatrix.size(); ++i) {
        for(size_t j=0; i<aaMatrix.size(); ++i) {
            if(aaMatrix[i][j] != aaMatrix[j][i]) {
                MF_DEBUG("  Symmetry ERROR: aa["<<i<<"]["<<j<<"]" << endl);
            }
        }
    }
    MF_DEBUG("AI: AA symmetry checked!" << endl);
}

// it's presumed that caller ensures the correct Mind state & synchronization
bool AiAaBoW::sleep() {
    lexicon.clear();
    notes.clear();
    outlines.clear();
    bow.clear();

    return true;
}

// it's presumed that caller ensures the correct Mind state & synchronization
bool AiAaBoW::amnesia() {
    sleep();
    aaMatrix.clear();

    return true;
}

} // m8r namespace
