/*
 markdown_configuration_representation.h     MindForger thinking notebook

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
#ifndef M8R_MARKDOWN_CONFIGURATION_REPRESENTATION_H
#define M8R_MARKDOWN_CONFIGURATION_REPRESENTATION_H

#include <string>
#include <vector>
#include <iostream>

#include "markdown.h"
#include "markdown_ast_node.h"
#include "../../config/configuration.h"

namespace m8r {

/**
 * @brief Markdown configuration representation.
 */
/* Method:
 *   Markdown (instance representing MD file)
 *     FILENAME -lexer->  LINES
 *     LINES    -lexer->  LEXEMS @ LEXER CTX
 *     LEXEMS   -parser-> AST @ PARSER CTX
 *     set(AST)
 *
 *   MarkdownConfigurationRepresentation (transcoder)
 *     from(AST) --> OUTLINE
 *       AST.getString(LEXEM) --> name, description, line, ...
 */
class MarkdownConfigurationRepresentation
{
    static constexpr int AVG_SECTION_SIZE = 300;
    static constexpr int AVG_CONFIGURATION_SIZE = 2*AVG_NOTE_SIZE;
private:
    Configuration config;

public:
    explicit MarkdownConfigurationRepresentation();
    MarkdownConfigurationRepresentation(const MarkdownConfigurationRepresentation&) = delete;
    MarkdownConfigurationRepresentation(const MarkdownConfigurationRepresentation&&) = delete;
    MarkdownConfigurationRepresentation &operator=(const MarkdownConfigurationRepresentation&) = delete;
    MarkdownConfigurationRepresentation &operator=(const MarkdownConfigurationRepresentation&&) = delete;
    ~MarkdownConfigurationRepresentation();

    virtual void configuration(const File& file);
    virtual std::string* to();

private:
    void configuration(std::vector<MarkdownAstNodeSection*>* ast);
};

}
#endif // M8R_MARKDOWN_CONFIGURATION_REPRESENTATION_H
