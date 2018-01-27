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
                cout << " " << lexems.at(i)->getLng();
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
        string* title{};
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
            title = section->getText();
            if(title!=nullptr) {
                cout << *title;
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
    dumpLexems(lexer.getLexems());
}

TEST(MarkdownParserTestCase, MarkdownParserSections)
{
    unique_ptr<string> fileName
            = unique_ptr<string>(new string{"/lib/test/resources/basic-repository/memory/outline.md"});
    //unique_ptr<string> fileName=new string{"/lib/test/resources/basic-repository/memory/no-metadata.md"};
    fileName.get()->insert(0, getMindforgerGitHomePath());

    // minimal MD
    cout << endl << "- Lexer ----------------------------------------------";
    MarkdownLexerSections lexer(fileName.get());
    lexer.tokenize();
    dumpLexems(lexer.getLexems());
    cout << endl << "- Parser ----------------------------------------------";
    MarkdownParserSections parser(lexer);
    parser.parse();
    dumpAst(parser.getAst());
    cout << endl << "- DONE ----------------------------------------------";
    cout << endl << "";
}

TEST(MarkdownParserTestCase, MarkdownRepresentation)
{
    string repository{"/lib/test/resources/basic-repository"};
    repository.insert(0, getMindforgerGitHomePath());

    m8r::Configuration configuration{repository};

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
        cout << endl << "    '" << note->getTitle() << "' (title)";
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

TEST(MarkdownParserTestCase, FileSystemPersistence)
{
    string repository{"/lib/test/resources/basic-repository"};
    repository.insert(0, getMindforgerGitHomePath());

    m8r::Configuration configuration{repository};
    m8r::Ontology ontology{configuration};
    m8r::MarkdownOutlineRepresentation mdr{ontology};
    m8r::FilesystemPersistence persistence{configuration,mdr};

    unique_ptr<string> text = unique_ptr<string>(new string{"abc"});
    cout << persistence.createFileName(string("/tmp"), text.get(), string(FILE_EXTENSION_MARKDOWN));
}

TEST(MarkdownParserBugsTestCase, MarkdownOnUbuntu)
{
    string repository{"/lib/test/resources/basic-repository"};
    repository.insert(0, getMindforgerGitHomePath());

    m8r::Configuration configuration{repository};
    m8r::Ontology ontology{configuration};

    m8r::MarkdownOutlineRepresentation mdr{ontology};
    string outlineFilename{"/lib/test/resources/bug-md-ubuntu/ubuntu-in.md"};
    outlineFilename.insert(0, getMindforgerGitHomePath());
    m8r::Outline* o = mdr.outline(outlineFilename);
    string* s = mdr.to(o);

    cout << endl << "-----------------------------------------------";
    cout << *s;
    cout << endl << "-----------------------------------------------";

    std::ofstream out("./tests/resources/bug-md-ubuntu/ubuntu-out.md");
    out << *s;
    out.close();

    delete o;
    delete s;

    cout << endl << "- DONE ----------------------------------------------";
}
