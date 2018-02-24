/*
 markdown_outline_representation.h     MindForger thinking notebook

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
#ifndef M8R_MARKDOWN_OUTLINE_REPRESENTATION_H_
#define M8R_MARKDOWN_OUTLINE_REPRESENTATION_H_

#include <string>
#include <cstdio>

#include "markdown.h"
#include "markdown_ast_node.h"
#include "../../model/outline.h"
#include "../../mind/ontology/ontology.h"
#include "../outline_representation.h"

namespace m8r {

/**
 * @brief Markdown outline representation.
 */
/* Method:
 *   Markdown (instance representing MD file)
 *     FILENAME -lexer->  LINES
 *     LINES    -lexer->  LEXEMS @ LEXER CTX
 *     LEXEMS   -parser-> AST @ PARSER CTX
 *     set(AST)
 *
 *   MarkdownOutlineRepresentation (transcoder)
 *     from(AST) --> OUTLINE
 *       AST.getString(LEXEM) --> title, description, line, ...
 *
 * Methods are virtual so that an inherited class may provide
 * e.g. a Markdown flavor or HTML implementations.
 */
class MarkdownOutlineRepresentation : public OutlineRepresentation
{
private:
    static constexpr int AVG_NOTE_SIZE = 500;
    static constexpr int AVG_OUTLINE_SIZE = 3*AVG_NOTE_SIZE;
    static const std::string MINIMAL_SECTION_HEADER;

    // tags, outline types and note types are dynamic (not fixed)
    Ontology& ontology;

public:
    explicit MarkdownOutlineRepresentation(Ontology& ontology);
    MarkdownOutlineRepresentation(const MarkdownOutlineRepresentation&) = delete;
    MarkdownOutlineRepresentation(const MarkdownOutlineRepresentation&&) = delete;
    MarkdownOutlineRepresentation &operator=(const MarkdownOutlineRepresentation&) = delete;
    MarkdownOutlineRepresentation &operator=(const MarkdownOutlineRepresentation&&) = delete;
    virtual ~MarkdownOutlineRepresentation();

    virtual Outline* outline(const File& file) override;
    virtual Outline* header(const std::string* md);
    virtual Note* note(const File& file);
    virtual Note* note(const std::string* md);

    virtual void description(const std::string* md, std::vector<std::string*>& description);

    virtual std::string* to(const Outline* outline);
    virtual std::string* to(const Outline* outline, std::string* md);
    virtual std::string* toHeader(const Outline* outline);
    virtual std::string* to(const Note* note);
    virtual std::string* to(const Note* note, std::string* md, bool includeMetadata=true);
    virtual std::string* toDescription(const Note* note, std::string* md);

private:
    Outline* outline(std::vector<MarkdownAstNodeSection*>* ast);
    Note* note(std::vector<MarkdownAstNodeSection*>* ast, const size_t astindex=0, Outline* outline=nullptr);
    void toHeader(const Outline* outline, std::string* md);
    std::string to(std::string* md, const std::vector<const Tag*>& tags);
};

} // m8r namespace

#endif /* M8R_MARKDOWN_OUTLINE_REPRESENTATION_H_ */
