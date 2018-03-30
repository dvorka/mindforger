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


    // TODO move to local calculate GLOBAL leaderboard on the fly (per N will be calculated on demand)
    const int AA_LEADERBOARD_SIZE = 10;
    const int AA_LEADERBOARD_LINE_EMPTY = -1;
    int aaLeaderboard[10][2]; for(int i=0; i<AA_LEADERBOARD_SIZE; i++) aaLeaderboard[i][0]=aaLeaderboard[i][1]=AA_LEADERBOARD_LINE_EMPTY;
    float aaLeaderboardMin=0.;
    int aaLeaderboardSize=0;

    // calculate FULL matrix of Ns associativity assessment for every N1 and N2 tuple
    float** aaMatrix = new float*[notes.size()];
    for(size_t i=0; i<notes.size(); ++i) {
        aaMatrix[i] = new float[notes.size()];
    }
    float aa;
    AssociationAssessmentNotesFeature aaFeature{};
    int x=-1, y;
    for(Note* n1:notes) {
        n1->setAiAaMatrixIndex(++x);
        y = -1;
        for(Note* n2:notes) {
            n2->setAiAaMatrixIndex(++y);
            if(n1==n2) {
                aaMatrix[x][y] = 1.;
            } else {
                // // TODO replace
                aaFeature.setHaveMutualRel(false);
                aaFeature.setTypeMatches(n1->getType()==n2->getType());
                // TODO replace foo value
                aaFeature.setSimilarityByTags(0.0);
                // TODO replace foo value
                aaFeature.setSimilarityByWords(0.0);

                // set both values above and below diagonal - detection will be faster later (no check x>y needed)
                aa = aaFeature.areNotesAssociatedMetric();
                //MF_DEBUG("  aa[" << x << "][" << y << "]=" << aa << endl);
                aaMatrix[x][y] = aa;
                aaMatrix[y][x] = aa;

                // update global leaderboard
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
        }
    }
    // print leaderboard
    cout << "Global N AA leaderboard("<< aaLeaderboardSize <<"):" << endl;
    for(int i=0; i<10; i++) {
        if(aaLeaderboard[i][0] != AA_LEADERBOARD_LINE_EMPTY) {
            cout << "  [" << aaLeaderboard[i][0] << "][" << aaLeaderboard[i][1] << "] " << aaMatrix[aaLeaderboard[i][0]][aaLeaderboard[i][1]] << endl;
        }
    }

    // TODO train NN - why (incorporate USER data), how much data to use, test set, ...
    //trainAaNn();

    delete aaMatrix;
}

float Ai::calculateSimilarityByWords(Note* n1, Note* n2)
{
    // TODO
    // formula: ... 10() ...
}

AssociationAssessmentNotesFeature* Ai::createAaFeature(Note* n1, Note* n2)
{
    AssociationAssessmentNotesFeature* result
        = new AssociationAssessmentNotesFeature{};

    // TODO replace foo value
    result->setHaveMutualRel(false);
    result->setTypeMatches(n1->getType()==n2->getType());
    // TODO replace foo value
    result->setSimilarityByTags(0.0);
    // TODO replace foo value
    result->setSimilarityByWords(0.0);

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

} // m8r namespace
