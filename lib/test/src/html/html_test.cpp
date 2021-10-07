/*
 markdown_benchmark.cpp     MindForger markdown test

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

#include <cstdlib>
#include <iostream>
#include <memory>
#include <cstdio>
#ifndef _WIN32
#  include <unistd.h>
#endif //_WIN32

#include <gtest/gtest.h>

#include "../test_utils.h"
#include "representations/html/html_outline_representation.h"
#include "mind/mind.h"
#include "persistence/filesystem_persistence.h"

using namespace std;

extern char* getMindforgerGitHomePath();

TEST(HtmlTestCase, Outline)
{    
    string fileName{"/lib/test/resources/benchmark-repository/memory/meta.md"};
    fileName.insert(0, getMindforgerGitHomePath());

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-htc-o.md");
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(fileName)),
        repositoryConfigRepresentation
    );
    m8r::Mind mind(config);
    m8r::HtmlColorsMock dummyColors{};
    m8r::HtmlOutlineRepresentation htmlRepresentation{mind.remind().getOntology(), dummyColors, nullptr};
    m8r::MarkdownOutlineRepresentation markdownRepresentation(mind.remind().getOntology(), nullptr);
    mind.learn();
    mind.think().get();

    ASSERT_GE(mind.remind().getOutlinesCount(), 1);

    // MD: 1.1MiB
    string markdown{};
    markdownRepresentation.to(mind.remind().getOutlines()[0], &markdown);
    cout << "Markdown " << markdown.size() << "B" << endl;

    string html{};
    htmlRepresentation.to(&markdown, &html);

    // HTML not printed - it is huge
    // cout << "= BEGIN HTML =" << endl << html << endl << "= END HTML =" << endl;

    EXPECT_NE(std::string::npos, html.find("increment"));
    EXPECT_NE(std::string::npos, html.find("Meyers"));
    EXPECT_NE(std::string::npos, html.find("Stroustrup"));
}

TEST(HtmlTestCase, Note)
{
    string fileName{"/lib/test/resources/benchmark-repository/memory/meta.md"};
    fileName.insert(0, getMindforgerGitHomePath());

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-htc-n.md");
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(fileName)),
        repositoryConfigRepresentation
    );
    m8r::Mind mind(config);
    m8r::HtmlColorsMock dummyColors{};
    m8r::HtmlOutlineRepresentation htmlRepresentation{mind.remind().getOntology(),dummyColors,nullptr};
    mind.learn();
    mind.think().get();

    ASSERT_GE(mind.remind().getOutlinesCount(), 1);

    string html{};
    htmlRepresentation.to(mind.remind().getOutlines()[0]->getNotes()[0], &html);

    cout << "= BEGIN HTML =" << endl << html << endl << "= END HTML =" << endl;
}

TEST(HtmlTestCase, NoteLinks)
{
    string fileName{"/lib/test/resources/markdown-repository/memory/feature-html-links.md"};
    fileName.insert(0, getMindforgerGitHomePath());

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-antc-nl.md");
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(fileName)),
        repositoryConfigRepresentation
    );
    m8r::Mind mind(config);
    m8r::HtmlColorsMock dummyColors{};
    m8r::HtmlOutlineRepresentation htmlRepresentation{mind.remind().getOntology(),dummyColors,nullptr};
    mind.learn();
    mind.think().get();

    ASSERT_GE(mind.remind().getOutlinesCount(), 1);

    string html{};
    htmlRepresentation.to(mind.remind().getOutlines()[0]->getNotes()[0], &html);

    // links are NOT resolved - they are kept as they are
    cout << "= BEGIN HTML =" << endl << html << endl << "= END HTML =" << endl;
}

TEST(HtmlTestCase, TaskList)
{
    string fileName{"/lib/test/resources/markdown-repository/memory/feature-task-list.md"};
    fileName.insert(0, getMindforgerGitHomePath());

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-antc-tl.md");
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(fileName)),
        repositoryConfigRepresentation
    );
    m8r::Mind mind(config);
    m8r::HtmlColorsMock dummyColors{};
    m8r::HtmlOutlineRepresentation htmlRepresentation{mind.remind().getOntology(),dummyColors,nullptr};
    mind.learn();
    mind.think().get();

    ASSERT_GE(mind.remind().getOutlinesCount(), 1);

    string html{};
    // O header
    htmlRepresentation.to(mind.remind().getOutlines()[0]->getOutlineDescriptorAsNote(), &html);
    cout << "= BEGIN O HTML =" << endl << html << endl << "= END O HTML =" << endl;
    EXPECT_NE(std::string::npos, html.find("input"));
    // N
    html.clear();
    htmlRepresentation.to(mind.remind().getOutlines()[0]->getNotes()[0], &html);
    cout << "= BEGIN N HTML =" << endl << html << endl << "= END N HTML =" << endl;
    EXPECT_NE(std::string::npos, html.find("input"));
}
