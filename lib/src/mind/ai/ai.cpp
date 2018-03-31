/*
 ai.cpp     MindForger thinking notebook

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
#include "ai.h"

namespace m8r {

using namespace std;

Ai::Ai(Memory& memory)
    :memory(memory),
     lexicon{},
     wordBlacklist{},
     tokenizer{lexicon,wordBlacklist}
{
    initializeWordBlacklist();
    aaMatrix=nullptr;
    // TODO AA NN
}

Ai::~Ai()
{
}

void Ai::trainAaNn()
{
    MF_DEBUG("START: training AA NN..." << endl);
    // TODO train NN using a reasonable number of features: createAaFeature()
    MF_DEBUG("FINISH: AA NN trained" << endl);
}

void Ai::learnMemory()
{
    MF_DEBUG("  AI::START: learning memory..." << endl);

    memory.getAllNotes(notes);

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
#ifdef DO_M8F_DEBUG
    lexicon.print();
#endif
    bow.reorderDocVectorsByWeight();
#ifdef DO_M8F_DEBUG
    bow.print();
#endif

    // calculate FULL matrix of Ns associativity assessment for every N1 and N2 tuple
    if(aaMatrix) {
        delete aaMatrix;
    }
    aaMatrix = new float*[notes.size()];
    for(size_t i=0; i<notes.size(); ++i) {
        aaMatrix[i] = new float[notes.size()];
    }

#ifdef DO_M8F_DEBUG
    MF_DEBUG("  Building AA matrix w/ " << (notes.size()*notes.size()/2) << " UNIQUE rankings..." << endl);
    float c=0;
    float p;
#endif

    // TODO vectorize this ~ LAUNCH multiple threads calculating values in parallel (portion of rows from the matrix)
    int WORD_RELEVANCY_THRESHOLD = 10; // use 10 words w/ highest weight from vectors (and ignore others - irrelevant can bring noice with volume)
    float aa;
    AssociationAssessmentNotesFeature aaFeature{};
    for(size_t y=0; y<notes.size(); y++) {
        notes[y]->setAiAaMatrixIndex(y); // sets index for ALL notes in notes vector

        #ifdef DO_M8F_DEBUG
        p = c/((float(notes.size()*notes.size()))/100.);
        MF_DEBUG("    " << (int)p << "% AA matrix rankings for '" << notes[y]->getName() << "'" << endl);
        #endif

        // calculate only values ABOVE diagonal i.e. initialize x=y
        for(size_t x=y; x<notes.size(); x++) {
            #ifdef DO_M8F_DEBUG
            c++;
            #endif

            if(x==y) {
                aaMatrix[x][y] = 1.;
            } else {
                Note* n1 = notes[x];
                Note* n2 = notes[y];

                aaFeature.setHaveMutualRel(false); // TODO
                aaFeature.setTypeMatches(n1->getType()==n2->getType());
                aaFeature.setSimilarityByTags(calculateSimilarityByTags(n1->getTags(),n2->getTags()));
                aaFeature.setSimilarityByTitles(0.0); // TODO
                aaFeature.setSimilarityByDescription(calculateSimilarityByWords(*bow.get(n1),*bow.get(n2),WORD_RELEVANCY_THRESHOLD));
                aaFeature.setSimilarityByTitlesInDescription(0.0); // TODO nice
                aaFeature.setSimilarityBySameTargetRels(0.0); // TODO nice

                // set both values above and below diagonal - detection will be faster later (no check x>y needed)
                aa = aaFeature.areNotesAssociatedMetric();
                //MF_DEBUG("  aa[" << x << "][" << y << "]=" << aa << endl);
                // TODO store features to be used as input to NN later > can be FIXED array size

                // set AA ranking both below and above diagonal
                aaMatrix[x][y] = aa;
                aaMatrix[y][x] = aa;
            }
        }
    }
    MF_DEBUG("  AA matrix built!" << endl);

    // TODO train NN usint aaMatrix - why (incorporate USER data), how much data to use, test set, ...
    //trainAaNn();

    MF_DEBUG("  AI::FINISH: memory learned" << endl);
}

// algorithm is based on similarity by words (for now there are no weights - might be added later if needed by other lib functions)
float Ai::calculateSimilarityByTags(const vector<const Tag*>* t1, const vector<const Tag*>* t2)
{
    if(!t1->size() || !t2->size()) {
        return 0.;
    } else {
        // direct access for efficiency
        vector<const Tag*> intersection{};
        float iWeight=0, uWeight=0;

        // iterate at most *threashold* words from v1: all + to UNION, matching + to INTERSECTION
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

        MF_DEBUG("  tagSimilarity = "<<iWeight<<" / "<<uWeight << endl);

        // intersection % of union
        return (iWeight/uWeight/100.)/100.;
    }
}

// consider ONLY most valuable words via threshold - many irrelevat words would kill the score (irrelevant words make noise)
float Ai::calculateSimilarityByWords(WordFrequencyList& v1, WordFrequencyList& v2, int threshold)
{
    if(!v1.size() || !v2.size()) {
        return 0.;
    } else {
        // direct access for efficiency
        WordFrequencyList intersection{&lexicon};
        float iWeight=0, uWeight=0;
        int t=0;

        // iterate at most *threashold* words from v1: all + to UNION, matching + to INTERSECTION
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

        // iterate at most *threashold* words from v2: w in intersection HANDLED both u&i, w in v2&v1 > intersection else union
        t=0;
        for(auto& e:v2.iterable()) {
            // consider at most threashold words from v2
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

        //MF_DEBUG("  wordSimilarity = "<<iWeight<<" / "<<uWeight <<" (t="<<t<<")" << endl);

        // intersection % of union
        return (iWeight/uWeight/100.)/100.;
    }
}

AssociationAssessmentNotesFeature* Ai::createAaFeature(Note* n1, Note* n2)
{
    AssociationAssessmentNotesFeature* result
        = new AssociationAssessmentNotesFeature{};

    // TODO all features to be set & calculated

    return result;
}

void Ai::initializeWordBlacklist() {
    wordBlacklist.addWord("the");
    wordBlacklist.addWord("of");
    wordBlacklist.addWord("and");
    wordBlacklist.addWord("to");
    wordBlacklist.addWord("a");
    wordBlacklist.addWord("in");
    wordBlacklist.addWord("for");
    wordBlacklist.addWord("is");
    wordBlacklist.addWord("on");
    wordBlacklist.addWord("that");
    wordBlacklist.addWord("by");
    wordBlacklist.addWord("this");
    wordBlacklist.addWord("with");
    wordBlacklist.addWord("I");
    wordBlacklist.addWord("you");
    wordBlacklist.addWord("it");
    wordBlacklist.addWord("not");
    wordBlacklist.addWord("or");
    wordBlacklist.addWord("be");
    wordBlacklist.addWord("are");
    wordBlacklist.addWord("from");
    wordBlacklist.addWord("at");
    wordBlacklist.addWord("as");
    wordBlacklist.addWord("your");
    wordBlacklist.addWord("all");
    wordBlacklist.addWord("have");
    wordBlacklist.addWord("new");
    wordBlacklist.addWord("more");
    wordBlacklist.addWord("an");
    wordBlacklist.addWord("was");
    wordBlacklist.addWord("we");
    wordBlacklist.addWord("will");
    wordBlacklist.addWord("can");
    wordBlacklist.addWord("us");
    wordBlacklist.addWord("about");
    wordBlacklist.addWord("if");
    wordBlacklist.addWord("my");
    wordBlacklist.addWord("has");
    wordBlacklist.addWord("but");
    wordBlacklist.addWord("our");
    wordBlacklist.addWord("one");
    wordBlacklist.addWord("other");
    wordBlacklist.addWord("do");
    wordBlacklist.addWord("no");
    wordBlacklist.addWord("they");
    wordBlacklist.addWord("he");
    wordBlacklist.addWord("may");
    wordBlacklist.addWord("what");
    wordBlacklist.addWord("which");
    wordBlacklist.addWord("their");
    wordBlacklist.addWord("any");
    wordBlacklist.addWord("there");
    wordBlacklist.addWord("so");
    wordBlacklist.addWord("his");
    wordBlacklist.addWord("when");
    wordBlacklist.addWord("who");
    wordBlacklist.addWord("also");
    wordBlacklist.addWord("get");
    wordBlacklist.addWord("am");
    wordBlacklist.addWord("been");
    wordBlacklist.addWord("would");
    wordBlacklist.addWord("how");
    wordBlacklist.addWord("were");
    wordBlacklist.addWord("me");
    wordBlacklist.addWord("some");
    wordBlacklist.addWord("these");
    wordBlacklist.addWord("its");
    wordBlacklist.addWord("like");
    wordBlacklist.addWord("than");
    wordBlacklist.addWord("had");
    wordBlacklist.addWord("should");
    wordBlacklist.addWord("her");
    wordBlacklist.addWord("such");
    wordBlacklist.addWord("then");
    wordBlacklist.addWord("where");
    wordBlacklist.addWord("does");
    wordBlacklist.addWord("could");
    wordBlacklist.addWord("did");
    wordBlacklist.addWord("those");
    wordBlacklist.addWord("want");
}

void Ai::getAssociationsLeaderboard(const Note* n, vector<Note*>& leaderboard)
{
    static const int AA_LEADERBOARD_SIZE = 10;
    static const int AA_LEADERBOARD_LINE_EMPTY = -1;

    int aaLeaderboard[10][2]; for(int i=0; i<AA_LEADERBOARD_SIZE; i++) aaLeaderboard[i][0]=aaLeaderboard[i][1]=AA_LEADERBOARD_LINE_EMPTY;
    float aaLeaderboardMin=0.;
    int aaLeaderboardSize=0;

    float aa;
    for(int x=0, y=n->getAiAaMatrixIndex(); x<notes.size(); x++) {
        if(x==y) continue; // self on diagonal

        aa = aaMatrix[x][y];

        // update leaderboard
        if(aa >= aaLeaderboardMin) {
            if(!aaLeaderboardSize) {
                aaLeaderboard[0][0]=x;
                aaLeaderboard[0][1]=y;
                aaLeaderboardSize++;
                aaLeaderboardMin=aa;
            } else {
                if(aaLeaderboardSize==AA_LEADERBOARD_SIZE
                     &&
                   aaLeaderboardMin==aaMatrix[aaLeaderboard[AA_LEADERBOARD_SIZE-1][0]][aaLeaderboard[AA_LEADERBOARD_SIZE-1][1]])
                {
                    // don't replace minimim w/ same minimu - skip
                } else {
                    for(int i=0; i<AA_LEADERBOARD_SIZE; i++) {
                        if(aaLeaderboard[i][0]==AA_LEADERBOARD_LINE_EMPTY) {
                            // using empty leaderboard line
                            aaLeaderboard[i][0]=x;
                            aaLeaderboard[i][1]=y;
                            aaLeaderboardSize++;
                            break;
                        }
                        if(aa > aaMatrix[aaLeaderboard[i][0]][aaLeaderboard[i][1]]) {
                            // insert new value in here and shift >> the rest of leaderboard
                            int sx=aaLeaderboard[i][0];
                            int sy=aaLeaderboard[i][1];
                            aaLeaderboard[i][0]=x;
                            aaLeaderboard[i][1]=y;
                            if(aaLeaderboardSize<AA_LEADERBOARD_SIZE) aaLeaderboardSize++;
                            for(int j=i+1; j<AA_LEADERBOARD_SIZE; j++) {
                                if(aaLeaderboard[j][0] == -1) {
                                    aaLeaderboard[j][0]=sx;
                                    aaLeaderboard[j][1]=sy;
                                    break;
                                } else {
                                    int bx=aaLeaderboard[j][0];
                                    int by=aaLeaderboard[j][1];
                                    aaLeaderboard[j][0]=sx;
                                    aaLeaderboard[j][1]=sy;
                                    sx=bx;
                                    sy=by;
                                }
                            }
                        }
                    }
                    aaLeaderboardMin = aaMatrix[aaLeaderboard[aaLeaderboardSize-1][0]][aaLeaderboard[aaLeaderboardSize-1][1]];
                }
            }
        }
    }

    // print leaderboard
    for(int i=0; i<aaLeaderboardSize; i++) {
        leaderboard.push_back(notes[aaLeaderboard[i][0]]);
    }
}

} // m8r namespace
