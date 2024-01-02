/*
 trie_benchmark.cpp     MindForger markdown test

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <vector>
#include <map>
#include <string>

#include <gtest/gtest.h>

#include "../../src/gear/trie.h"
#include "../../src/gear/file_utils.h"

using namespace std;
using namespace m8r;

extern char* getMindforgerGitHomePath();

/*
RESULT: trie seems to be 10% faster:

Building MAP[191480]
191480 words ADDED in 31.814ms
MAP done
Searching MAP[191480]
191480 words SEARCHED in 31.345ms
MAP done

Building TRIE[191480]:
191480 words ADDED in 23.236ms
TRIE done
Searching TRIE[191480]
191480 words SEARCHED in 26.602ms
TRIE done
 */
TEST(TrieBenchmark, DISABLED_TrieVsMap)
{
    // 1.1M file
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/benchmark-repository/memory/meta.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());
    string* s = m8r::fileToString(*fileName.get());
    // split to words
    vector<string> words{};
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    cout << "Splitting to words:" << endl;
    while((pos = s->find(delimiter)) != string::npos) {
        token = s->substr(0, pos);
        //std::cout << token << std::endl;
        words.push_back(token);
        s->erase(0, pos + delimiter.length());
    }
    //cout << s << endl;
    cout << "Split done" << endl;

    /*
     * MAP
     */

    cout << "Building MAP[" << words.size() << "]" << endl;
    map<string,int> m;
    auto beginMapBuild = chrono::high_resolution_clock::now();
    for(string& w:words) {
        m[w] = 1;
    }
    auto endMapBuild = chrono::high_resolution_clock::now();
    MF_DEBUG(words.size() << " words ADDED in " << chrono::duration_cast<chrono::microseconds>(endMapBuild-beginMapBuild).count()/1000.0 << "ms" << endl);
    cout << "MAP done" << endl;

    cout << "Searching MAP[" << words.size() << "]" << endl;
    auto beginMapSearch = chrono::high_resolution_clock::now();
    for(string& w:words) {
        m[w];
    }
    auto endMapSearch= chrono::high_resolution_clock::now();
    MF_DEBUG(words.size() << " words SEARCHED in " << chrono::duration_cast<chrono::microseconds>(endMapSearch-beginMapSearch).count()/1000.0 << "ms" << endl);
    cout << "MAP done" << endl;

    /*
     * TRIE
     */

    cout << "Building TRIE[" << words.size() << "]:" << endl;
    Trie trie{};
    auto beginTrieBuild = chrono::high_resolution_clock::now();
    for(string& w:words) {
        trie.addWord(w);
    }
    auto endTrieBuild = chrono::high_resolution_clock::now();
    MF_DEBUG(words.size() << " words ADDED in " << chrono::duration_cast<chrono::microseconds>(endTrieBuild-beginTrieBuild).count()/1000.0 << "ms" << endl);
    //MF_DEBUG(" ~ AVG: " << chrono::duration_cast<chrono::microseconds>(endTrieBuild-beginTrieBuild).count()/1000000.0 << "ms" << endl);
    cout << "TRIE done" << endl;

    cout << "Searching TRIE[" << words.size() << "]" << endl;
    auto beginTrieSearch = chrono::high_resolution_clock::now();
    for(string& w:words) {
        trie.findWord(w);
    }
    auto endTrieSearch= chrono::high_resolution_clock::now();
    MF_DEBUG(words.size() << " words SEARCHED in " << chrono::duration_cast<chrono::microseconds>(endTrieSearch-beginTrieSearch).count()/1000.0 << "ms" << endl);
    cout << "TRIE done" << endl;
}
