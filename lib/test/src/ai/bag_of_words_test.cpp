/*
 bag_of_words_test.cpp     MindForger application test

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

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <map>

extern "C" {
#include <mkdio.h>
}

#include <gtest/gtest.h>

using namespace std;

TEST(AiBowTestCase, DescriptionToWordsList)
{
    string markdown;
    markdown.assign(
        "Outline Name\n"
        "========\n"
        "O text begin [LINK-LABEL](http://link-1.com) text end.\n"
        "\n"
        "First Section\n"
        "-------------\n"
        "N1 text `N1 inline code` N1 text end.\n"
        "\n"
        "## Second Section\n"
        "S2 intro text:\n"
        "```\n"
        "N2 text codeblock.\n"
        "```\n"
        "\n"
        "Note 3\n"
        "-------------\n"
        "N2 text.\n"
        "\n");

    cout << "Tokenizing..." << endl;
    // tokenizer
    //   - hardcoded delimiters
    //   - filters out words w/ length <1
    //   - stems words
    //   - computes token frequency (TF)
    //   - RESULT is additive i.e. adding to map can be used to build:
    //     a) lexicon which is union of all BoWs (has total frequencies)
    //     b) per document BoW
    //   - perhaps MAP is better than ARRAY as structure for further processing
    map<string,int> bow;

    string w{};
    for(size_t i=0; i<markdown.size(); i++) {
        // TODO incorporate character provider StringCharProvider - can read string, vector<string>, N or O
        switch(markdown[i]) {
        case '-':
            // check lookahead
            if(i<markdown.size()-1 && markdown[i+1]!='-') {
                w += markdown[i];
                break;
            }
        case ' ':
        case '\n':
        case '\r':
        case '!':
        case '?':
        case '.':
        case ':':
        case ';':
        case '#':
        case '=':
        case '`':
        case '(':
        case ')':
        case '[':
        case ']':
        case '*':
        case '_':
        case '"':
        case '\'':
        case '~':
        case '@':
        case '$':
        case '%':
        case '^':
        case '&':
        case '+':
        case '{':
        case '}':
        case '|':
        case '\\':
        case '<':
        case '>':
        case '/':
            if(w.size()>1) {
                // TODO stem word
                ++bow[w];
            }
            w.clear();
            break;
        default:
            w += markdown[i];
            break;
        }
    }

    cout << "BoW[" << bow.size() << "]:" << endl;
    for(auto s:bow) {
        cout << "  " << (s.first) << " [" << s.second << "] " << endl;
    }

    // Having BoW next steps are:
    //   - create similarityEmbedding ~ for every Ni vector w/ [[bow],stemmed tags,relationship,type]
    //   - for every Ni and Nj create notesSimilarityFeature ~ smart unit using similarityEmbedding[Ni] and se[Nj]
    //     [# matching words in total, avg of at most words TF-IDF, # matching tags, have relationship, # relationship to same target, type matches]
    //   - now ask NN(notesSimilarityFeature[Ni,Nj]) = {1/0} ... similar/not similar (associated each other/don't ...)

    /**
     * @brief Lexicon of all words w/ global frequencies.
     */
    class Lexicon {
    private:
        map<string,int> m; // duplicity detection on merging
        vector<pair<string*,int>> words;
    public:
        void add(map<string,int>) {}
        void finalize() {} // delete map when nothing more will be added, blocks new additions
        string& operator [](int) {} // get word for index

    };

    class ThingEmbedding {
    };

    class NoteEmbedding : public ThingEmbedding {
        string type;
        map<string,int> bow;
        //map<> stemmed tags;
        //relationshipTargets
    };


    /**
     * @brief Are-notes-associated NN input feature (simmetric relation).
     */
    class NotesSimilarityFeature {
    public:
        // 30%
        static constexpr int IDX_HAVE_MUTUAL_REL = 0;
        // 10% (x0% remains)
        static constexpr int IDX_TYPE_MATCHES = 4;
        // 20% (x0% remains)
        static constexpr int IDX_SIMILARITY_BY_TAGS = 2;

        // x0% (x0% remains)
        static constexpr int IDX_SIMILARITY_BY_TITLES= 2;
        // x0% (x0% remains)
        static constexpr int IDX_SIMILARITY_BY_RELEVANT_WORDS_IN_DESCS = 2;
        // x0% (x0% remains)
        static constexpr int IDX_SIMILARITY_BY_TITLES_IN_DESCS = 2;
        // x0% (x0% remains)
        static constexpr int IDX_SIMILARITY_BY_RELS_W_SAME_TARGETS = 3;

    private:
        pair<Note*,Note*> notes;
        float features[];

    public:
        void setHaveMutualRel(bool haveRel) {
            features[IDX_HAVE_MUTUAL_REL] = haveRel?1.:0.;
        }

        /**
         * @brief Set similarity computed just by tags.
         *
         * Consider the following examples:
         *  [important, ai, ml] and [ai] -> ~80% related
         *  [important, ai], [important, running] -> 5% related
         *  [important, ai], [cool, ai] -> ~100% related
         *
         * Hints:
         *  - distinquish VALUABLE tags (ai) and invaluable tags (cool)
         *  - give tags WEIGHTS by their value
         *  - formula
         *    - unionTags(N1,N2) = [split 100% among all tags found in N1 or N2 by tag value]
         *    - intersectionTags(N1,N2) = sum(% of tags in both Ns - relevant bring more, worthless less)
         *
         * @param similarityByTags percentage of similarity by tags [0,1]
         *
         * IMPROVE:
         *  - semantic word vector make solve tag importance problem (wordVec)
         *  - TF-IDF of tags
         */
        void setSimilarityByTags(float similarityByTags) {
            features[IDX_HAVE_WEIGHTED_MATCHING_TAGS] = similarityByTags;
        }

        void setTypeMatches(bool typeMatches) {
            features[IDX_HAVE_MUTUAL_REL] = typeMatches?1.:0.;
        }

        /**
         * @brief Set similarity computed by words in title.
         */

        // TODO N1 title occurs in body of N2, N2 in N1, mutual title containment ... means related

        /**
         * @brief Set similarity computed by words in description.
         */
        void setSimilarityByWords(float similarityByTags) {
            features[IDX_HAVE_WEIGHTED_MATCHING_TAGS] = similarityByTags;
        }

        /**
         * @brief Metric that estimates similarity w/o NN i.e. it's an exact formula I proposed w/ range=[0,1]
         *
         * I start from 100% i.e. what makes Ns to be associations. I alocate portions of 100% to
         * each similarity aspect A: 10%*A1+40%*A2+...+5%*AN=100% (<=> if As==1).
         */
        float areNotesAssociatedMetric() {
            return (
                features[IDX_HAVE_MUTUAL_REL]*30. +
                features[IDX_HAVE_MUTUAL_REL]*30.
              )/100.;
        }
    };

}
