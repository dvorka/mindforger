/*
 trie_test.cpp     MindForger application test

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "gear/trie.h"
#include "gear/file_utils.h"

using namespace std;

extern char* getMindforgerGitHomePath();

TEST(TrieTestCase, AddAndSearch)
{
    // GIVEN
    vector<string> words{};
    words.push_back("I");
    words.push_back("you");
    words.push_back("yours");
    words.push_back("he");

    // WHEN
    cout << "Building TRIE[" << words.size() << "]:" << endl;
    m8r::Trie trie{};
    for(string& w:words) {
        trie.addWord(w);
    }

    // THEN
    trie.print();

    ASSERT_EQ(false, trie.empty());
    string s{"yours"};
    ASSERT_TRUE(trie.findWord(s));
}

TEST(TrieTestCase, AddAndRemove)
{
    // add word twice > print
    // remove word w/ refcount 1 > print
    // remove word w/ refcount 2 > print
    // find removed word (False)

    // build trie
    vector<string> words{};
    words.push_back("A");
    words.push_back("twice");
    words.push_back("twice");
    words.push_back("once");
    words.push_back("BB");
    m8r::Trie trie{};
    for(string& w:words) {
        trie.addWord(w);
    }

    cout << "INITIAL trie..." << endl;
    trie.print();

    // remove word w/ refcount 1
    string word{"once"};
    ASSERT_TRUE(trie.findWord(word));
    cout << "REMOVING (1) word..." << endl;
    trie.removeWord(word);
    cout << "AFTER (1) remove..." << endl;
    trie.print();
    ASSERT_FALSE(trie.findWord(word));

    // remove word w/ refcount 2
    word.assign("twice");
    ASSERT_TRUE(trie.findWord(word));
    cout << "REMOVING (1/2) word..." << endl;
    trie.removeWord(word, true);
    cout << "AFTER (1/2) remove..." << endl;
    trie.print();
    ASSERT_TRUE(trie.findWord(word));

    cout << "REMOVING (2/2) word..." << endl;
    trie.removeWord(word, true);
    cout << "AFTER (2/2) remove..." << endl;
    int count = trie.print();
    ASSERT_FALSE(trie.findWord(word));
    ASSERT_EQ(13, count);
}
