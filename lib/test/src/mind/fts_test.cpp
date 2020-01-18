/*
 fts_test.cpp     MindForger fullt text search test

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <stddef.h>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../../../src/config/configuration.h"
#include "../../../src/mind/mind.h"

extern char* getMindforgerGitHomePath();

using namespace std;

void printFtsResult(vector<m8r::Note*>* result)
{
    if(result->size()) {
        cout << " FOUND " << result->size() << " result(s):" << endl;
        for(size_t i=0; i<result->size(); i++) {
            cout << "  #" << i << " " <<
                    result->at(i)->getOutline()->getName() << " / " <<
                    result->at(i)->getName() << endl;
        }
    } else {
        cout << " NOTHING found" << endl;
    }
}

TEST(FtsTestCase, FTS) {
    string repositoryPath{"/lib/test/resources/basic-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mtc-f.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));

    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();

    cout << endl << "Statistics:" << endl;
    cout << "  Outlines: " << mind.remind().getOutlinesCount() << endl;
    cout << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize() << endl;

    cout << "EXACT search..." << endl;

    string pattern("hash");
    vector<m8r::Note*>* result = mind.findNoteFts(pattern, m8r::FtsSearch::EXACT);
    printFtsResult(result);
    EXPECT_NE(nullptr, result);
    EXPECT_EQ(2, result->size());
    delete result;

    cout << "IGNORE CASE search..." << endl;

    pattern.assign("hash");
    result = mind.findNoteFts(pattern, m8r::FtsSearch::IGNORE_CASE);
    printFtsResult(result);
    EXPECT_NE(nullptr, result);
    EXPECT_EQ(3, result->size());
    delete result;

    cout << "REGEX search..." << endl;

    pattern.assign("lo*king");
    result = mind.findNoteFts(pattern, m8r::FtsSearch::REGEXP);
    printFtsResult(result);
    EXPECT_NE(nullptr, result);
    EXPECT_EQ(2, result->size());
    delete result;
}
