/*
 markdown_lexer-sections.cpp     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "markdown_lexer_sections.h"

using namespace std;

namespace m8r {

/*
 * FileLineProvider
 */

// IMPROVE consider lazy loading of lines using FileLineProvider instead of pre-loading
class FileLineProvider
{
private:
    ifstream* infile;

public:
    explicit FileLineProvider(const char* filename);
    virtual ~FileLineProvider();

    string* getLine();
};

FileLineProvider::FileLineProvider(const char* filename)
{
    infile = new ifstream{filename};
}

string* FileLineProvider::getLine()
{
    // IMPROVE this is slow heap operation
    string* line = new string();
    getline(*infile, *line);
    return line;
}

FileLineProvider::~FileLineProvider()
{
    infile->close();
    delete infile;
}

/*
 * MarkdownLexemTable
 */

MarkdownLexemTable::MarkdownLexemTable()
{
    BEGIN_DOC = new MarkdownLexem { MarkdownLexemType::BEGIN_DOC };
    lexems.insert(BEGIN_DOC);
    META_BEGIN = new MarkdownLexem { MarkdownLexemType::META_BEGIN };
    lexems.insert(META_BEGIN);
    META_PROPERTY_DELIMITER = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_DELIMITER};
    lexems.insert(META_PROPERTY_DELIMITER);
    META_PROPERTY_type = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_type};
    lexems.insert(META_PROPERTY_type);
    META_PROPERTY_created = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_created};
    lexems.insert(META_PROPERTY_created);
    META_PROPERTY_reads = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_reads};
    lexems.insert(META_PROPERTY_reads);
    META_PROPERTY_read = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_read};
    lexems.insert(META_PROPERTY_read);
    META_PROPERTY_revision = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_revision};
    lexems.insert(META_PROPERTY_revision);
    META_PROPERTY_modified = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_modified};
    lexems.insert(META_PROPERTY_modified);
    META_PROPERTY_importance = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_importance};
    lexems.insert(META_PROPERTY_importance);
    META_PROPERTY_urgency = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_urgency};
    lexems.insert(META_PROPERTY_urgency);
    META_PROPERTY_progress = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_progress};
    lexems.insert(META_PROPERTY_progress);
    META_PROPERTY_tags = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_tags};
    lexems.insert(META_PROPERTY_tags);
    META_PROPERTY_links= new MarkdownLexem{MarkdownLexemType::META_PROPERTY_links};
    lexems.insert(META_PROPERTY_links);
    META_PROPERTY_deadline = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_deadline};
    lexems.insert(META_PROPERTY_deadline);
    META_PROPERTY_scope = new MarkdownLexem{MarkdownLexemType::META_PROPERTY_scope};
    lexems.insert(META_PROPERTY_scope);
    META_NAMEVALUE_DELIMITER = new MarkdownLexem{MarkdownLexemType::META_NAMEVALUE_DELIMITER};
    lexems.insert(META_NAMEVALUE_DELIMITER);
    HTML_COMMENT_BEGIN = new MarkdownLexem{MarkdownLexemType::HTML_COMMENT_BEGIN};
    lexems.insert(HTML_COMMENT_BEGIN);
    HTML_COMMENT_END = new MarkdownLexem{MarkdownLexemType::HTML_COMMENT_END};
    lexems.insert(HTML_COMMENT_END);
    BR = new MarkdownLexem{MarkdownLexemType::BR};
    lexems.insert(BR);
    END_DOC = new MarkdownLexem{MarkdownLexemType::END_DOC};
    lexems.insert(END_DOC);
}

MarkdownLexemTable::~MarkdownLexemTable()
{
    delete BEGIN_DOC;
    delete META_BEGIN;
    delete META_PROPERTY_DELIMITER;
    delete META_PROPERTY_type;
    delete META_PROPERTY_created;
    delete META_PROPERTY_reads;
    delete META_PROPERTY_read;
    delete META_PROPERTY_revision;
    delete META_PROPERTY_modified;
    delete META_PROPERTY_importance;
    delete META_PROPERTY_urgency;
    delete META_PROPERTY_progress;
    delete META_PROPERTY_tags;
    delete META_PROPERTY_links;
    delete META_PROPERTY_deadline;
    delete META_PROPERTY_scope;
    delete META_NAMEVALUE_DELIMITER;
    delete HTML_COMMENT_BEGIN;
    delete HTML_COMMENT_END;
    delete BR;
    delete END_DOC;
}

