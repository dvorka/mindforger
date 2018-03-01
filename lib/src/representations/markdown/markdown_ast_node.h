/*
 markdown_ast_node.h     MindForger thinking notebook

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
#ifndef M8R_MARKDOWN_AST_MarkdownAstNodeSection_H_
#define M8R_MARKDOWN_AST_MarkdownAstNodeSection_H_

#include <string>

#include "markdown_note_metadata.h"

namespace m8r {

enum class MarkdownAstNodeType {
    // AST type
    AST_STANDARD,       // AST w/ finest granularity
    AST_SECTIONS_ONLY,  // AST on header/section-body granularity

    // logical structure
    SECTION,            // = header + section body
    SECTION_BODY,       // = unparsed section content (parsed in standard AST only)
    END,                // end of document

    // blocks
    HEADER,
    PARAGRAPH,
    BLOCKQUOTE,
    CODEBLOCK,
    LIST_ORDERED,
    LIST_UNORDERED,
    LIST_ITEM,
    HR,
    // spans
    CODE,
    LINK,
    IMAGE,
    REFERENCE,
    REFERENCE_DEFINITION,
    EMPHASIS,
    DOUBLE_EMPHASIS,
    DOUBLE_BOLD,
    // content
    TEXTLINE,
    TEXT
};

// Markdown backslash escapes for:
//  \   backslash
//  `   backtick
//  *   asterisk
//  _   underscore
//  {}  curly braces
//  []  square brackets
//  ()  parentheses
//  #   hash mark
//  +   plus sign
//  -   minus sign (hyphen)
//  .   dot
//  !   exclamation mark

/**
 * @brief Markdown parsed to AST.
 *
 * Markdown abstract syntax tree (AST) typically on section granularity.
 */
class MarkdownAstNode
{
protected:
    MarkdownAstNodeType type;
    /**
     * @brief Text.
     *
     * Allocated to be reused in notes, outlines, etc.
     */
    std::string* text;

public:
    MarkdownAstNode() = delete;
    explicit MarkdownAstNode(const MarkdownAstNodeType type);
    MarkdownAstNode(const MarkdownAstNode &) = delete;
    MarkdownAstNode(const MarkdownAstNode &&) = delete;
    MarkdownAstNode &operator=(const MarkdownAstNode &) = delete;
    MarkdownAstNode &operator=(const MarkdownAstNode &&) = delete;
    virtual ~MarkdownAstNode();

    std::string* getText() const { return text; }
    MarkdownAstNodeType getType() const;
    void setType(MarkdownAstNodeType type);
};

class MarkdownAstSectionMetadata
{
private:
    const std::string* type;

    time_t created;
    time_t modified;
    u_int32_t revision;
    time_t read;
    u_int32_t reads;

    int8_t importance;
    int8_t urgency;
    int8_t progress;

    std::vector<std::string*> tags;

public:
    explicit MarkdownAstSectionMetadata();
    MarkdownAstSectionMetadata(const MarkdownAstSectionMetadata&) = delete;
    MarkdownAstSectionMetadata(const MarkdownAstSectionMetadata&&) = delete;
    MarkdownAstSectionMetadata &operator=(const MarkdownAstSectionMetadata&) = delete;
    MarkdownAstSectionMetadata &operator=(const MarkdownAstSectionMetadata&&) = delete;
    virtual ~MarkdownAstSectionMetadata();

    time_t getCreated() const;
    void setCreated(time_t created);
    int8_t getImportance() const;
    void setImportance(int8_t importance);
    time_t getModified() const;
    void setModified(time_t modified);
    int8_t getProgress() const;
    void setProgress(int8_t progress);
    time_t getRead() const;
    void setRead(time_t read);
    u_int32_t getReads() const;
    void setReads(u_int32_t reads);
    u_int32_t getRevision() const;
    void setRevision(u_int32_t revision);
    const std::string* getType() const;
    void setType(const std::string* type);
    int8_t getUrgency() const;
    void setUrgency(int8_t urgency);
    const std::vector<std::string*>& getTags() const;
    void setTags(std::vector<std::string*>* tags);
    const std::string* getPrimaryTag() const;
};

class MarkdownAstNodeSection : public MarkdownAstNode
{
public:
    // IMPROVE constexpr
    static u_int16_t PREAMBLE;

protected:
    /**
     * @brief Regular section depth [0,0xFFFF) or preamble section marker (PREAMBLE ~ no depth).
     */
    u_int16_t depth;
    MarkdownAstSectionMetadata metadata;
    std::vector<std::string*>* body;

public:
    explicit MarkdownAstNodeSection();
    explicit MarkdownAstNodeSection(std::string *name);
    MarkdownAstNodeSection(const MarkdownAstNodeSection &) = delete;
    MarkdownAstNodeSection(const MarkdownAstNodeSection &&) = delete;
    MarkdownAstNodeSection &operator=(const MarkdownAstNodeSection &) = delete;
    MarkdownAstNodeSection &operator=(const MarkdownAstNodeSection &&) = delete;
    virtual ~MarkdownAstNodeSection();

    std::vector<std::string*>* getBody() const { return body; }
    std::vector<std::string*>* moveBody() { std::vector<std::string*>* result=body; body=nullptr; return result; }
    void setBody(std::vector<std::string*>* body);

    u_int16_t getDepth() const;
    void setDepth(u_int16_t depth);
    MarkdownAstSectionMetadata& getMetadata();

    void setPreamble() { depth = PREAMBLE; }
    bool isPreambleSection() { return depth == PREAMBLE; }
};

} // m8r namespace

#endif /* M8R_MARKDOWN_AST_MarkdownAstNodeSection_H_ */
