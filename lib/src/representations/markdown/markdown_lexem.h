/*
 markdown_lexem.h     MindForger thinking notebook

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
#ifndef M8R_MARKDOWN_LEXEM_H_
#define M8R_MARKDOWN_LEXEM_H_

#include <climits>
#include <string>

namespace m8r {

/**
 * @brief Symbol type.
 */
enum class MarkdownLexemType
{
    BEGIN_DOC,                  // begin of document

    SECTION,                    // "#" .. "##...#"
    SECTION_equals,              // "==" 2 or more equal signs (post declared section)
    SECTION_hyphens,             // "--" 2 or more hyphens (post declared section)
    TEXT,                       // "name"
    LINE,                       // ^"This is sentence."$ (w/o ^ and $ markers)
    WHITESPACES,                 // [:whitespace]+ (\n\r TAB)

    META_BEGIN,                 // Metadata (case insensitive)
    META_TEXT,                  // text of metadata HTML comment: '[:whitespace]Metadata: ...' (w/o HTML comment begin/end markers)
    META_PROPERTY_DELIMITER,    // ,

    META_PROPERTY_UNKNOWN,      // unknown property name > stored in text field
    META_PROPERTY_type,         // "type" keyword symbol
    META_PROPERTY_created,
    META_PROPERTY_reads,
    META_PROPERTY_read,
    META_PROPERTY_revision,
    META_PROPERTY_modified,
    META_PROPERTY_importance,
    META_PROPERTY_urgency,
    META_PROPERTY_progress,
    META_PROPERTY_tags,         // outline or note tags
    META_PROPERTY_links,
    META_PROPERTY_deadline,
    META_PROPERTY_forget,

    META_NAMEVALUE_DELIMITER,   // :
    META_PROPERTY_VALUE,

    HTML_COMMENT_BEGIN,         // <!--
    HTML_COMMENT_TEXT,
    HTML_COMMENT_END,           // -->

    BR,                         // \n

    END_DOC                    // end of document
};

/**
 * @brief Token created by a Markdown lexical analyzer (tokenizer).
 */
class MarkdownLexem
{
public:
    // IMPROVE constexpr
    static unsigned short int NO_TEXT;
    static unsigned short int WHOLE_LINE;

private:
    MarkdownLexemType type;
    /**
     * @brief Offset - line number where text presents (USHRT_MAX represents no text, 64k lines max)
     */
    unsigned short int off;
    /**
     * @brief Index - beginning of the text on the line (64k chars line length max).
     */
    unsigned short int idx;
    /**
     * @brief Length - text length (USHRT_MAX represents whole line).
     */
    unsigned short int lng;
    /**
     * @brief Depth - if lexem represents section [1,INF> (64k levels deep sections hierarchy).
     */
    unsigned short int depth;

public:
    MarkdownLexem() = delete;
    explicit MarkdownLexem(MarkdownLexemType type);
    MarkdownLexem(
            MarkdownLexemType type,
            unsigned short int offset,
            unsigned short int index,
            unsigned short int lenght);
    MarkdownLexem(MarkdownLexemType type, unsigned short int depth);
    MarkdownLexem(const MarkdownLexem&) = delete;
    MarkdownLexem(const MarkdownLexem&&) = delete;
    MarkdownLexem& operator=(const MarkdownLexem&) = delete;
    MarkdownLexem& operator=(const MarkdownLexem&&) = delete;
    virtual ~MarkdownLexem();

    MarkdownLexemType getType() const;
    void setType(MarkdownLexemType type);
    unsigned getDepth() const;
    void setDepth(unsigned depth);
    unsigned short int getIdx() const { return idx; }
    void setIdx(unsigned short int idx);
    unsigned short int getLng() const { return lng; }
    void setLng(unsigned short int lng);
    unsigned short int getOff() const { return off; }
    void setOff(unsigned short int off);
};

} // m8r namespace

#endif /* M8R_MARKDOWN_LEXEM_H_ */