/*
 * MarkdownSymbolTable
 */

const MarkdownLexemTable& MarkdownSymbolTable::LEXEM = MarkdownLexemTable();

MarkdownSymbolTable::MarkdownSymbolTable()
{
}

bool MarkdownSymbolTable::lookup(string* symbol) const
{
    if(symbol!=nullptr && symbols.find(symbol)!=symbols.end()) {
        return true;
    } else {
        return false;
    }
}

void MarkdownSymbolTable::addSymbol(string* symbol)
{
    symbols.insert(symbol);
}

MarkdownSymbolTable::~MarkdownSymbolTable()
{
    if(symbols.size()) {
        // TODO delete set members
    }
}

/*
 * MarkdownLexerSections
 */

MarkdownLexerSections::MarkdownLexerSections(const string* filePath)
{
    this->filePath = filePath;
    this->fileSize = 0;
    this->inCodeBlock = false;
    this->lastBrTokensOffset = 0;
}

MarkdownLexerSections::~MarkdownLexerSections()
{
    // lines
    for(string*& line:lines) {
        if(line!=nullptr) {
            delete line;
        }
    }

    // lexems
    for(MarkdownLexem*& lexem:lexems) {
        if(lexem!=nullptr) {
            if(!MarkdownSymbolTable::LEXEM.contains(lexem)) {
                delete lexem;
                lexem=nullptr;
            }
        }
    }
}

void MarkdownLexerSections::tokenize()
{
    fileSize = 0;
    if(fileToLines(filePath, lines, fileSize)) {
        // IMPROVE body of this function can be shared by file & text
        lexems.push_back(MarkdownSymbolTable::LEXEM.BEGIN_DOC);

        unsigned offset = 0;
        while(nextToken(offset)) {
            offset++;
        }

        if(lexems.size()==1) {
            lexems.clear();
        } else {
            lexems.push_back(MarkdownSymbolTable::LEXEM.END_DOC);
        }
    }
}

void MarkdownLexerSections::tokenize(const string* text)
{
    if(stringToLines(text, lines)) {
        // IMPROVE body of this function can be shared by file & text
        lexems.push_back(MarkdownSymbolTable::LEXEM.BEGIN_DOC);

        unsigned offset = 0;
        while(nextToken(offset)) {
            offset++;
        }

        if(lexems.size()==1) {
            lexems.clear();
        } else {
            lexems.push_back(MarkdownSymbolTable::LEXEM.END_DOC);
        }
    }
}

bool MarkdownLexerSections::lexWhitespaces(const unsigned offset, unsigned short int& idx)
{
    unsigned short int i = idx+1;
    if(lines[offset]!=nullptr) {
        while(lines[offset]->size()>i && isspace(lines[offset]->at(i))) {
            i++;
        }
        if(i != idx+1) {
            lexems.push_back(new MarkdownLexem(MarkdownLexemType::WHITESPACES,offset,idx+1,i-1-idx));
            idx = i-1;
            return true;
        }
    }
    return false;
}

bool MarkdownLexerSections::startsWithCodeBlockSymbol(const unsigned offset) const
{
    if(lines[offset]!=nullptr && lines[offset]->size()>=3
         &&
       lines[offset]->at(0)=='`' && lines[offset]->at(1)=='`' && lines[offset]->at(2)=='`'
    ){
        return true;
    } else {
        return false;
    }
}

bool MarkdownLexerSections::startsWithHtmlCommentEndSymbol(const unsigned offset, const unsigned short idx) const
{
    if(lines[offset]!=nullptr && lines[offset]->size()>=(size_t)(idx+3)
         &&
       lines[offset]->at(idx)=='-' && lines[offset]->at(idx+1)=='-' && lines[offset]->at(idx+2)=='>'
    ){
        return true;
    } else {
        return false;
    }
}

bool MarkdownLexerSections::lexSectionSymbol(const unsigned offset, unsigned short int& idx)
{
    unsigned depth = 0; // depth = [0,n)
    if(lines[offset]!=nullptr) {
        while(lines[offset]->size()>depth && lines[offset]->at(depth)=='#') {
            ++depth;
        }
        if(depth
             &&
           (lines[offset]->size()>=depth || isspace(lines[offset]->at(depth))))
        {
            idx = depth-1;
            lexems.push_back(new MarkdownLexem(MarkdownLexemType::SECTION,depth-1));
            return true;
        }
    }
    return false;
}

