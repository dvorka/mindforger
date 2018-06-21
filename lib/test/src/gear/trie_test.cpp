/*
 trie_test.cpp     MindForger application test

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
#include <vector>
#include <map>
#include <string>

#include <gtest/gtest.h>

#include "../../src/gear/trie.h"
#include "../../src/gear/file_utils.h"

using namespace std;

extern char* getMindforgerGitHomePath();

TEST(TrieTestCase, AddAndSearch)
{
    vector<string> words{};
    words.push_back("I");
    words.push_back("you");
    words.push_back("yours");
    words.push_back("he");

    cout << "Building TRIE[" << words.size() << "]:" << endl;
    m8r::Trie trie{};
    for(string& w:words) {
        trie.addWord(w);
    }

    ASSERT_EQ(false, trie.empty());
    string s{"yours"};
    ASSERT_TRUE(trie.findWord(s));
}
