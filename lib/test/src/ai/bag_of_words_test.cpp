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
    map<string, int> bow;

    string w{};
    for(size_t i=0; i<markdown.size(); i++) {
        // TODO incorporate character provider - can read string, vector<string>, N or O
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

}