bool MarkdownLexerSections::lexHtmlCommentBeginSymbol(const unsigned offset, unsigned short int& idx)
{
    if(lines[offset]!=nullptr && lines[offset]->size()>=(size_t)(idx+4)
         &&
       lines[offset]->at(idx)=='<' && lines[offset]->at(idx+1)=='!' && lines[offset]->at(idx+2)=='-' && lines[offset]->at(idx+3)=='-'
    ){
        idx+=4;
        lexems.push_back(symbolTable.LEXEM.HTML_COMMENT_BEGIN);
        return true;
    } else {
        return false;
    }
}

bool MarkdownLexerSections::lexHtmlCommentEndSymbol(const unsigned offset, unsigned short int& idx)
{
    if(lines[offset]!=nullptr && lines[offset]->size()>=(size_t)(idx+3)
         &&
       lines[offset]->at(idx)=='-' && lines[offset]->at(idx+1)=='-' && lines[offset]->at(idx+2)=='>'
    ){
        idx+=3;
        lexems.push_back(symbolTable.LEXEM.HTML_COMMENT_END);
        return true;
    } else {
        return false;
    }
}

bool MarkdownLexerSections::lexMetadataSymbol(const unsigned offset, unsigned short int& idx)
{
    // case insensitive 'metadata'
    if(lines[offset]!=nullptr && lines[offset]->size()>=(size_t)(idx+9)
         &&
       (lines[offset]->at(idx+1)=='M' || lines[offset]->at(idx+1)=='m') &&
       (lines[offset]->at(idx+2)=='e' || lines[offset]->at(idx+2)=='E') &&
       (lines[offset]->at(idx+3)=='t' || lines[offset]->at(idx+3)=='T') &&
       (lines[offset]->at(idx+4)=='a' || lines[offset]->at(idx+4)=='A') &&
       (lines[offset]->at(idx+5)=='d' || lines[offset]->at(idx+5)=='D') &&
       (lines[offset]->at(idx+6)=='a' || lines[offset]->at(idx+6)=='A') &&
       (lines[offset]->at(idx+7)=='t' || lines[offset]->at(idx+7)=='T') &&
       (lines[offset]->at(idx+8)=='a' || lines[offset]->at(idx+8)=='A') &&
       lines[offset]->at(idx+9)==':'
    ){
        idx+=9;
        lexems.push_back(symbolTable.LEXEM.META_BEGIN);
        return true;
    } else {
        return false;
    }
}

