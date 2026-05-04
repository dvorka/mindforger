/*
 markdown_ast_node.h     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_MARKDOWN_AST_M8RUI_NAVIGATOR_NODE_H_
#define M8R_MARKDOWN_AST_M8RUI_NAVIGATOR_NODE_H_

#include <string>

#include "../../model/link.h"
#include "../../config/time_scope.h"
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

    TimeScope timeScope;
    time_t deadline;
    std::vector<Link*> links;

public:
    explicit MarkdownAstSectionMetadata();
    MarkdownAstSectionMetadata(const MarkdownAstSectionMetadata&) = delete;
    MarkdownAstSectionMetadata(const MarkdownAstSectionMetadata&&) = delete;
    MarkdownAstSectionMetadata &operator=(const MarkdownAstSectionMetadata&) = delete;
    MarkdownAstSectionMetadata &operator=(const MarkdownAstSectionMetadata&&) = delete;
    virtual ~MarkdownAstSectionMetadata();

    bool isVirgin() const { return type==nullptr && created==0 && tags.size(); }

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

    TimeScope& getTimeScope();
    void setTimeScope(const TimeScope& timeScope);
    time_t getDeadline() const;
    void setDeadline(time_t deadline);
    const std::vector<Link*>& getLinks() const;
    void setLinks(std::vector<Link*>* links);
    void clearLinks() { links.clear(); }
};

class MarkdownAstNodeSection : public MarkdownAstNode
{
public:
    static constexpr u_int16_t PREAMBLE = 0xff00;
    static constexpr int FLAG_MASK_POST_DECLARED_SECTION = 1;
    static constexpr int FLAG_MASK_TRAILING_HASHES_SECTION = 1<<1;

protected:
    /**
     * @brief Regular section depth [0,0xFFFF) or preamble section marker (PREAMBLE ~ no depth).
     */
    u_int16_t depth;
    MarkdownAstSectionMetadata metadata;
    std::vector<std::string*>* body;

    // various flags (bit)
    int flags;

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

    void setPostDeclaredSection() { flags |= FLAG_MASK_POST_DECLARED_SECTION; }
    bool isPostDeclaredSection() const { return flags & FLAG_MASK_POST_DECLARED_SECTION; }
    void setTrailingHashesSection() { flags |= FLAG_MASK_TRAILING_HASHES_SECTION; }
    bool isTrailingHashesSection() const { return flags & FLAG_MASK_TRAILING_HASHES_SECTION; }
};

} // m8r namespace

#endif /* M8R_MARKDOWN_AST_M8RUI_NAVIGATOR_NODE_H_ */
