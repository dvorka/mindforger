/*
 markdown_benchmark.cpp     MindForger markdown test

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

#include <cstdlib>
#include <iostream>
#include <memory>
#include <cstdio>
#include <unistd.h>

#include <gtest/gtest.h>

#include "../src/test_gear.h"
#include "../../src/representations/html/html_outline_representation.h"
#include "../../src/mind/mind.h"
#include "../../src/persistence/filesystem_persistence.h"

using namespace std;

extern char* getMindforgerGitHomePath();

// 2018/03/18 110MiB (100x1.1MiB) MDs 2 HTML converted in 2496.91ms ~ AVG: 2.49691ms
TEST(HtmlBenchmark, DISABLED_Outline)
{    
    string fileName{"/lib/test/resources/benchmark-repository/memory/meta.md"};
    fileName.insert(0, getMindforgerGitHomePath());

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-hb-o.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(fileName)));
    m8r::Mind mind(config);
    m8r::DummyHtmlColors dummyColors{};
    m8r::HtmlOutlineRepresentation htmlRepresentation{mind.remind().getOntology(),dummyColors,nullptr};
    m8r::MarkdownOutlineRepresentation markdownRepresentation(mind.remind().getOntology(),nullptr);
    mind.think();

    ASSERT_GE(mind.remind().getOutlinesCount(), 1);

    // MD: 1.1MiB
    string outlineAsMarkdown{};
    markdownRepresentation.to(mind.remind().getOutlines()[0], &outlineAsMarkdown);
    cout << "Markdown " << outlineAsMarkdown.size() << "B" << endl;

    // do >1 iterations
    const int ITERATIONS = 100;
    auto begin = chrono::high_resolution_clock::now();
    for(int i=0; i<ITERATIONS; i++) {
        string html{};
        htmlRepresentation.to(&outlineAsMarkdown, &html);
    }
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG(endl << (ITERATIONS*1.1) << "MiB (" << ITERATIONS << "x1.1MiB) MDs 2 HTML converted in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms");
    MF_DEBUG(" ~ AVG: " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000000.0 << "ms" << endl);
}