bool MarkdownLexerSections::lexMetaPropertyName(const unsigned offset, unsigned short int& idx)
{
    if(lines[offset]->size() > (size_t)(idx+1)) {
        switch(lines[offset]->at(idx+1)) {
        case 't':
            if(lines[offset]->at(idx+2)=='y' &&
               lines[offset]->at(idx+3)=='p' &&
               lines[offset]->at(idx+4)=='e' &&
               (lines[offset]->at(idx+5)==':' || !isspace(idx+5))) {
                idx+=4;
                lexems.push_back(symbolTable.LEXEM.META_PROPERTY_type);
                return true;
            } else {
                if(lines[offset]->at(idx+2)=='a' &&
                   lines[offset]->at(idx+3)=='g' &&
                   lines[offset]->at(idx+4)=='s' &&
                   (lines[offset]->at(idx+5)==':' || !isspace(idx+5))) {
                    idx+=4;
                    lexems.push_back(symbolTable.LEXEM.META_PROPERTY_tags);
                    return true;
                } else {
                    return false;
                }
            }
        case 'c':
            if(lines[offset]->at(idx+2)=='r' &&
               lines[offset]->at(idx+3)=='e' &&
               lines[offset]->at(idx+4)=='a' &&
               lines[offset]->at(idx+5)=='t' &&
               lines[offset]->at(idx+6)=='e' &&
               lines[offset]->at(idx+7)=='d' &&
               (lines[offset]->at(idx+8)==':' || !isspace(idx+8))) {
                lexems.push_back(symbolTable.LEXEM.META_PROPERTY_created);
                idx+=7;
                return true;
            } else {
                return false;
            }
        case 'r':
            if(lines[offset]->at(idx+2)=='e') {
                if(lines[offset]->at(idx+3)=='a' &&
                   lines[offset]->at(idx+4)=='d')
                {
                    if(lines[offset]->at(idx+5)=='s' &&
                       (lines[offset]->at(idx+6)==':' || !isspace(idx+6))) {
                        idx+=5;
                        lexems.push_back(symbolTable.LEXEM.META_PROPERTY_reads);
                        return true;
                    } else {
                        if((lines[offset]->at(idx+5)==':' || !isspace(idx+5))) {
                            idx+=4;
                            lexems.push_back(symbolTable.LEXEM.META_PROPERTY_read);
                            return true;
                        }
                    }
                } else {
                    if(lines[offset]->at(idx+3)=='v' &&
                       lines[offset]->at(idx+4)=='i' &&
                       lines[offset]->at(idx+5)=='s' &&
                       lines[offset]->at(idx+6)=='i' &&
                       lines[offset]->at(idx+7)=='o' &&
                       lines[offset]->at(idx+8)=='n' &&
                       (lines[offset]->at(idx+9)==':' || !isspace(idx+9)))
                    {
                        idx+=8;
                        lexems.push_back(symbolTable.LEXEM.META_PROPERTY_revision);
                        return true;
                    }
                }
            }
            return false;
        case 'i':
            if(lines[offset]->at(idx+2)=='m' &&
               lines[offset]->at(idx+3)=='p' &&
               lines[offset]->at(idx+4)=='o' &&
               lines[offset]->at(idx+5)=='r' &&
               lines[offset]->at(idx+6)=='t' &&
               lines[offset]->at(idx+7)=='a' &&
               lines[offset]->at(idx+8)=='n' &&
               lines[offset]->at(idx+9)=='c' &&
               lines[offset]->at(idx+10)=='e' &&
               (lines[offset]->at(idx+11)==':' || !isspace(idx+11))) {
                lexems.push_back(symbolTable.LEXEM.META_PROPERTY_importance);
                idx+=10;
                return true;
            } else {
                return false;
            }
        case 'u':
            if(lines[offset]->at(idx+2)=='r' &&
               lines[offset]->at(idx+3)=='g' &&
               lines[offset]->at(idx+4)=='e' &&
               lines[offset]->at(idx+5)=='n' &&
               lines[offset]->at(idx+6)=='c' &&
               lines[offset]->at(idx+7)=='y' &&
               (lines[offset]->at(idx+8)==':' || !isspace(idx+8))) {
                lexems.push_back(symbolTable.LEXEM.META_PROPERTY_urgency);
                idx+=7;
                return true;
            } else {
                return false;
            }
        case 'p':
            if(lines[offset]->at(idx+2)=='r' &&
               lines[offset]->at(idx+3)=='o' &&
               lines[offset]->at(idx+4)=='g' &&
               lines[offset]->at(idx+5)=='r' &&
               lines[offset]->at(idx+6)=='e' &&
               lines[offset]->at(idx+7)=='s' &&
               lines[offset]->at(idx+8)=='s' &&
               (lines[offset]->at(idx+9)==':' || !isspace(idx+9))) {
                lexems.push_back(symbolTable.LEXEM.META_PROPERTY_progress);
                idx+=8;
                return true;
            } else {
                return false;
            }
        case 'm':
            if(lines[offset]->at(idx+2)=='o' &&
               lines[offset]->at(idx+3)=='d' &&
               lines[offset]->at(idx+4)=='i' &&
               lines[offset]->at(idx+5)=='f' &&
               lines[offset]->at(idx+6)=='i' &&
               lines[offset]->at(idx+7)=='e' &&
               lines[offset]->at(idx+8)=='d' &&
               (lines[offset]->at(idx+9)==':' || !isspace(idx+9))) {
                lexems.push_back(symbolTable.LEXEM.META_PROPERTY_modified);
                idx+=8;
                return true;
            } else {
                return false;
            }
        case 'l':
            // key for relationships is 'links' because a) there are clashes for 'r' b) links is shorter than relationships
            if(lines[offset]->at(idx+2)=='i' &&
               lines[offset]->at(idx+3)=='n' &&
               lines[offset]->at(idx+4)=='k' &&
               lines[offset]->at(idx+5)=='s' &&
               (lines[offset]->at(idx+6)==':' || !isspace(idx+6))) {
                lexems.push_back(symbolTable.LEXEM.META_PROPERTY_links);
                idx+=5;
                return true;
            } else {
                return false;
            }
        case 's':
            if(lines[offset]->at(idx+2)=='c' &&
               lines[offset]->at(idx+3)=='o' &&
               lines[offset]->at(idx+4)=='p' &&
               lines[offset]->at(idx+5)=='e' &&
               (lines[offset]->at(idx+6)==':' || !isspace(idx+6))) {
                lexems.push_back(symbolTable.LEXEM.META_PROPERTY_scope);
                idx+=5;
                return true;
            } else {
                return false;
            }
        case 'd':
            if(lines[offset]->at(idx+2)=='e' &&
               lines[offset]->at(idx+3)=='a' &&
               lines[offset]->at(idx+4)=='d' &&
               lines[offset]->at(idx+5)=='l' &&
               lines[offset]->at(idx+6)=='i' &&
               lines[offset]->at(idx+7)=='n' &&
               lines[offset]->at(idx+8)=='e' &&
               (lines[offset]->at(idx+9)==':' || !isspace(idx+9))) {
                lexems.push_back(symbolTable.LEXEM.META_PROPERTY_deadline);
                idx+=8;
                return true;
            } else {
                return false;
            }
        default:
            return false;
        }
    }
    return false;
}

