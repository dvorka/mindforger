/*
 markdown.cpp     MindForger thinking notebook

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
#include "markdown.h"

using namespace std;

namespace m8r {

Markdown::Markdown(const std::string* filePath)
{
    this->filePath = filePath;
    this->fileSize = 0;
    this->modified = 0;
    this->ast = nullptr;
}

void Markdown::clear(void)
{
    this->fileSize = 0;
    this->modified = 0;
    this->title.clear();
    if(ast!=nullptr) {
        delete ast;
        ast = nullptr;
    }
}

void Markdown::from(void)
{
    clear();
    modified = fileModificationTime(filePath);
    MarkdownLexerSections lexer{filePath};
    lexer.tokenize();
    // IMPROVE the rest of this section could be shared by file & text
    if(lexer.getLexems().size()) {
        fileSize = lexer.getFileSize();
        // must be pointer (circular header dep)
        MarkdownParserSections parser{lexer};
        parser.parse();
        // parser is deleted on return, but AST is kept
        ast = parser.moveAst();
        from(ast);
    } // else: empty file/no lexems
}

void Markdown::from(const std::string* text)
{
    clear();
    modified = datetimeNow();
    MarkdownLexerSections lexer{};
    lexer.tokenize(text);
    // IMPROVE the rest of this section could be shared by file & text
    if(lexer.getLexems().size()) {
        fileSize = lexer.getFileSize();
        MarkdownParserSections parser = MarkdownParserSections(lexer);
        parser.parse();
        // parser is deleted on return, but AST is kept
        ast = parser.moveAst();
        from(ast);
    } // else: empty file/no lexems
}

void Markdown::from(const std::vector<MarkdownAstNodeSection*>* ast)
{
    if(ast!=nullptr && ast->size()) {
        if(ast->at(0)->getText()!=nullptr) {
            title.assign(*ast->at(0)->getText());
        }
    }
}

const std::string* Markdown::getFilePath() const
{
    return filePath;
}

unsigned Markdown::getFileSize(void) const
{
    return fileSize;
}

Markdown::~Markdown()
{
    if(ast) {
        for(MarkdownAstNodeSection*& node:*ast) {
            delete node;
        }
        delete ast;
        ast = nullptr;
    }
}

} /* namespace m8r */


