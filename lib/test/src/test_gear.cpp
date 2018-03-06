/*
 test_gear.cpp     MindForger test utils and gears

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

#include "test_gear.h"

namespace m8r {

using namespace std;

void printOutlineNotes(Outline* o)
{
    if(o) {
        cout << "Outline: " << o->getName() << endl;
        for(unsigned int i=0; i<o->getNotes().size(); i++) {
            cout << "  " << o->getNotes()[i]->getName() << endl;
        }
    } else {
        cout << "Outline is NULL" << endl;
    }
}

void createEmptyRepository(string& repositoryDir, map<string,string>& pathToContent)
{
    // clean
    removeDirectoryRecursively(repositoryDir.c_str());
    // create empty repo
    Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    // create content (path is expected to be absolute)
    if(pathToContent.size()) {
        for(auto& i:pathToContent) {
            stringToFile(i.first, i.second);
        }
    }
}

void printLexemType(MarkdownLexemType type)
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
    case MarkdownLexemType::META_PROPERTY_deadline:
        cout << "META deadline    #";
        break;
    case MarkdownLexemType::META_PROPERTY_links:
        cout << "META links       #";
        break;
    case MarkdownLexemType::META_PROPERTY_forget:
        cout << "META forget      #";
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

void printLexems(const vector<MarkdownLexem*>& lexems)
{
    cout << endl << "LEXEMs:";
    if(!lexems.empty()) {
        cout << " (" << lexems.size() << ")";
        for(unsigned long i=0; i<lexems.size(); ++i) {
            cout << endl << "  #" << i << " ";
            printLexemType(lexems.at(i)->getType());
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

void printAst(const vector<MarkdownAstNodeSection*>* ast)
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

} // namespace

