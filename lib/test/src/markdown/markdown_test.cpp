/*
 markdown_test.cpp     MindForger markdown test

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

#include <cstdlib>
#include <iostream>
#include <memory>
#include <cstdio>
#ifndef _WIN32
#  include <unistd.h>
#endif

#include <gtest/gtest.h>

#include "../../../src/representations/markdown/markdown_lexem.h"
#include "../../../src/representations/markdown/markdown_ast_node.h"
#include "../../../src/representations/markdown/markdown_lexer_sections.h"
#include "../../../src/representations/markdown/markdown_parser_sections.h"
#include "../../../src/representations/markdown/markdown_outline_representation.h"

#include "../../../src/config/configuration.h"
#include "../../../src/mind/ontology/ontology.h"
#include "../../../src/mind/ai/autolinking_preprocessor.h"
#include "../../../src/persistence/filesystem_persistence.h"

#include "../test_gear.h"

using namespace std;
using namespace m8r;

extern char* getMindforgerGitHomePath();

TEST(MarkdownParserTestCase, MarkdownLexerSections)
{
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/basic-repository/memory/outline.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());
    MarkdownLexerSections lexer(fileName.get());

    // minimal MD
    lexer.tokenize();
    const std::vector<MarkdownLexem*>& lexems = lexer.getLexems();
    printLexems(lexems);

    // asserts
    EXPECT_EQ(MarkdownLexemType::BEGIN_DOC, lexems[0]->getType());

    EXPECT_EQ(MarkdownLexemType::SECTION, lexems[1]->getType());
    EXPECT_EQ(0, lexems[1]->getDepth());

    EXPECT_EQ(MarkdownLexemType::WHITESPACES, lexems[2]->getType());
    EXPECT_EQ(MarkdownLexemType::TEXT, lexems[3]->getType());
    EXPECT_EQ(0, lexems[3]->getOff());
    EXPECT_EQ(2, lexems[3]->getIdx());
    EXPECT_EQ(9, lexems[3]->getLng());
}

TEST(MarkdownParserTestCase, MarkdownLexerSectionsNoMetadata)
{
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/basic-repository/memory/no-metadata.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());
    MarkdownLexerSections lexer(fileName.get());

    // minimal MD
    lexer.tokenize();
    printLexems(lexer.getLexems());
}

TEST(MarkdownParserTestCase, MarkdownLexerSectionsPreamble)
{
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/apiary-repository/memory/01. Simplest API.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());
    MarkdownLexerSections lexer(fileName.get());

    // tokenize
    lexer.tokenize();
    const std::vector<MarkdownLexem*>& lexems = lexer.getLexems();
    printLexems(lexems);

    // asserts
    EXPECT_EQ(MarkdownLexemType::BEGIN_DOC, lexems[0]->getType());
    EXPECT_EQ(MarkdownLexemType::LINE, lexems[1]->getType());
    EXPECT_EQ(MarkdownLexemType::BR, lexems[2]->getType());
    EXPECT_EQ(MarkdownLexemType::BR, lexems[3]->getType());
    EXPECT_EQ(MarkdownLexemType::SECTION, lexems[4]->getType());
    EXPECT_EQ(0, lexems[4]->getDepth());
}

TEST(MarkdownParserTestCase, MarkdownLexerSectionsPostDeclaredHeaders)
{
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/bugs-repository/memory/feature-9-post-declared-headers.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());
    MarkdownLexerSections lexer(fileName.get());

    // tokenize
    lexer.tokenize();
    const std::vector<MarkdownLexem*>& lexems = lexer.getLexems();
    printLexems(lexems);

    // asserts
    EXPECT_EQ(MarkdownLexemType::BEGIN_DOC, lexems[0]->getType());
    EXPECT_EQ(MarkdownLexemType::SECTION_equals, lexems[1]->getType());
    EXPECT_EQ(MarkdownLexemType::LINE, lexems[2]->getType());
    EXPECT_EQ(MarkdownLexemType::BR, lexems[3]->getType());
    EXPECT_EQ(MarkdownLexemType::LINE, lexems[4]->getType());
    EXPECT_EQ(MarkdownLexemType::BR, lexems[5]->getType());
}

TEST(MarkdownParserTestCase, MarkdownLexerSectionsPostDeclaredHeaders2)
{
    string content;
    content.assign(
        "Outline Name\n"
        "========\n"
        "O text.\n"
        "\n"
        "First Section\n"
        "-------------\n"
        "N1 text.\n"
        "\n"
        "## Second Section\n"
        "\n"
        "N2 text.\n"
        "\n"
        "Note 3\n"
        "-------------\n"
        "N2 text.\n"
        "\n");

    MarkdownLexerSections lexer(nullptr);

    // tokenize
    lexer.tokenize(&content);
    const std::vector<MarkdownLexem*>& lexems = lexer.getLexems();
    ASSERT_TRUE(lexems.size());
    printLexems(lexems);

    // asserts
    EXPECT_EQ(MarkdownLexemType::BEGIN_DOC, lexems[0]->getType());
    EXPECT_EQ(MarkdownLexemType::SECTION_equals, lexems[1]->getType());
    EXPECT_EQ(MarkdownLexemType::LINE, lexems[2]->getType());
    EXPECT_EQ(MarkdownLexemType::BR, lexems[3]->getType());
    EXPECT_EQ(MarkdownLexemType::LINE, lexems[4]->getType());
    EXPECT_EQ(MarkdownLexemType::BR, lexems[5]->getType());
}

TEST(MarkdownParserTestCase, MarkdownLexerTimeScope)
{
    string content;
    content.assign(
        "# Outline Name <!-- Metadata: scope: 1y2m3d4h5m; -->\n"
        "O text.\n"
        "\n"
        "## First Section\n"
        "N1 text.\n"
        "\n"
        "## Second Section\n"
        "N2 text.\n"
        "\n");

    MarkdownLexerSections lexer(nullptr);

    // tokenize
    lexer.tokenize(&content);
    const std::vector<MarkdownLexem*>& lexems = lexer.getLexems();
    ASSERT_TRUE(lexems.size());
    printLexems(lexems);

    // asserts
    EXPECT_EQ(MarkdownLexemType::BEGIN_DOC, lexems[0]->getType());
    EXPECT_EQ(MarkdownLexemType::META_PROPERTY_scope, lexems[9]->getType());
}

TEST(MarkdownParserTestCase, MarkdownLexerLinks)
{
    string content;
    content.assign(
                "# Outline Name <!-- Metadata: links: [same as](./o1.md); -->\n"
                "O text.\n"
                "\n"
                "## First Section <!-- Metadata: links: [opposite of](./x.md),[is a](./y.md#a-z); -->\n"
                "N1 text.\n"
                "\n"
                "## Second Section\n"
                "N2 text.\n"
                "\n");

    MarkdownLexerSections lexer(nullptr);

    // tokenize
    lexer.tokenize(&content);
    const std::vector<MarkdownLexem*>& lexems = lexer.getLexems();
    ASSERT_TRUE(lexems.size());
    printLexems(lexems);

    // asserts
    EXPECT_EQ(MarkdownLexemType::BEGIN_DOC, lexems[0]->getType());
    EXPECT_EQ(MarkdownLexemType::META_PROPERTY_links, lexems[9]->getType());
}

TEST(MarkdownParserTestCase, MarkdownParserSections)
{
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/basic-repository/memory/outline.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());

    // minimal MD
    cout << endl << "- Lexer ----------------------------------------------";
    MarkdownLexerSections lexer(fileName.get());
    lexer.tokenize();
    printLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_FALSE(parser.hasMetadata());
    printAst(parser.getAst());
    cout << endl << "- DONE ----------------------------------------------";
    cout << endl;
}

TEST(MarkdownParserTestCase, MarkdownParserSectionsPreamble)
{
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/apiary-repository/memory/01. Simplest API.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());

    // minimal MD
    cout << endl << "- Lexer ----------------------------------------------";
    MarkdownLexerSections lexer(fileName.get());
    lexer.tokenize();
    const std::vector<MarkdownLexem*>& lexems = lexer.getLexems();
    printLexems(lexems);
    EXPECT_EQ(62, lexems.size());

    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_TRUE(!parser.hasMetadata());
    std::vector<MarkdownAstNodeSection*>* ast = parser.getAst();
    printAst(ast);
    EXPECT_EQ(4, ast->size());
    // preamble section
    EXPECT_TRUE(ast->at(0)->isPreambleSection());
    EXPECT_EQ(nullptr, ast->at(0)->getText());

    cout << endl << "- DONE ----------------------------------------------";
    cout << endl;
}

TEST(MarkdownParserTestCase, MarkdownParserSectionsEmptyFirstLine)
{
    string repositoryPath{"/tmp"};
    string fileName{"md-parser-first-empty-line-file.md"};
    string content;
    string filePath{repositoryPath+"/"+fileName};

    content.assign(
        "\n"
        "\n"
        "\n"
        "# First Markdown"
        "\nFirst MD text."
        "\n"
        "\n## Note 1"
        "\nNote 1 text."
        "\n"
        "\n## Note 2"
        "\nNote 2 text."
        "\n");
    m8r::stringToFile(filePath, content);

    // minimal MD
    cout << endl << "- Lexer ----------------------------------------------";
    MarkdownLexerSections lexer(&filePath);
    lexer.tokenize();
    const std::vector<MarkdownLexem*>& lexems = lexer.getLexems();
    printLexems(lexems);
    EXPECT_EQ(31, lexems.size());

    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_TRUE(!parser.hasMetadata());
    std::vector<MarkdownAstNodeSection*>* ast = parser.getAst();
    printAst(ast);
    EXPECT_EQ(4, ast->size());
    // preamble section
    EXPECT_TRUE(ast->at(0)->isPreambleSection());
    EXPECT_EQ(nullptr, ast->at(0)->getText());

    cout << endl << "- DONE ----------------------------------------------";
    cout << endl;
}

TEST(MarkdownParserTestCase, MarkdownParserSectionsNoMetadata)
{
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/basic-repository/memory/no-metadata.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());

    // minimal MD
    cout << endl << "- Lexer ----------------------------------------------";
    MarkdownLexerSections lexer(fileName.get());
    lexer.tokenize();
    printLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_TRUE(!parser.hasMetadata());
    printAst(parser.getAst());
    cout << endl << "- DONE ----------------------------------------------";
    cout << endl;
}

TEST(MarkdownParserTestCase, Bug37Meta)
{
    string fileName{"/lib/test/resources/bugs-repository/memory/bug-37-meta.md"};
    fileName.insert(0, getMindforgerGitHomePath());
    cout << endl << "- Lexer ----------------------------------------------";
    MarkdownLexerSections lexer(&fileName);
    lexer.tokenize();
    printLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_TRUE(parser.hasMetadata());
    printAst(parser.getAst());
    ASSERT_EQ(4, parser.getAst()->size());
    cout << endl << "- DONE ----------------------------------------------";
    cout << endl;
}

TEST(MarkdownParserTestCase, Bug37Nometa)
{
    string fileName{"/lib/test/resources/bugs-repository/memory/bug-37-nometa.md"};
    fileName.insert(0, getMindforgerGitHomePath());
    cout << endl << "- Lexer ----------------------------------------------";
    MarkdownLexerSections lexer(&fileName);
    lexer.tokenize();
    printLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_FALSE(parser.hasMetadata());
    printAst(parser.getAst());
    ASSERT_EQ(4, parser.getAst()->size());
    cout << endl << "- DONE ----------------------------------------------";
    cout << endl;
}

TEST(MarkdownParserTestCase, Bug37Notrailing)
{
    string fileName{"/lib/test/resources/bugs-repository/memory/bug-37-notrailing.md"};
    fileName.insert(0, getMindforgerGitHomePath());
    cout << endl << "- Lexer ----------------------------------------------";
    MarkdownLexerSections lexer(&fileName);
    lexer.tokenize();
    printLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_FALSE(parser.hasMetadata());
    printAst(parser.getAst());
    ASSERT_EQ(4, parser.getAst()->size());
    cout << endl << "- DONE ----------------------------------------------";
    cout << endl;
}

TEST(MarkdownParserTestCase, MarkdownRepresentation)
{
    string repositoryPath{"/lib/test/resources/basic-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mptc-mr.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));

    m8r::Ontology ontology{};

    string text =
            "## Canonical Message <!-- Metadata: tags: important; type: Goal; created: 2015-05-02 21:30:28; reads: 55; read: 2016-03-31 13:54:45; revision: 3; modified: 2016-03-31 13:54:45; importance: 3/5; urgency: 2/5; progress: 20%; -->\n"
            "\n"
            "This document contains ideas related to the definition of similarity codes for canonical messages.\n"
            "\n";

    cout << endl << "- MD > Note ----------------------------------------------";
    m8r::MarkdownOutlineRepresentation mdr{ontology, nullptr};

    unique_ptr<Note> note = unique_ptr<Note>(mdr.note(&text));
    if(note) {
        cout << endl << "    '" << note->getName() << "' (name)";
        cout << endl << "    '" << note->getDepth() << "' (depth)";
        cout << endl << "    " << (note->getPrimaryTag()?note->getPrimaryTag()->getName():"NULL") << " (primary tag)";
        if(note->getTags()->size()) {
            for(size_t t=0; t<note->getTags()->size(); t++) {
                cout << endl << "      " << (*note->getTags())[t]->getName() << " (tag)";
            }
        }
        cout << endl << "    " << (note->getType()?note->getType()->getName():"NULL") << " (type)";
        cout << endl << "      Description[" << note->getDescription().size() << "]:";
        for(string* description:note->getDescription()) {
            cout << endl << "        '" << *description << "' (description)";
        }
        cout << endl << "  " << note->getCreated() << " (created)";
        cout << endl << "  " << note->getModified() << " (modified)";
        cout << endl << "  " << note->getRevision() << " (revision)";
        cout << endl << "  " << note->getReads() << " (reads)";
        cout << endl << "  " << note->getRead() << " (read)";
    } else {
        cout << endl << "Note is NULL!";
    }

    cout << endl << "- DONE ----------------------------------------------";
}

TEST(MarkdownParserTestCase, MarkdownRepresentationPreamble)
{
    string repositoryPath{"/lib/test/resources/apiary-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mptc-mrp.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    m8r::Ontology ontology{};

    // parse
    m8r::MarkdownOutlineRepresentation mdr{ontology, nullptr};
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/apiary-repository/memory/01. Simplest API.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());
    File file{*fileName.get()};
    m8r::Outline* o = mdr.outline(file);

    // asserts
    EXPECT_NE(nullptr, o);
    EXPECT_EQ(2, o->getNotesCount());

    cout << endl << "- Preamble ---";
    EXPECT_EQ(2, o->getPreamble().size());
    cout << endl << "'" << *(o->getPreamble()[0]) << "'";
    cout << endl << "'" << *(o->getPreamble()[1]) << "'";
    EXPECT_EQ("FORMAT: 1A", *(o->getPreamble()[0]));
    EXPECT_EQ("", *(o->getPreamble()[1]));
    EXPECT_TRUE(o->isApiaryBlueprint());

    cout << endl << "- Outline ---";
    cout << endl << "'" << o->getName() << "'";
    EXPECT_EQ("The Simplest API", o->getName());
    EXPECT_EQ(15, o->getDescription().size());

    cout << endl << "- N[0] ---";
    cout << endl << "'" << o->getNotes()[0]->getName() << "'";
    EXPECT_EQ("API Blueprint", o->getNotes()[0]->getName());
    EXPECT_EQ(3, o->getNotes()[0]->getDescription().size());

    cout << endl << "- N[1] ---";
    cout << endl << "'" << o->getNotes()[1]->getName() << "'";
    EXPECT_EQ("GET /message", o->getNotes()[1]->getName());

    // preamble serialization check
    string* original = m8r::fileToString(o->getKey());
    string* serialized = mdr.to(o);
    EXPECT_EQ(*original, *serialized);

    delete original;
    delete serialized;
    delete o;
    cout << endl << "- DONE ----------------------------------------------" << endl;
}

TEST(MarkdownParserTestCase, MarkdownRepresentationPostDeclaredSection)
{
    string repositoryPath{"/tmp"};
    string fileName{"md-post-declared-section.md"};
    string content;
    string filePath{repositoryPath+"/"+fileName};

    content.assign(
        "Outline Name\n"
        "============\n"
        "O text.\n"
        "\n"
        "First Section\n"
        "-------------\n"
        "N1 text.\n"
        "\n"
        "## Second Section\n"
        "\n"
        "N2 text.\n"
        "\n"
        "Note 2\n"
        "------\n"
        "\n"
        "N2 text.\n"
        "\n"
        "Note 3\n"
        "------\n"
        "N3 text.\n"
        "\n");
    m8r::stringToFile(filePath, content);

    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repository->setMode(m8r::Repository::RepositoryMode::FILE);
    repository->setFile(fileName);
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mptc-mrpdc.md");
    config.setActiveRepository(config.addRepository(repository));
    m8r::Ontology ontology{};

    // parse
    m8r::MarkdownOutlineRepresentation mdr{ontology, nullptr};
    File file{filePath};
    m8r::Outline* o = mdr.outline(file);

    // asserts
    EXPECT_NE(nullptr, o);
    ASSERT_EQ(4, o->getNotesCount());

    cout << endl << "- O ---";
    cout << endl << "Name: '" << o->getName() << "'";
    EXPECT_EQ("Outline Name", o->getName());
    cout << endl << "Desc: '" << o->getDescriptionAsString() << "'";
    EXPECT_EQ(2, o->getDescription().size());

    EXPECT_EQ("First Section", o->getNotes()[0]->getName());
    EXPECT_EQ(2, o->getNotes()[0]->getDescription().size());
    EXPECT_EQ("Second Section", o->getNotes()[1]->getName());
    EXPECT_EQ(3, o->getNotes()[1]->getDescription().size());
    EXPECT_EQ("Note 2", o->getNotes()[2]->getName());
    EXPECT_EQ(3, o->getNotes()[2]->getDescription().size());
    EXPECT_EQ("Note 3", o->getNotes()[3]->getName());
    EXPECT_EQ(2, o->getNotes()[3]->getDescription().size());

    // serialize
    string* serialized = mdr.to(o);
    cout << endl << "- SERIALIZED ---";
    cout << endl << *serialized;
    EXPECT_EQ(content, *serialized);

    delete serialized;
    delete o;
}

TEST(MarkdownParserTestCase, MarkdownRepresentationTrailingHashesSection)
{
    string repositoryPath{"/tmp"};
    string fileName{"md-trailing-hashes-section.md"};
    string content;
    string filePath{repositoryPath+"/"+fileName};

    content.assign(
        "# Outline Name #\n"
        "O text.\n"
        "\n"
        "# First Section #\n"
        "N1 text.\n"
        "\n"
        "# WRONG 1 ##\n"
        "W1 text.\n"
        "\n"
        "## Second Section ##\n"
        "N2 text.\n"
        "\n"
        "## WRONG 2 #\n"
        "W2 text.\n"
        "\n"
        "### Note 3 ###\n"
        "N3 text.\n"
        "\n");
    m8r::stringToFile(filePath, content);

    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repository->setMode(m8r::Repository::RepositoryMode::FILE);
    repository->setFile(fileName);
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mptc-mrthc.md");
    config.setActiveRepository(config.addRepository(repository));
    m8r::Ontology ontology{};

    // parse
    m8r::MarkdownOutlineRepresentation mdr{ontology, nullptr};
    File file{filePath};
    m8r::Outline* o = mdr.outline(file);

    // asserts
    EXPECT_NE(nullptr, o);
    ASSERT_EQ(5, o->getNotesCount());

    cout << endl << "- O ---";
    cout << endl << "Name: '" << o->getName() << "'";
    EXPECT_EQ("Outline Name", o->getName());
    cout << endl << "Desc: '" << o->getDescriptionAsString() << "'";
    EXPECT_EQ(false, o->isPostDeclaredSection());
    EXPECT_EQ(true, o->isTrailingHashesSection());

    EXPECT_EQ("First Section", o->getNotes()[0]->getName());
    EXPECT_EQ(false, o->getNotes()[0]->isPostDeclaredSection());
    EXPECT_EQ(true, o->getNotes()[0]->isTrailingHashesSection());
    EXPECT_EQ(2, o->getNotes()[0]->getDescription().size());

    EXPECT_EQ("WRONG 1 ##", o->getNotes()[1]->getName());
    EXPECT_EQ(false, o->getNotes()[1]->isPostDeclaredSection());
    EXPECT_EQ(false, o->getNotes()[1]->isTrailingHashesSection());
    EXPECT_EQ(2, o->getNotes()[1]->getDescription().size());

    EXPECT_EQ("Second Section", o->getNotes()[2]->getName());
    EXPECT_EQ(false, o->getNotes()[2]->isPostDeclaredSection());
    EXPECT_EQ(true, o->getNotes()[2]->isTrailingHashesSection());
    EXPECT_EQ(2, o->getNotes()[2]->getDescription().size());

    EXPECT_EQ("WRONG 2 #", o->getNotes()[3]->getName());
    EXPECT_EQ(false, o->getNotes()[3]->isPostDeclaredSection());
    EXPECT_EQ(false, o->getNotes()[3]->isTrailingHashesSection());
    EXPECT_EQ(2, o->getNotes()[3]->getDescription().size());

    EXPECT_EQ("Note 3", o->getNotes()[4]->getName());
    EXPECT_EQ(false, o->getNotes()[4]->isPostDeclaredSection());
    EXPECT_EQ(true, o->getNotes()[4]->isTrailingHashesSection());
    EXPECT_EQ(2, o->getNotes()[4]->getDescription().size());

    // serialize
    string* serialized = mdr.to(o);
    cout << endl << "- SERIALIZED ---";
    cout << endl << *serialized;
    EXPECT_EQ(content, *serialized);

    delete serialized;
    delete o;
}

TEST(MarkdownParserTestCase, MarkdownRepresentationEmptyFirstLine)
{
    string repositoryPath{"/tmp"};
    string fileName{"md-parser-first-empty-line-file.md"};
    string content;
    string filePath{repositoryPath+"/"+fileName};

    content.assign(
        "\n"
        "\n"
        "\n"
        "# First Markdown"
        "\nFirst MD text."
        "\n"
        "\n## Note 1"
        "\nNote 1 text."
        "\n"
        "\n## Note 2"
        "\nNote 2 text."
        "\n");
    m8r::stringToFile(filePath, content);

    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repository->setMode(m8r::Repository::RepositoryMode::FILE);
    repository->setFile(fileName);
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mptc-mrefl.md");
    config.setActiveRepository(config.addRepository(repository));
    m8r::Ontology ontology{};

    // parse
    m8r::MarkdownOutlineRepresentation mdr{ontology, nullptr};
    File file{filePath};
    m8r::Outline* o = mdr.outline(file);

    // asserts
    EXPECT_NE(nullptr, o);
    EXPECT_EQ(2, o->getNotesCount());

    cout << endl << "- Preamble ---";
    EXPECT_EQ(3, o->getPreamble().size());
    cout << endl << "'" << *(o->getPreamble()[0]) << "'";
    cout << endl << "'" << *(o->getPreamble()[1]) << "'";
    cout << endl << "'" << *(o->getPreamble()[2]) << "'";
    EXPECT_EQ("", *(o->getPreamble()[0]));
    EXPECT_EQ("", *(o->getPreamble()[1]));
    EXPECT_EQ("", *(o->getPreamble()[2]));
    EXPECT_TRUE(!o->isApiaryBlueprint());

    cout << endl << "- Outline ---";
    cout << endl << "'" << o->getName() << "'";
    EXPECT_EQ("First Markdown", o->getName());
    EXPECT_EQ(2, o->getDescription().size());

    delete o;
}

TEST(MarkdownParserTestCase, FileSystemPersistence)
{
    string repositoryPath{"/lib/test/resources/basic-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mptc-fsp.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    m8r::Ontology ontology{};
    m8r::MarkdownOutlineRepresentation mdr{ontology, nullptr};
    m8r::HtmlOutlineRepresentation htmlr{ontology, nullptr};
    m8r::FilesystemPersistence persistence{mdr, htmlr};

    unique_ptr<string> text = unique_ptr<string>(new string{"abc"});
    cout << persistence.createFileName(string("/tmp"), text.get(), string(FILE_EXTENSION_MD_MD));
}

TEST(MarkdownParserBugsTestCase, EmptyNameSkipsEof)
{
    string repositoryPath{"/lib/test/resources/bugs-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mptc-ense.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    m8r::Ontology ontology{};

    m8r::MarkdownOutlineRepresentation mdr{ontology, nullptr};
    string outlineFilename{"/lib/test/resources/bugs-repository/memory/bug-37.md"};
    outlineFilename.insert(0, getMindforgerGitHomePath());
    m8r::Outline* o = mdr.outline(outlineFilename);
    string* s = mdr.to(o);

    cout << endl << "-----------------------------------------------";
    cout << *s;
    cout << endl << "-----------------------------------------------";

    // TODO asserts

    std::ofstream out("./tests/resources/bugs-repository/memory/bug-37-PARSER-OUTPUT.md");
    out << *s;
    out.close();

    delete o;
    delete s;

    cout << endl << "- DONE ----------------------------------------------";
}

TEST(MarkdownParserTestCase, TimeScope)
{
    string repositoryPath{"/tmp"};
    string fileName{"md-parser-time-scope.md"};
    string content;
    string filePath{repositoryPath+"/"+fileName};

    content.assign(
                "# Outline Name <!-- Metadata: scope: 1y2m3d4h5m; -->\n"
                "O text.\n"
                "\n"
                "## First Section\n"
                "N1 text.\n"
                "\n"
                "## Second Section\n"
                "N2 text.\n"
                "\n");
    m8r::stringToFile(filePath, content);

    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repository->setMode(m8r::Repository::RepositoryMode::FILE);
    repository->setFile(fileName);
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mptc-ts.md");
    config.setActiveRepository(config.addRepository(repository));
    m8r::Ontology ontology{};

    // parse
    m8r::MarkdownOutlineRepresentation mdr{ontology, nullptr};
    File file{filePath};
    m8r::Outline* o = mdr.outline(file);

    // asserts
    EXPECT_NE(nullptr, o);
    string timeScopeAsString{};
    o->getTimeScope().toString(timeScopeAsString);
    cout << endl << "Time scope: " << timeScopeAsString << " (" << o->getTimeScope().relativeSecs << "s)";
    EXPECT_EQ(36993900, o->getTimeScope().relativeSecs);
    o->getTimeScope().toString(timeScopeAsString);
    EXPECT_EQ("1y2m3d4h5m", timeScopeAsString);
    EXPECT_EQ(2, o->getNotesCount());

    // serialize
    string* serialized = mdr.to(o);
    cout << endl << "- SERIALIZED ---";
    cout << endl << *serialized;
    EXPECT_NE(std::string::npos, serialized->find("scope: 1y2m3d4h5m;"));

    delete serialized;
    delete o;
}

TEST(MarkdownParserTestCase, Deadline)
{
    string repositoryPath{"/tmp"};
    string fileName{"md-parser-deadline.md"};
    string content;
    string filePath{repositoryPath+"/"+fileName};

    content.assign(
                "# Outline Name\n"
                "O text.\n"
                "\n"
                "## First Section  <!-- Metadata: deadline: 2010-11-12 13:14:15; -->\n"
                "N1 text.\n"
                "\n"
                "## Second Section\n"
                "N2 text.\n"
                "\n");
    m8r::stringToFile(filePath, content);

    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repository->setMode(m8r::Repository::RepositoryMode::FILE);
    repository->setFile(fileName);
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mptc-d.md");
    config.setActiveRepository(config.addRepository(repository));
    m8r::Ontology ontology{};

    // parse
    m8r::MarkdownOutlineRepresentation mdr{ontology, nullptr};
    File file{filePath};
    m8r::Outline* o = mdr.outline(file);

    // asserts
    EXPECT_NE(nullptr, o);
    EXPECT_EQ(2, o->getNotesCount());

    cout << endl << "Deadline: " << o->getNotes()[0]->getDeadline();
#ifdef _WIN32
    tm deadLineDate = { 15,14,13,12,10,110, 0, 0, 0 };
#else
    tm deadLineDate = { 15,14,13,12,10,110, 0, 0, 0, 0, 0 };
#endif // _WIN32
    time_t deadLine = mktime(&deadLineDate);
    EXPECT_EQ(deadLine, o->getNotes()[0]->getDeadline());

    // serialize
    string* serialized = mdr.to(o);
    cout << endl << "- SERIALIZED ---";
    cout << endl << *serialized;
    EXPECT_NE(std::string::npos, serialized->find("deadline: 2010-11-12 13:14:15"));

    delete serialized;
    delete o;
}

TEST(MarkdownParserTestCase, Links)
{
    string repositoryPath{"/tmp"};
    string fileName{"md-parser-deadline.md"};
    string content;
    string filePath{repositoryPath+"/"+fileName};

    content.assign(
                "# Outline Name <!-- Metadata: links: [same as](./o1.md); -->\n"
                "O text.\n"
                "\n"
                "## First Section <!-- Metadata: links: [opposite of](./x.md),[is a](./y.md#a-z); -->\n"
                "N1 text.\n"
                "\n"
                "## Second Section\n"
                "N2 text.\n"
                "\n");
    m8r::stringToFile(filePath, content);

    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repository->setMode(m8r::Repository::RepositoryMode::FILE);
    repository->setFile(fileName);
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mptc-l.md");
    config.setActiveRepository(config.addRepository(repository));
    m8r::Ontology ontology{};

    // parse
    m8r::MarkdownOutlineRepresentation mdr{ontology, nullptr};
    File file{filePath};
    m8r::Outline* o = mdr.outline(file);

    // asserts
    EXPECT_NE(nullptr, o);
    EXPECT_EQ(2, o->getNotesCount());

    cout << endl << "O links: " << o->getLinksCount();
    EXPECT_EQ(1, o->getLinksCount());

    cout << endl << "N links: " << o->getNotes()[0]->getLinksCount();
    EXPECT_EQ(2, o->getNotes()[0]->getLinksCount());

    // serialize
    string* serialized = mdr.to(o);
    cout << endl << "- SERIALIZED ---";
    cout << endl << *serialized;
    EXPECT_NE(std::string::npos, serialized->find("[same as](./o1.md)"));
    EXPECT_NE(std::string::npos, serialized->find("[opposite of](./x.md)"));
    EXPECT_NE(std::string::npos, serialized->find("[is a](./y.md#a-z)"));

    delete serialized;
    delete o;
}

TEST(MarkdownParserTestCase, Bug622Loop64kLinesOverflow)
{
    string fileName{"/lib/test/resources/bugs-repository/memory/bug-622-70k-lines.md"};
    fileName.insert(0, getMindforgerGitHomePath());
    cout << endl << "- Lexer ----------------------------------------------";
    MarkdownLexerSections lexer(&fileName);
    lexer.tokenize();
    // file too big - do NOT print: printLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    // file too big - do NOT print: printAst(parser.getAst());
    ASSERT_EQ(71234, parser.getAst()->size());
    cout << endl << "- DONE ----------------------------------------------";
    cout << endl;
}
