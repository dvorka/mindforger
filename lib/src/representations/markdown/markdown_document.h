/*
 markdown.h     MindForger thinking notebook

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
#ifndef M8R_MARKDOWN_DOCUMENT_H_
#define M8R_MARKDOWN_DOCUMENT_H_

#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../../gear/string_utils.h"
#include "markdown_ast_node.h"
#include "markdown_lexer_sections.h"
#include "markdown_parser_sections.h"
#include "markdown_outline_metadata.h"

namespace m8r {

class MarkdownAstNodeSection;

/**
 * @brief Markdown file representation.
 *
 * Represents Markdown file (name, sections, paragraphs, bullet lists, etc.)
 * that has no notion of MindForger data model (outlines, notes, ...).
 */
class MarkdownDocument
{
public:
    /**
     * @brief Markdown format.
     *
     * MINDFORGER indicates MindForger DSL (w/ metadata) and MARKDOWN
     * plain Markdown file w/o any enhancements.
     */
    enum Format {
        MINDFORGER = 1,
        MARKDOWN = 2
    };

private:
    const std::string* filePath;
    Format format;
    unsigned fileSize;
    time_t modified;

    /**
     * @brief Markdown root section name.
     */
    std::string name;
    std::vector<MarkdownAstNodeSection*>* ast;

public:
    explicit MarkdownDocument(const std::string* filePath);
    MarkdownDocument() = delete;
    MarkdownDocument(const MarkdownDocument &) = delete;
    MarkdownDocument(const MarkdownDocument &&) = delete;
    MarkdownDocument &operator=(const MarkdownDocument &) = delete;
    MarkdownDocument &operator=(const MarkdownDocument &&) = delete;
    virtual ~MarkdownDocument();

    void from();
    void from(const std::string* text);
    bool isParsed() const { return ast==nullptr; }
    void clear();

    const std::string* getFilePath() const;
    Format getFormat() const { return format; }
    unsigned getFileSize() const;
    time_t getModified() const { return modified; }
    std::string* getName();
    /**
     * @brief Get AST for read only processing.
     */
    std::vector<MarkdownAstNodeSection*>* getAst() const { return ast; }
    /**
     * @brief Get AST to disassemble it in order to create an instance efficiently.
     */
    std::vector<MarkdownAstNodeSection*>* moveAst() {
        std::vector<MarkdownAstNodeSection*>* result = ast;
        ast = nullptr;
        return result;
    }

private:
    void from(const std::vector<MarkdownAstNodeSection*>* ast);
};


} // m8r namespace

#endif /* M8R_MARKDOWN_DOCUMENT_H_ */
