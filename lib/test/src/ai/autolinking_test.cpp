/*
 autolinkin_test.cpp     MindForger application test

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

#include <vector>
#include <string>

#include "../../../src/gear/string_utils.h"
#include "../../../src/config/configuration.h"
#include "../../../src/mind/mind.h"
#include "../../../src/mind/ai/ai.h"
#include "../../../src/representations/markdown/markdown_outline_representation.h"
#include "../../../src/mind/ai/autolinking/naive_autolinking_preprocessor.h"

#include "../test_utils.h"

#include <gtest/gtest.h>

extern char* getMindforgerGitHomePath();
extern void toString(const std::vector<std::string*> ss, std::string& os);

using namespace std;

#ifndef MF_MD_2_HTML_CMARK

TEST(AutolinkingTestCase, NaiveAutolinker)
{
    string repositoryPath{"/lib/test/resources/basic-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-atc-a.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();
    cout << endl << "Statistics:";
    cout << endl << "  Outlines: " << mind.remind().getOutlinesCount();
    cout << endl << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize();
    ASSERT_EQ(3, mind.remind().getOutlinesCount());
    m8r::NaiveAutolinkingPreprocessor autolinker{mind};

    cout << endl << endl << "Testing MD autolinking:" << endl;
    m8r::Note* n = mind.remind().getOutlines()[0]->getNotes()[0];
    vector<string*> autolinkedMd{};
    autolinker.process(n->getDescription(), autolinkedMd);
    string autolinkedString{};
    m8r::toString(autolinkedMd, autolinkedString);
    cout << "= BEGIN AUTO MD =" << endl << autolinkedString << endl << "= END AUTO MD =" << endl;

    for(string* s:autolinkedMd) {
        delete s;
    }
}

TEST(AutolinkingTestCase, NaiveMarkdownRepresentation)
{
    string repositoryPath{"/lib/test/resources/basic-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-atc-mr.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    config.setAutolinking(true);
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();
    cout << endl << "Statistics:";
    cout << endl << "  Outlines: " << mind.remind().getOutlinesCount();
    cout << endl << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize();
    ASSERT_EQ(3, mind.remind().getOutlinesCount());
    m8r::NaiveAutolinkingPreprocessor autolinker{mind};
    m8r::MarkdownOutlineRepresentation mdRepresentation{mind.remind().getOntology(), &autolinker};

    cout << endl << endl << "Testing MD autolinking:" << endl;
    m8r::Note* n = mind.remind().getOutlines()[0]->getNotes()[0];
    string* autolinkedString = mdRepresentation.to(n);
    cout << "= BEGIN AUTO! MD =" << endl << *autolinkedString << endl << "= END AUTO! MD =" << endl;

    delete autolinkedString;
}

TEST(AutolinkingTestCase, NaiveCrashAndBurn)
{
    string repositoryPath{"/lib/test/resources/autolinking-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-atc-cnb.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    config.setAutolinking(true);
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();
    cout << endl << "Statistics:";
    cout << endl << "  Outlines: " << mind.remind().getOutlinesCount();
    cout << endl << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize();
    ASSERT_EQ(1, mind.remind().getOutlinesCount());
    m8r::NaiveAutolinkingPreprocessor autolinker{mind};

    cout << endl << endl << "Testing MD autolinking:" << endl;
    m8r::Note* n = mind.remind().getOutlines()[0]->getNotes()[0];
    vector<string*> autolinkedMd{};
    autolinker.process(n->getDescription(), autolinkedMd);
    string autolinkedString{};
    m8r::toString(autolinkedMd, autolinkedString);
    cout << "= BEGIN AUTO MD =" << endl << autolinkedString << endl << "= END AUTO MD =" << endl;

    // ensure original links are intact
    EXPECT_NE(std::string::npos, autolinkedString.find("[with Blue sky](./and/link/to/Blue)"));

    for(string* s:autolinkedMd) {
        delete s;
    }
}

#endif
