/*
 markdown_test.cpp     MindForger markdown test

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

#include "../../../src/representations/markdown/markdown_lexem.h"
#include "../../../src/representations/markdown/markdown_ast_node.h"
#include "../../../src/representations/markdown/markdown_lexer_sections.h"
#include "../../../src/representations/markdown/markdown_parser_sections.h"
#include "../../../src/representations/markdown/markdown_outline_representation.h"

#include "../../../src/config/configuration.h"
#include "../../../src/mind/ontology/ontology.h"
#include "../../../src/persistence/filesystem_persistence.h"

using namespace std;
using namespace m8r;

extern char* getMindforgerGitHomePath();

void dumpLexemType(MarkdownLexemType type)
{
    switch(type) {
    case MarkdownLexemType::BEGIN_DOC:
        cout << "BEGIN_DOC         ";
        break;
    case MarkdownLexemType::END_DOC:
        cout << "END_DOC           ";
        break;
    case MarkdownLexemType::SECTION:
        cout << "SECTION           ";
        break;
    case MarkdownLexemType::SECTION_equals:
        cout << "SECTION=           ";
        break;
    case MarkdownLexemType::SECTION_hyphens:
        cout << "SECTION-           ";
        break;
    case MarkdownLexemType::BR:
        cout << "BR                ";
        break;
    case MarkdownLexemType::LINE:
        cout << "LINE              ";
        break;
    case MarkdownLexemType::TEXT:
        cout << "TEXT              ";
        break;
    case MarkdownLexemType::WHITESPACES:
        cout << "WHITESPACES       ";
        break;
    case MarkdownLexemType::HTML_COMMENT_BEGIN:
        cout << "HTML_COMMENT_BEGIN";
        break;
    case MarkdownLexemType::HTML_COMMENT_END:
        cout << "HTML_COMMENT_END  ";
        break;

    case MarkdownLexemType::META_BEGIN:
        cout << "META_BEGIN        ";
        break;
    case MarkdownLexemType::META_PROPERTY_DELIMITER:
        cout << "META_PROP_DELIM   ";
        break;
    case MarkdownLexemType::META_NAMEVALUE_DELIMITER:
        cout << "META_NV_DELIM     ";
        break;

    case MarkdownLexemType::META_PROPERTY_created:
        cout << "META created     #";
        break;
    case MarkdownLexemType::META_PROPERTY_importance:
        cout << "META importance  #";
        break;
    case MarkdownLexemType::META_PROPERTY_modified:
        cout << "META modified    #";
        break;
    case MarkdownLexemType::META_PROPERTY_progress:
        cout << "META progress    #";
        break;
    case MarkdownLexemType::META_PROPERTY_read:
        cout << "META read        #";
        break;
    case MarkdownLexemType::META_PROPERTY_reads:
        cout << "META reads       #";
        break;
    case MarkdownLexemType::META_PROPERTY_revision:
        cout << "META revision    #";
        break;
    case MarkdownLexemType::META_PROPERTY_tags:
        cout << "META tags        #";
        break;
    case MarkdownLexemType::META_PROPERTY_type:
        cout << "META type        #";
        break;
    case MarkdownLexemType::META_PROPERTY_urgency:
        cout << "META urgency     #";
        break;

    case MarkdownLexemType::META_PROPERTY_VALUE:
        cout << "META_PROP_VALUE  >";
        break;

    case MarkdownLexemType::META_TEXT:
        cout << "META_TEXT         ";
        break;

    default:
        cout << (int)type;
        break;
    }
}

void dumpLexems(const vector<MarkdownLexem*>& lexems)
{
    cout << endl << "LEXEMs:";
    if(!lexems.empty()) {
        cout << " (" << lexems.size() << ")";
        for(unsigned long i=0; i<lexems.size(); ++i) {
            cout << endl << "  #" << i << " ";
            dumpLexemType(lexems.at(i)->getType());
            if(lexems.at(i)->getType() == MarkdownLexemType::SECTION) {
                cout << " " << lexems.at(i)->getDepth();
            } else {
                cout << " " << lexems.at(i)->getOff();
                cout << " " << lexems.at(i)->getIdx();
                cout << " " << (lexems.at(i)->getLng()==MarkdownLexem::WHOLE_LINE?"*":std::to_string(lexems.at(i)->getLng()));
            }
        }
    } else {
        cout << "  EMPTY";
    }
    cout << endl << "End of LEXEMs";
}

void dumpAst(const vector<MarkdownAstNodeSection*>* ast)
{
    cout << endl << "AST: ";
    if(ast) {
        cout << " (" << ast->size() << ")";
        string* name{};
        size_t c = 0;
        for(MarkdownAstNodeSection* section:*ast) {
            cout << endl << "  " << ++c << " #";
            cout << section->getDepth();
            if(section->getBody()) {
                cout << " d" << section->getBody()->size();
            } else {
                cout << " dNULL";
            }
            cout << " '";
            name = section->getText();
            if(name!=nullptr) {
                cout << *name;
            } else {
                cout << "NULL";
            }
            cout << "'";
            MarkdownAstSectionMetadata& meta = section->getMetadata();
            cout << "    / t: " << (meta.getType()?*meta.getType():"NULL");
            cout << " / c: " << meta.getCreated();
            cout << " / m: " << meta.getModified();
            cout << " / r: " << meta.getRead();
            cout << " / l: " << (meta.getPrimaryTag()==nullptr?"NULL":*(meta.getPrimaryTag()));
            cout << " / R: " << meta.getReads();
            cout << " / W: " << meta.getRevision();
            cout << " / *: '" << (int)meta.getImportance() << "'";
            cout << " / !: " << (int)meta.getUrgency();
            cout << " / %: " << (int)meta.getProgress();
            cout << " / t: ";
            if(meta.getTags().size()) {
                for(string* s:meta.getTags()) {
                    cout << *s << "|";
                }
            } else {
                cout << "EMPTY";
            }

        }

    } else {
        cout << "  EMPTY";
    }
    cout << endl << "End of AST";
}

TEST(MarkdownParserTestCase, MarkdownLexerSections)
{
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/basic-repository/memory/outline.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());
    MarkdownLexerSections lexer(fileName.get());

    // minimal MD
    lexer.tokenize();
    const std::vector<MarkdownLexem*>& lexems = lexer.getLexems();
    dumpLexems(lexems);

    // asserts
    EXPECT_EQ(lexems[0]->getType(), MarkdownLexemType::BEGIN_DOC);

    EXPECT_EQ(lexems[1]->getType(), MarkdownLexemType::SECTION);
    EXPECT_EQ(lexems[1]->getDepth(), 0);

    EXPECT_EQ(lexems[2]->getType(), MarkdownLexemType::WHITESPACES);
    EXPECT_EQ(lexems[3]->getType(), MarkdownLexemType::TEXT);
    EXPECT_EQ(lexems[3]->getOff(), 0);
    EXPECT_EQ(lexems[3]->getIdx(), 2);
    EXPECT_EQ(lexems[3]->getLng(), 9);
}

TEST(MarkdownParserTestCase, MarkdownLexerSectionsNoMetadata)
{
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/basic-repository/memory/no-metadata.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());
    MarkdownLexerSections lexer(fileName.get());

    // minimal MD
    lexer.tokenize();
    dumpLexems(lexer.getLexems());
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
    dumpLexems(lexems);

    // asserts
    EXPECT_EQ(lexems[0]->getType(), MarkdownLexemType::BEGIN_DOC);
    EXPECT_EQ(lexems[1]->getType(), MarkdownLexemType::LINE);
    EXPECT_EQ(lexems[2]->getType(), MarkdownLexemType::BR);
    EXPECT_EQ(lexems[3]->getType(), MarkdownLexemType::BR);
    EXPECT_EQ(lexems[4]->getType(), MarkdownLexemType::SECTION);
    EXPECT_EQ(lexems[4]->getDepth(), 0);
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
    dumpLexems(lexems);

    // asserts
    EXPECT_EQ(lexems[0]->getType(), MarkdownLexemType::BEGIN_DOC);
    EXPECT_EQ(lexems[1]->getType(), MarkdownLexemType::SECTION_equals);
    EXPECT_EQ(lexems[2]->getType(), MarkdownLexemType::LINE);
    EXPECT_EQ(lexems[3]->getType(), MarkdownLexemType::BR);
    EXPECT_EQ(lexems[4]->getType(), MarkdownLexemType::LINE);
    EXPECT_EQ(lexems[5]->getType(), MarkdownLexemType::BR);
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
    dumpLexems(lexems);

    // asserts
    EXPECT_EQ(lexems[0]->getType(), MarkdownLexemType::BEGIN_DOC);
    EXPECT_EQ(lexems[1]->getType(), MarkdownLexemType::SECTION_equals);
    EXPECT_EQ(lexems[2]->getType(), MarkdownLexemType::LINE);
    EXPECT_EQ(lexems[3]->getType(), MarkdownLexemType::BR);
    EXPECT_EQ(lexems[4]->getType(), MarkdownLexemType::LINE);
    EXPECT_EQ(lexems[5]->getType(), MarkdownLexemType::BR);
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
    dumpLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_TRUE(parser.hasMetadata());
    dumpAst(parser.getAst());
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
    dumpLexems(lexems);
    EXPECT_EQ(lexems.size(), 62);

    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_TRUE(!parser.hasMetadata());
    std::vector<MarkdownAstNodeSection*>* ast = parser.getAst();
    dumpAst(ast);
    EXPECT_EQ(ast->size(), 4);
    // preamble section
    EXPECT_TRUE(ast->at(0)->isPreambleSection());
    EXPECT_EQ(ast->at(0)->getText(), nullptr);

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
    dumpLexems(lexems);
    EXPECT_EQ(lexems.size(), 31);

    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_TRUE(!parser.hasMetadata());
    std::vector<MarkdownAstNodeSection*>* ast = parser.getAst();
    dumpAst(ast);
    EXPECT_EQ(ast->size(), 4);
    // preamble section
    EXPECT_TRUE(ast->at(0)->isPreambleSection());
    EXPECT_EQ(ast->at(0)->getText(), nullptr);

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
    dumpLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_TRUE(!parser.hasMetadata());
    dumpAst(parser.getAst());
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
    dumpLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_TRUE(parser.hasMetadata());
    dumpAst(parser.getAst());
    ASSERT_EQ(parser.getAst()->size(), 4);
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
    dumpLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_FALSE(parser.hasMetadata());
    dumpAst(parser.getAst());
    ASSERT_EQ(parser.getAst()->size(), 4);
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
    dumpLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    EXPECT_FALSE(parser.hasMetadata());
    dumpAst(parser.getAst());
    ASSERT_EQ(parser.getAst()->size(), 4);
    cout << endl << "- DONE ----------------------------------------------";
    cout << endl;
}

TEST(MarkdownParserTestCase, MarkdownRepresentation)
{
    string repositoryPath{"/lib/test/resources/basic-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());

    m8r::Configuration& configuration = m8r::Configuration::getInstance();
    configuration.setActiveRepository(configuration.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));

    m8r::Ontology ontology{configuration};

    string text =
            "## Canonical Message <!-- Metadata: tags: important; type: Goal; created: 2015-05-02 21:30:28; reads: 55; read: 2016-03-31 13:54:45; revision: 3; modified: 2016-03-31 13:54:45; importance: 3/5; urgency: 2/5; progress: 20%; -->\n"
            "\n"
            "This document contains ideas related to the definition of similarity codes for canonical messages.\n"
            "\n";

    cout << endl << "- MD > Note ----------------------------------------------";
    m8r::MarkdownOutlineRepresentation mdr{ontology};

    unique_ptr<Note> note = unique_ptr<Note>(mdr.note(&text));
    if(note) {
        cout << endl << "    '" << note->getName() << "' (name)";
        cout << endl << "    '" << note->getDepth() << "' (depth)";
        cout << endl << "    " << (note->getPrimaryTag()?note->getPrimaryTag()->getName():"NULL") << " (primary tag)";
        if(note->getTags().size()) {
            for(size_t t=0; t<note->getTags().size(); t++) {
                cout << endl << "      " << note->getTags()[t]->getName() << " (tag)";
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
    m8r::Configuration& configuration = m8r::Configuration::getInstance();
    configuration.setActiveRepository(configuration.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    m8r::Ontology ontology{configuration};

    // parse
    m8r::MarkdownOutlineRepresentation mdr{ontology};
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/apiary-repository/memory/01. Simplest API.md"});
    fileName.get()->insert(0, getMindforgerGitHomePath());
    File file{*fileName.get()};
    m8r::Outline* o = mdr.outline(file);

    // asserts
    EXPECT_NE(o, nullptr);
    EXPECT_EQ(o->getNotesCount(), 2);

    cout << endl << "- Preamble ---";
    EXPECT_EQ(o->getPreamble().size(), 2);
    cout << endl << "'" << *(o->getPreamble()[0]) << "'";
    cout << endl << "'" << *(o->getPreamble()[1]) << "'";
    EXPECT_EQ(*(o->getPreamble()[0]), "FORMAT: 1A");
    EXPECT_EQ(*(o->getPreamble()[1]), "");
    EXPECT_TRUE(o->isApiaryBlueprint());

    cout << endl << "- Outline ---";
    cout << endl << "'" << o->getName() << "'";
    EXPECT_EQ(o->getName(), "The Simplest API");
    EXPECT_EQ(o->getDescription().size(), 15);

    cout << endl << "- N[0] ---";
    cout << endl << "'" << o->getNotes()[0]->getName() << "'";
    EXPECT_EQ(o->getNotes()[0]->getName(), "API Blueprint");
    EXPECT_EQ(o->getNotes()[0]->getDescription().size(), 3);

    cout << endl << "- N[1] ---";
    cout << endl << "'" << o->getNotes()[1]->getName() << "'";
    EXPECT_EQ(o->getNotes()[1]->getName(), "GET /message");

    // preamble serialization check
    string* original = m8r::fileToString(o->getKey());
    string* serialized = mdr.to(o);
    EXPECT_EQ(*original, *serialized);

    cout << endl << "- DONE ----------------------------------------------" << endl;
    delete o;
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
    m8r::Configuration& configuration = m8r::Configuration::getInstance();
    configuration.setActiveRepository(configuration.addRepository(repository));
    m8r::Ontology ontology{configuration};

    // parse
    m8r::MarkdownOutlineRepresentation mdr{ontology};
    File file{filePath};
    m8r::Outline* o = mdr.outline(file);

    // asserts
    EXPECT_NE(o, nullptr);
    ASSERT_EQ(o->getNotesCount(), 4);

    cout << endl << "- O ---";
    cout << endl << "Name: '" << o->getName() << "'";
    EXPECT_EQ(o->getName(), "Outline Name");
    cout << endl << "Desc: '" << o->getDescriptionAsString() << "'";
    EXPECT_EQ(o->getDescription().size(), 2);

    EXPECT_EQ(o->getNotes()[0]->getName(), "First Section");
    EXPECT_EQ(o->getNotes()[0]->getDescription().size(), 2);
    EXPECT_EQ(o->getNotes()[1]->getName(), "Second Section");
    EXPECT_EQ(o->getNotes()[1]->getDescription().size(), 3);
    EXPECT_EQ(o->getNotes()[2]->getName(), "Note 2");
    EXPECT_EQ(o->getNotes()[2]->getDescription().size(), 3);
    EXPECT_EQ(o->getNotes()[3]->getName(), "Note 3");
    EXPECT_EQ(o->getNotes()[3]->getDescription().size(), 2);

    // serialize
    string* serialized = mdr.to(o);
    cout << endl << "- SERIALIZED ---";
    cout << endl << *serialized;
    EXPECT_EQ(content, *serialized);

    delete serialized;
}

// TODO support for trailing hashes to be implemented
TEST(MarkdownParserTestCase, DISABLED_MarkdownRepresentationSectionTrailingHashes)
{
    string repositoryPath{"/tmp"};
    string fileName{"md-trailing-hashes-section.md"};
    string content;
    string filePath{repositoryPath+"/"+fileName};

    content.assign(
        "Outline Name\n"
        "============\n"
        "O text.\n"
        "\n"
        "# First Section #\n"
        "N1 text.\n"
        "\n"
        "## Second Section ##\n"
        "\n"
        "N2 text.\n"
        "\n"
        "### Note 3 ###\n"
        "N3 text.\n"
        "\n");
    m8r::stringToFile(filePath, content);

    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repository->setMode(m8r::Repository::RepositoryMode::FILE);
    repository->setFile(fileName);
    m8r::Configuration& configuration = m8r::Configuration::getInstance();
    configuration.setActiveRepository(configuration.addRepository(repository));
    m8r::Ontology ontology{configuration};

    // parse
    m8r::MarkdownOutlineRepresentation mdr{ontology};
    File file{filePath};
    m8r::Outline* o = mdr.outline(file);

    // asserts
    EXPECT_NE(o, nullptr);
    ASSERT_EQ(o->getNotesCount(), 3);

    cout << endl << "- O ---";
    cout << endl << "Name: '" << o->getName() << "'";
    EXPECT_EQ(o->getName(), "Outline Name");
    cout << endl << "Desc: '" << o->getDescriptionAsString() << "'";
    EXPECT_EQ(o->getDescription().size(), 2);

    EXPECT_EQ(o->getNotes()[0]->getName(), "First Section");
    EXPECT_EQ(o->getNotes()[0]->getDescription().size(), 2);
    EXPECT_EQ(o->getNotes()[1]->getName(), "Second Section");
    EXPECT_EQ(o->getNotes()[1]->getDescription().size(), 3);
    EXPECT_EQ(o->getNotes()[2]->getName(), "Note 3");
    EXPECT_EQ(o->getNotes()[2]->getDescription().size(), 2);

    // serialize
    string* serialized = mdr.to(o);
    cout << endl << "- SERIALIZED ---";
    cout << endl << *serialized;
    EXPECT_EQ(content, *serialized);

    delete serialized;
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
    m8r::Configuration& configuration = m8r::Configuration::getInstance();
    configuration.setActiveRepository(configuration.addRepository(repository));
    m8r::Ontology ontology{configuration};

    // parse
    m8r::MarkdownOutlineRepresentation mdr{ontology};
    File file{filePath};
    m8r::Outline* o = mdr.outline(file);

    // asserts
    EXPECT_NE(o, nullptr);
    EXPECT_EQ(o->getNotesCount(), 2);

    cout << endl << "- Preamble ---";
    EXPECT_EQ(o->getPreamble().size(), 3);
    cout << endl << "'" << *(o->getPreamble()[0]) << "'";
    cout << endl << "'" << *(o->getPreamble()[1]) << "'";
    cout << endl << "'" << *(o->getPreamble()[2]) << "'";
    EXPECT_EQ(*(o->getPreamble()[0]), "");
    EXPECT_EQ(*(o->getPreamble()[1]), "");
    EXPECT_EQ(*(o->getPreamble()[2]), "");
    EXPECT_TRUE(!o->isApiaryBlueprint());

    cout << endl << "- Outline ---";
    cout << endl << "'" << o->getName() << "'";
    EXPECT_EQ(o->getName(), "First Markdown");
    EXPECT_EQ(o->getDescription().size(), 2);
}

TEST(MarkdownParserTestCase, FileSystemPersistence)
{
    string repositoryPath{"/lib/test/resources/basic-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    m8r::Ontology ontology{config};
    m8r::MarkdownOutlineRepresentation mdr{ontology};
    m8r::FilesystemPersistence persistence{config,mdr};

    unique_ptr<string> text = unique_ptr<string>(new string{"abc"});
    cout << persistence.createFileName(string("/tmp"), text.get(), string(FILE_EXTENSION_MARKDOWN));
}

TEST(MarkdownParserBugsTestCase, EmptyNameSkipsEof)
{
    string repositoryPath{"/lib/test/resources/bugs-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath)));
    m8r::Ontology ontology{config};

    m8r::MarkdownOutlineRepresentation mdr{ontology};
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
