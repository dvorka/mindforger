/*
 emoji_test.cpp     MindForger application test

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../../../src/representations/emoji.h"

using namespace std;
using namespace m8r;

/**
 * @brief Get the characters of the found emojis.
 */
static vector<string> findCharacters(const string& expression)
{
    vector<string> characters{};
    for(const Emoji* emoji:Emojis::getInstance().findEmojis(expression)) {
        characters.push_back(emoji->getCharacter());
    }

    return characters;
}

TEST(EmojiTestCase, SearchWords)
{
    // GIVEN a search expression with mixed case and extra whitespaces
    string expression{"  Green   CIRCLE "};

    // WHEN it is split to search words
    vector<string> words = Emojis::toSearchWords(expression);

    // THEN words are lowercased and empty words are dropped
    cout << "Search words of '" << expression << "':" << endl;
    for(const string& word:words) {
        cout << "  '" << word << "'" << endl;
    }
    ASSERT_EQ(2, words.size());
    ASSERT_EQ("green", words[0]);
    ASSERT_EQ("circle", words[1]);

    // GIVEN a blank expression
    // WHEN it is split to search words
    words = Emojis::toSearchWords("   ");
    // THEN there are no words to search by
    ASSERT_TRUE(words.empty());
}

TEST(EmojiTestCase, MatchEmoji)
{
    // GIVEN an emoji with a name and keywords
    Emoji emoji{"🚀", "rocket", "launch start boost"};

    // WHEN/THEN a prefix of the name matches
    ASSERT_TRUE(emoji.matches(Emojis::toSearchWords("roc")));
    // WHEN/THEN the match is case insensitive
    ASSERT_TRUE(emoji.matches(Emojis::toSearchWords("ROCKET")));
    // WHEN/THEN a keyword matches
    ASSERT_TRUE(emoji.matches(Emojis::toSearchWords("launch")));
    // WHEN/THEN ALL the words must match
    ASSERT_TRUE(emoji.matches(Emojis::toSearchWords("rocket boost")));
    ASSERT_FALSE(emoji.matches(Emojis::toSearchWords("rocket bug")));
    // WHEN/THEN no words match everything
    ASSERT_TRUE(emoji.matches(Emojis::toSearchWords("")));
    // WHEN/THEN an unrelated word does not match
    ASSERT_FALSE(emoji.matches(Emojis::toSearchWords("beetle")));

    cout << "Emoji '" << emoji.getCharacter() << "' named '"
         << emoji.getName() << "' matched as expected: DONE" << endl;
}

TEST(EmojiTestCase, FindEmojis)
{
    // GIVEN the emojis catalog
    const Emojis& emojis = Emojis::getInstance();

    // WHEN searched by a name prefix
    vector<string> found = findCharacters("rock");
    // THEN exactly the rocket is found
    cout << "Emojis found by 'rock': " << found.size() << endl;
    ASSERT_EQ(1, found.size());
    ASSERT_EQ("🚀", found[0]);

    // WHEN searched by a keyword which is shared by several emojis
    found = findCharacters("bug");
    // THEN the lady beetle is among the results
    ASSERT_FALSE(found.empty());
    ASSERT_NE(found.end(), find(found.begin(), found.end(), "🐞"));

    // WHEN searched by a greek letter name
    found = findCharacters("lambda");
    // THEN both the capital and the small letter are found
    cout << "Emojis found by 'lambda': " << found.size() << endl;
    ASSERT_EQ(2, found.size());
    ASSERT_EQ("Λ", found[0]);
    ASSERT_EQ("λ", found[1]);

    // WHEN searched by words which narrow the result
    found = findCharacters("lambda capital");
    // THEN only the capital letter is found
    ASSERT_EQ(1, found.size());
    ASSERT_EQ("Λ", found[0]);

    // WHEN searched by an expression which matches nothing
    found = findCharacters("thisisnotanemoji");
    // THEN nothing is found
    ASSERT_TRUE(found.empty());

    // WHEN searched by a blank expression
    found = findCharacters("  ");
    // THEN the whole catalog is returned
    size_t catalogSize{0};
    for(const EmojiSection& section:emojis.getSections()) {
        catalogSize += section.getEmojis().size();
    }
    cout << "Emojis in the catalog: " << catalogSize << endl;
    ASSERT_EQ(catalogSize, found.size());
    ASSERT_LT(100, catalogSize);
}

TEST(EmojiTestCase, CatalogIntegrity)
{
    // GIVEN the emojis catalog
    const Emojis& emojis = Emojis::getInstance();

    // WHEN all the sections and emojis are walked through
    set<string> characters{};
    set<string> duplicates{};
    size_t namelessCount{0};
    ASSERT_FALSE(emojis.getSections().empty());
    for(const EmojiSection& section:emojis.getSections()) {
        ASSERT_FALSE(section.getName().empty());
        ASSERT_FALSE(section.getEmojis().empty());

        for(const Emoji& emoji:section.getEmojis()) {
            ASSERT_FALSE(emoji.getCharacter().empty());
            if(emoji.getName().empty()) {
                namelessCount++;
            }
            if(!characters.insert(emoji.getCharacter()).second) {
                duplicates.insert(emoji.getCharacter());
            }
        }
    }

    // THEN every emoji has a name and no character is offered twice
    cout << "Emojis w/o a name: " << namelessCount << endl;
    for(const string& duplicate:duplicates) {
        cout << "ERROR: duplicate emoji '" << duplicate << "'" << endl;
    }
    ASSERT_EQ(0, namelessCount);
    ASSERT_TRUE(duplicates.empty());
}
