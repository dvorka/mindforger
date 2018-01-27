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
#ifndef M8R_MARKDOWN_H_
#define M8R_MARKDOWN_H_

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
 * Represents Markdown file (title, sections, paragraphs, bullet lists, etc.)
 * that has no notion of MindForger data model (outlines, notes, ...).
 */
class Markdown
{
private:
    const std::string* filePath;
    unsigned fileSize;
    time_t modified;

    /**
     * @brief Markdown root section title.
     */
    std::string title;
    std::vector<MarkdownAstNodeSection*>* ast;

public:
    explicit Markdown(const std::string* filePath);
    Markdown() = delete;
    Markdown(const Markdown &) = delete;
    Markdown(const Markdown &&) = delete;
    Markdown &operator=(const Markdown &) = delete;
    Markdown &operator=(const Markdown &&) = delete;
    virtual ~Markdown();

    void from();
    void from(const std::string* text);
    bool isParsed() const { return ast==nullptr; }
    void clear();

    const std::string* getFilePath() const;
    unsigned getFileSize() const;
    time_t getModified() const { return modified; }
    std::string* getTitle();
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


} /* namespace m8r */

#endif /* M8R_MARKDOWN_H_ */
