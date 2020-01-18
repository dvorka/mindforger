/*
 markdown_configuration_representation.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "markdown_document.h"
#include "../../config/configuration.h"

namespace m8r {

class Configuration;
class MarkdownAstNodeSection;

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
    static constexpr int AVG_CONFIGURATION_SIZE = 2*AVG_SECTION_SIZE;

public:
    explicit MarkdownConfigurationRepresentation();
    MarkdownConfigurationRepresentation(const MarkdownConfigurationRepresentation&) = delete;
    MarkdownConfigurationRepresentation(const MarkdownConfigurationRepresentation&&) = delete;
    MarkdownConfigurationRepresentation &operator=(const MarkdownConfigurationRepresentation&) = delete;
    MarkdownConfigurationRepresentation &operator=(const MarkdownConfigurationRepresentation&&) = delete;
    ~MarkdownConfigurationRepresentation();

    std::string* to(Configuration& c);

    /**
     * @brief Load configuration from file and return true on success (file exists), otherwise return false.
     */
    bool load(Configuration& c);
    /**
     * @brief Save configuration to file.
     */
    void save(Configuration& c) { save(nullptr, &c); }
    /**
     * @brief Save initial configuration file.
     */
    void save(const File& file) { save(&file, nullptr); }

private:
    void configuration(std::vector<MarkdownAstNodeSection*>* ast, Configuration& c);
    void configuration(std::string* title, std::vector<std::string*>* body, Configuration& c);
    std::string& to(Configuration* c, std::string& md);
    void save(const File* file, Configuration* c);
};

}
#endif // M8R_MARKDOWN_CONFIGURATION_REPRESENTATION_H