/**
 * Tokenize the remaining part of the line regardless what's there.
 */
bool MarkdownLexerSections::lexToEndOfHtmlComment(const unsigned offset, unsigned short int& idx)
{
    if(lines[offset]!=nullptr && lines[offset]->size()>(size_t)(idx+1)) {
        unsigned short int i;
        for(i=idx+1;
            i<lines[offset]->size();
            i++) {
            if(lines[offset]->at(i)=='-') {
                if(startsWithHtmlCommentEndSymbol(offset,i)) {
                    if(i > idx+1) {
                        lexems.push_back(new MarkdownLexem(MarkdownLexemType::META_TEXT,offset,idx,i-idx)); // note: ushort-ushort narrowing ({} > ())
                        idx=i;
                    }
                    lexHtmlCommentEndSymbol(offset,i);
                    if(lines[offset]->size()>=i) {
                        lexems.push_back(symbolTable.LEXEM.BR);
                    }
                    return true;
                }
            }
        }
        if(i > idx+1) {
            lexems.push_back(new MarkdownLexem(MarkdownLexemType::META_TEXT,offset,idx,i-idx)); // note: ushort-ushort narrowing ({} > ())
            lexems.push_back(symbolTable.LEXEM.BR);
            idx=i;
            return true;
        }
    }
    lexems.push_back(symbolTable.LEXEM.BR);
    return false;
}

bool MarkdownLexerSections::lexPostDeclaredSectionHeader(const unsigned int offset, const char delimiter)
{
    if (offset == 0 || inCodeBlock) {
        // if the first MD document line starts with '=' > markdown document w/ preamble || in code > ignore
        addLineToLexems(offset);
        return false;
    } else {
        // previous line is valid section name && current line is header line for that name
        if(lines[offset-1]!=nullptr && lines[offset-1]->size()>=2 && !isspace(lines[offset-1]->at(0))
             &&
           isSameCharsLine(offset, delimiter))
        {
            // Patch processed lexems and inject post declared section marker - before:
            //
            //  #0 BEGIN_DOC          0 0 0
            //  #1 LINE               0 0 *
            //  #2 BR                 0 0 0
            //  #3 ... current line...
            // after:
            //  #0 BEGIN_DOC          0 0 0
            //  #1 SECTION=           0 0 0         # note === marker: SECTION vs SECTION= vs SECTION-
            //  #2 LINE               0 0 *
            //  #3 BR                 0 0 0
            //
            // or before:
            //
            //  #3 ...
            //  #4 BR                 0 0 0
            //  #5 BR                 0 0 0
            //  #6 BR                 0 0 0
            //  #7 LINE               8 0 *
            //  #8 BR                 0 0 0
            //  #9 ... current line...
            // after:
            //  #3 ...
            //  #4 BR                 0 0 0
            //  #5 BR                 0 0 0
            //  #6 BR                 0 0 0
            //  #7 SECTION=           0 0 0         # note === marker: SECTION vs SECTION= vs SECTION-
            //  #8 LINE               8 0 *
            //  #9 BR                 0 0 0
            //
            // Conlusion: there MUST be BR and LINE, insert section right after, otherwise normal line

            if(lexems.size()>2 // BEGIN_DOC LINE BR
                 &&
               lexems[lexems.size()-1]->getType()==MarkdownLexemType::BR
                 &&
               lexems[lexems.size()-2]->getType()==MarkdownLexemType::LINE)
            {
                if(delimiter=='=') {
                    lexems.insert(lexems.begin()+lexems.size()-2, new MarkdownLexem(MarkdownLexemType::SECTION_equals,0));
                } else {
                    lexems.insert(lexems.begin()+lexems.size()-2, new MarkdownLexem(MarkdownLexemType::SECTION_hyphens,1));
                }
            } else {
                addLineToLexems(offset);
                return false;
            }
            // insert  marker > parser will create section w/ section type marker (lexem lookahead)
            return true;
        } else {
            addLineToLexems(offset);
            return false;
        }
    }
}

