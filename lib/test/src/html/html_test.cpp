/*
 markdown_benchmark.cpp     MindForger markdown test

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

TEST(HtmlTestCase, MathSupportDisabled)
{
    // GIVEN: math support disabled (the default)
    string fileName{"/lib/test/resources/benchmark-repository/memory/meta.md"};
    fileName.insert(0, getMindforgerGitHomePath());
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-htc-math-no.md");
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(fileName)),
        repositoryConfigRepresentation
    );
    config.setUiEnableMathInMd(m8r::Configuration::MathJsLibSupport::MATH_NO);
    m8r::Mind mind(config);
    m8r::HtmlColorsMock dummyColors{};
    m8r::HtmlOutlineRepresentation htmlRepresentation{mind.remind().getOntology(), dummyColors, nullptr};
    mind.learn();
    mind.think().get();
    string markdown{"# Test\n\nInline math: $E=mc^2$\n"};

    // WHEN
    string html{};
    htmlRepresentation.to(&markdown, &html);

    // THEN: no math JS/CSS is injected at all
    EXPECT_EQ(std::string::npos, html.find("katex"));
    EXPECT_EQ(std::string::npos, html.find("mathjax"));
}

TEST(HtmlTestCase, MathSupportKaTeXIsOfflineOnly)
{
    // GIVEN: KaTeX selected as the math rendering engine
    string fileName{"/lib/test/resources/benchmark-repository/memory/meta.md"};
    fileName.insert(0, getMindforgerGitHomePath());
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-htc-math-katex.md");
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(fileName)),
        repositoryConfigRepresentation
    );
    config.setUiEnableMathInMd(m8r::Configuration::MathJsLibSupport::MATH_KATEX);
    m8r::Mind mind(config);
    m8r::HtmlColorsMock dummyColors{};
    m8r::HtmlOutlineRepresentation htmlRepresentation{mind.remind().getOntology(), dummyColors, nullptr};
    mind.learn();
    mind.think().get();
    string markdown{"# Test\n\nInline math: $E=mc^2$\n"};

    // WHEN
    string html{};
    htmlRepresentation.to(&markdown, &html);

    // THEN: KaTeX is loaded exclusively from bundled Qt resources - never the network
    EXPECT_NE(std::string::npos, html.find("qrc:/js/katex.min.js"));
    EXPECT_NE(std::string::npos, html.find("qrc:/js/katex-auto-render.min.js"));
    EXPECT_NE(std::string::npos, html.find("qrc:/html-css/katex.min.css"));
    EXPECT_EQ(std::string::npos, html.find("mathjax"));
    EXPECT_EQ(std::string::npos, html.find("http://"));
    EXPECT_EQ(std::string::npos, html.find("https://"));
    EXPECT_EQ(std::string::npos, html.find("\\\\("));
}

TEST(HtmlTestCase, MathSupportMathJaxLegacyIsOfflineOnly)
{
    // GIVEN: MathJax (legacy) selected as the math rendering engine
    string fileName{"/lib/test/resources/benchmark-repository/memory/meta.md"};
    fileName.insert(0, getMindforgerGitHomePath());
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-htc-math-mathjax.md");
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(fileName)),
        repositoryConfigRepresentation
    );
    config.setUiEnableMathInMd(m8r::Configuration::MathJsLibSupport::MATH_MATHJAX);
    m8r::Mind mind(config);
    m8r::HtmlColorsMock dummyColors{};
    m8r::HtmlOutlineRepresentation htmlRepresentation{mind.remind().getOntology(), dummyColors, nullptr};
    mind.learn();
    mind.think().get();
    string markdown{"# Test\n\nInline math: $E=mc^2$\n"};

    // WHEN
    string html{};
    htmlRepresentation.to(&markdown, &html);

    // THEN: MathJax is loaded exclusively from bundled Qt resources - never the network
    EXPECT_NE(std::string::npos, html.find("qrc:/js/mathjax-tex-svg.js"));
    EXPECT_EQ(std::string::npos, html.find("katex"));
    EXPECT_EQ(std::string::npos, html.find("http://"));
    EXPECT_EQ(std::string::npos, html.find("https://"));
    EXPECT_EQ(std::string::npos, html.find("\\\\("));
}

TEST(HtmlTestCase, DiagramSupportOfflineMermaidIsFixed)
{
    // GIVEN: offline Mermaid diagrams selected - previously broken since mermaid.js
    // was never registered as a Qt resource (qrc:/js/mermaid.js 404-ed at runtime)
    string fileName{"/lib/test/resources/benchmark-repository/memory/meta.md"};
    fileName.insert(0, getMindforgerGitHomePath());
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-htc-diagram-offline.md");
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(fileName)),
        repositoryConfigRepresentation
    );
    config.setUiEnableDiagramsInMd(m8r::Configuration::JavaScriptLibSupport::OFFLINE);
    m8r::Mind mind(config);
    m8r::HtmlColorsMock dummyColors{};
    m8r::HtmlOutlineRepresentation htmlRepresentation{mind.remind().getOntology(), dummyColors, nullptr};
    mind.learn();
    mind.think().get();
    string markdown{"# Test\n\n```mermaid\ngraph TD; A-->B;\n```\n"};

    // WHEN
    string html{};
    htmlRepresentation.to(&markdown, &html);

    // THEN
    EXPECT_NE(std::string::npos, html.find("qrc:/js/mermaid.js"));
    EXPECT_EQ(std::string::npos, html.find("https://cdnjs.cloudflare.com"));
}

TEST(HtmlTestCase, DiagramSupportOnlineIsNoLongerOffered)
{
    // GIVEN: ONLINE explicitly set (no longer reachable from the Preferences UI,
    // which now offers only "disable"/"offline" - kept in the enum only for
    // backward-compat config-file migration to OFFLINE)
    string fileName{"/lib/test/resources/benchmark-repository/memory/meta.md"};
    fileName.insert(0, getMindforgerGitHomePath());
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-htc-diagram-online.md");
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(fileName)),
        repositoryConfigRepresentation
    );
    config.setUiEnableDiagramsInMd(m8r::Configuration::JavaScriptLibSupport::ONLINE);
    m8r::Mind mind(config);
    m8r::HtmlColorsMock dummyColors{};
    m8r::HtmlOutlineRepresentation htmlRepresentation{mind.remind().getOntology(), dummyColors, nullptr};
    mind.learn();
    mind.think().get();
    string markdown{"# Test\n\n```mermaid\ngraph TD; A-->B;\n```\n"};

    // WHEN
    string html{};
    htmlRepresentation.to(&markdown, &html);

    // THEN: no diagram JS is injected at all for interactive (non-standalone) rendering
    EXPECT_EQ(std::string::npos, html.find("mermaid.js"));
    EXPECT_EQ(std::string::npos, html.find("http://"));
    EXPECT_EQ(std::string::npos, html.find("https://"));
}
