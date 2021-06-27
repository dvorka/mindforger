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

#include "../../src/representations/markdown/markdown_lexem.h"
#include "../../src/representations/markdown/markdown_ast_node.h"
#include "../../src/representations/markdown/markdown_lexer_sections.h"
#include "../../src/representations/markdown/markdown_parser_sections.h"
#include "../../src/representations/markdown/markdown_outline_representation.h"

#include "../../src/config/configuration.h"
#include "../../src/mind/ontology/ontology.h"
#include "../../src/persistence/filesystem_persistence.h"

using namespace std;
using namespace m8r;

extern char* getMindforgerGitHomePath();

// 2018/03/02 100x = 2.460ms (120MiB)
TEST(MarkdownParserBenchmark, DISABLED_ParserMeta)
{    
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/benchmark-repository/memory/meta.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());

    // do >1 iterations
    const int ITERATIONS = 100;
    auto begin = chrono::high_resolution_clock::now();
    for(int i=0; i<ITERATIONS; i++) {
        cout << "." << flush;
        MarkdownLexerSections lexer(fileName.get());
        lexer.tokenize();
        MarkdownParserSections parser(lexer);
        parser.parse();

        EXPECT_TRUE(parser.hasMetadata());
    }
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG(endl << (ITERATIONS*1.2) << "MiB (" << ITERATIONS << "x1.1MiB) MDs parsed in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms");
    MF_DEBUG(" ~ AVG: " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000000.0 << "ms" << endl);
}

// 2018/03/02 100x = 1.000ms (770MiB)
TEST(MarkdownParserBenchmark, DISABLED_ParserNoMeta)
{
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/benchmark-repository/memory/nometa.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());

    // do >1 iterations
    const int ITERATIONS = 100;
    auto begin = chrono::high_resolution_clock::now();
    for(int i=0; i<ITERATIONS; i++) {
        cout << "." << flush;
        MarkdownLexerSections lexer(fileName.get());
        lexer.tokenize();
        MarkdownParserSections parser(lexer);
        parser.parse();

        EXPECT_FALSE(parser.hasMetadata());
    }
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG(endl << (ITERATIONS*0.77) << "MiB (" << ITERATIONS << "x0.77MiB) MDs parsed in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms");
    MF_DEBUG(" ~ AVG: " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000000.0 << "ms" << endl);
}