void MarkdownLexerSections::addLineToLexems(const unsigned int offset)
{
    lexems.push_back(new MarkdownLexem{MarkdownLexemType::LINE, offset, 0, MarkdownLexem::WHOLE_LINE});
    lexems.push_back(symbolTable.LEXEM.BR);
}

bool MarkdownLexerSections::nextToken(const unsigned int offset) {
    if(offset<lines.size()) {
        if(lines[offset]->size()==0) {
            lexems.push_back(symbolTable.LEXEM.BR);
            return true;
        } else {
            switch(lines[offset]->at(0)) {
            case '`':
                if(startsWithCodeBlockSymbol(offset)) {
                    // sections lexer just needs to detect code block to avoid detection of false sections, but no need to tokenize it
                    toggleInCodeBlock();
                }
                addLineToLexems(offset);
                return true;
            case '#': // IF #+ space THEN section ELSE line
                if(!inCodeBlock) {
                    unsigned short int idx = 0;
                    if(!lexSectionSymbol(offset,idx)) {
                        addLineToLexems(offset);
                        return true;
                    } else {
                        // #+ parsed > process the rest: [:whitespace]+ (TEXT [:whitespace]+)* METADATA?
                        lexWhitespaces(offset, idx);
                        char cc;
                        unsigned short int ws=0, text=0, x = idx+1;
                        while(lookahead(offset,idx)) {
                            cc = lines[offset]->at(++idx);
                            if(isspace(cc)) {
                                // a) whitespaces
                                if(ws==0 && text) {
                                    lexems.push_back(new MarkdownLexem(MarkdownLexemType::TEXT,offset,x,idx-x)); // note: ushort-ushort narrowing ({} > ())
                                    text = 0;
                                    x = idx;
                                }
                                ws++;
                            } else {
                                // b1) text as section metadata
                                if(cc=='<' && lexHtmlCommentBeginSymbol(offset,idx)) {
                                    lexWhitespaces(offset,idx);
                                    if(lexMetadataSymbol(offset,idx)) {
                                        do {
                                            lexWhitespaces(offset,idx);
                                            if(lexMetaPropertyName(offset,idx)) {
                                                lexWhitespaces(offset,idx);
                                                if(lexMetaPropertyNameValueDelimiter(offset,idx)) {
                                                    lexWhitespaces(offset,idx);
                                                    if(lexMetaPropertyValue(offset,idx)) {
                                                        lexMetaPropertyDelimiter(offset,idx);
                                                    } else {
                                                        // a mess inside comment > seek before the end of comment and finish line parsing
                                                        lexToEndOfHtmlComment(offset,idx);
                                                        return true;
                                                    }
                                                } else {
                                                    lexToEndOfHtmlComment(offset,idx);
                                                    return true;
                                                }
                                            } else {
                                                lexToEndOfHtmlComment(offset,idx);
                                                return true;
                                            }
                                        } while(lookahead(offset,idx));
                                        lexWhitespaces(offset,idx);

                                        unsigned short int mess = 0;
                                        char ccc;
                                        while(lookahead(offset,idx)) {
                                            ccc = lines[offset]->at(++idx);
                                            if(ccc=='-' && lexHtmlCommentEndSymbol(offset,idx)) {
                                                if(mess) {
                                                    // TODO BUG add text BEFORE last lexem
                                                    lexems.push_back(new MarkdownLexem(MarkdownLexemType::TEXT,offset,idx-mess,mess)); // note: ushort-ushort narrowing ({} > ())
                                                }
                                                // IMPROVE process the rest of line after --> (ignored for now)

                                                // TODO FIX
                                                lexems.push_back(symbolTable.LEXEM.BR);
                                                // TODO FIX

                                                return true;
                                            } else {
                                                mess++;
                                            }
                                        }
                                        if(mess) {
                                            lexems.push_back(new MarkdownLexem(MarkdownLexemType::TEXT,offset,idx-mess,mess)); // note: ushort-ushort narrowing ({} > ())
                                        }

                                        // TODO FIX
                                        lexems.push_back(symbolTable.LEXEM.BR);
                                        // TODO FIX

                                        return true;
                                    } else {
                                        // parse HTML comment (as it's not HTML comment w/ metadata)
                                        // TODO
                                    }
                                } else {
                                    // b2) text
                                    if(text==0 && ws) {
                                        lexems.push_back(new MarkdownLexem(MarkdownLexemType::WHITESPACES,offset,x,idx-x)); // note: ushort-ushort narrowing ({} > ())
                                        ws = 0;
                                        x = idx;
                                    }
                                    text++;
                                }
                            }
                        } // while
                        if(ws) {
                            lexems.push_back(new MarkdownLexem(MarkdownLexemType::WHITESPACES,offset,x,idx+1-x)); // note: ushort-ushort narrowing ({} > ())
                        }
                        if(text) {
                            lexems.push_back(new MarkdownLexem(MarkdownLexemType::TEXT,offset,x,idx+1-x)); // note: ushort-ushort narrowing ({} > ())
                        }
                        lexems.push_back(symbolTable.LEXEM.BR);
                        return true;
                    }
                } else {
                    // in code block
                    addLineToLexems(offset);
                }
                return true;
            case '=': // IF [=]+ > 1st level header (not in ``` and prev text)
                lexPostDeclaredSectionHeader(offset, '=');
                return true;
            case '-':
                lexPostDeclaredSectionHeader(offset, '-');
                return true;
            default:
                addLineToLexems(offset);
                return true;
            }
        }
    } else {
        return false;
    }
}

