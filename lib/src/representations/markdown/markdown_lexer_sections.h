/*
 markdown_lexer_sections.h     MindForger thinking notebook

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
#ifndef M8R_MARKDOWN_LEXER_SECTIONS_H_
#define M8R_MARKDOWN_LEXER_SECTIONS_H_

#include <set>
#include <string>
#include <vector>
#include <unordered_set>

#include "../../gear/lang_utils.h"
#include "../../gear/file_utils.h"
#include "markdown_lexem.h"

namespace m8r {

/**
 * @brief Managed table of reusable lexems.
 */
class MarkdownLexemTable {
private:
    // IMPROVE unordered set
    std::set<MarkdownLexem*> lexems;

public:
    MarkdownLexem* BEGIN_DOC;
    MarkdownLexem* META_BEGIN;
    MarkdownLexem* META_PROPERTY_DELIMITER;
    MarkdownLexem* META_PROPERTY_type;
    MarkdownLexem* META_PROPERTY_created;
    MarkdownLexem* META_PROPERTY_reads;
    MarkdownLexem* META_PROPERTY_read;
    MarkdownLexem* META_PROPERTY_revision;
    MarkdownLexem* META_PROPERTY_modified;
    MarkdownLexem* META_PROPERTY_importance;
    MarkdownLexem* META_PROPERTY_urgency;
    MarkdownLexem* META_PROPERTY_progress;
    MarkdownLexem* META_PROPERTY_tags;
    MarkdownLexem* META_PROPERTY_links;
    MarkdownLexem* META_PROPERTY_deadline;
    MarkdownLexem* META_PROPERTY_scope;
    MarkdownLexem* META_NAMEVALUE_DELIMITER;
    MarkdownLexem* HTML_COMMENT_BEGIN;
    MarkdownLexem* HTML_COMMENT_END;
    MarkdownLexem* BR;
    MarkdownLexem* END_DOC;

    MarkdownLexemTable();
    MarkdownLexemTable(const MarkdownLexemTable&) = delete;
    MarkdownLexemTable(const MarkdownLexemTable&&) = delete;
    MarkdownLexemTable& operator=(const MarkdownLexemTable&) = delete;
    MarkdownLexemTable& operator=(const MarkdownLexemTable&&) = delete;
    ~MarkdownLexemTable();

    bool contains(MarkdownLexem *lexem) const { return lexems.find(lexem)!=lexems.end(); }
};

class MarkdownSymbolTable
{
private:
    /**
     * Symbols that are reused through the lexing and parsing
     * process (function names, variable names, ...) are kept
     * from loading file to final entity (Outline, Note) creation.
     */
    // IMPROVE use unordered_set once I found out how to compile it
    std::set<std::string*> symbols;

public:
    // reusable lexems
    static const MarkdownLexemTable& LEXEM;

public:
    explicit MarkdownSymbolTable();
    MarkdownSymbolTable(const MarkdownSymbolTable&) = delete;
    MarkdownSymbolTable(const MarkdownSymbolTable&&) = delete;
    MarkdownSymbolTable& operator=(const MarkdownSymbolTable&) = delete;
    MarkdownSymbolTable& operator=(const MarkdownSymbolTable&&) = delete;
    ~MarkdownSymbolTable();

    void addSymbol(std::string *symbol);
    bool lookup(std::string* symbol) const;
    void clearSymbols() { symbols.clear(); }
};

/**
 * @brief Markdown lexical analyzer for section-level granularity parser.
 */
class MarkdownLexerSections
{
private:
    const std::string* filePath;
    unsigned lastBrTokensOffset;
    bool inCodeBlock;

    unsigned long int fileSize;
    std::vector<std::string*> lines;
    // IMPROVE prepare a LexemPool: vector + MarkdownLexem[1000] and allocate from there (performance)
    std::vector<MarkdownLexem*> lexems;
    MarkdownSymbolTable symbolTable;

public:
    explicit MarkdownLexerSections(const std::string* filePath=nullptr);
    MarkdownLexerSections(const MarkdownLexerSections &) = delete;
    MarkdownLexerSections(const MarkdownLexerSections &&);
    MarkdownLexerSections &operator=(const MarkdownLexerSections &) = delete;
    MarkdownLexerSections &operator=(const MarkdownLexerSections &&) = delete;
    virtual ~MarkdownLexerSections();

    void tokenize();
    void tokenize(const std::string* text);

    /**
     * Returns text, caller is expected to destroy it.
     */
    std::string* getText(const MarkdownLexem*);

    void setFilePath(const std::string*& filePath) { this->filePath = filePath; }
    unsigned long int getFileSize() const { return fileSize; }
    const std::vector<MarkdownLexem*>& getLexems() const { return lexems; }
    const std::vector<std::string*>& getLines() const { return lines; }
    const MarkdownSymbolTable& getSymbolTable() const { return symbolTable; }
    MarkdownLexem* operator[](size_t i) { return lexems[i]; }
    const MarkdownLexem* operator[](size_t i) const { return lexems[i]; }
    bool empty() const { return lexems.empty(); }
    size_t size() const { return lexems.size(); }

private:
    bool nextToken(const unsigned short int offset);

    inline bool lookahead(const unsigned short offset, const unsigned short idx) const;
    void toggleInCodeBlock() { inCodeBlock=!inCodeBlock; }

    inline bool isSameCharsLine(const unsigned short offset, const char c) const;
    inline bool startsWithCodeBlockSymbol(const unsigned short offset) const;
    inline bool startsWithHtmlCommentEndSymbol(const unsigned short offset, const unsigned short int idx) const;

    inline bool lexWhitespaces(const unsigned short offset, unsigned short int& idx);
    inline bool lexSectionSymbol(const unsigned short offset, unsigned short int& idx);
    inline bool lexHtmlCommentBeginSymbol(const unsigned short offset, unsigned short int& idx);
    inline bool lexHtmlCommentEndSymbol(const unsigned short offset, unsigned short int& idx);
    inline bool lexMetadataSymbol(const unsigned short offset, unsigned short int& idx);
    inline bool lexMetaPropertyName(const unsigned short offset, unsigned short int& idx);
    inline bool lexMetaPropertyNameValueDelimiter(const unsigned short offset, unsigned short int& idx);
    inline bool lexMetaPropertyValue(const unsigned short offset, unsigned short int& idx);
    inline bool lexMetaPropertyDelimiter(const unsigned short offset, unsigned short int& idx);
    inline bool lexToEndOfHtmlComment(const unsigned short offset, unsigned short int& idx);
    inline bool lexPostDeclaredSectionHeader(const unsigned short offset, const char delimiter);

    inline void addLineToLexems(const unsigned short offset);

    /**
     * @brief Insert back section lexem if "standalone line section declaration" found.
     *
     * Tokenize previous line as section header and prepend SECTION lexem with given depth.
     */
    void fixBackDeclaredSection(const unsigned short offset, const unsigned short sectionDepth);
};

} // m8r namespace

#endif /* M8R_MARKDOWN_LEXER_SECTIONS_H_ */