bool MarkdownLexerSections::isSameCharsLine(const unsigned offset, const char c) const
{
    // fail fast
    if(lines[offset]!=nullptr && lines[offset]->size()
         &&
       lines[offset]->at(0)==c && lines[offset]->at(lines[offset]->size()-1)==c)
    {
        for(unsigned i=1; i<lines[offset]->size()-1; i++) {
            if(lines[offset]->at(i)!=c) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool MarkdownLexerSections::lookahead(const unsigned offset, const unsigned short idx) const
{
    if(lines[offset]->size() > (size_t)(idx+1)) {
        return true;
    } else {
        return false;
    }
}

bool MarkdownLexerSections::lexMetaPropertyNameValueDelimiter(const unsigned offset, unsigned short int& idx)
{
    if(lines[offset]->size()>(size_t)(idx+1) && lines[offset]->at(idx+1)==':') {
        idx++;
        lexems.push_back(symbolTable.LEXEM.META_NAMEVALUE_DELIMITER);
        return true;
    } else {
        return false;
    }
}

bool MarkdownLexerSections::lexMetaPropertyValue(const unsigned offset, unsigned short int& idx)
{
    if(lines[offset]!=nullptr && lines[offset]->size()>(size_t)(idx+1)) {
        unsigned short int i;
        for(i=idx+1;
            i<lines[offset]->size() && lines[offset]->at(i)!=';';
            i++)
        {}
        if(i>idx+1) {
            lexems.push_back(new MarkdownLexem(MarkdownLexemType::META_PROPERTY_VALUE,offset,idx+1,i-idx-1));
            idx=i-1;
            return true;
        }
    }
    return false;
}

bool MarkdownLexerSections::lexMetaPropertyDelimiter(const unsigned offset, unsigned short int& idx)
{
    if(lines[offset]->size()>(size_t)(idx+1) && lines[offset]->at(idx+1)==';') {
        lexems.push_back(symbolTable.LEXEM.META_PROPERTY_DELIMITER);
        idx++;
        return true;
    } else {
        return false;
    }
}

string* MarkdownLexerSections::getText(const MarkdownLexem* lexem)
{
    if(lexem!=nullptr && lines.size()) {
        if(lexem->getOff()<lines.size()) {
            if(lexem->getLng()==MarkdownLexem::WHOLE_LINE) {
                string *result = lines[lexem->getOff()];
                lines[lexem->getOff()] = nullptr;
                return result;
            } else {
                if(lexem->getLng()==0) {
                    return new string{};
                } else {
                    return new string{lines[lexem->getOff()]->substr(lexem->getIdx(),lexem->getLng())};
                }
            }
        }
    }
    return nullptr;
}

} // m8r namespace
