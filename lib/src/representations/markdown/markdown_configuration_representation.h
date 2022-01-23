/*
 markdown_configuration_representation.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

#include "markdown_document.h"
#include "markdown_repository_configuration_representation.h"
#include "../../config/configuration.h"
#include "../../persistence/configuration_persistence.h"

namespace m8r {

class Configuration;
class MarkdownAstNodeSection;

/**
 * @brief Markdown configuration representation.
 */
/* Method:
 *   Markdown (instance which represents MD file)
 *     FILENAME -lexer->  LINES
 *     LINES    -lexer->  LEXEMS @ LEXER CTX
 *     LEXEMS   -parser-> AST @ PARSER CTX
 *     set(AST)
 *
 *   MarkdownConfigurationRepresentation (transcoder)
 *     from(AST) --> OUTLINE
 *       AST.getString(LEXEM) --> name, description, line, ...
 */
/*
 * IMPROVE:
 *
 * - rewrite this class from source code to *object config representation*
 * - declare supported configuration item types:
 *   - boolean, int, float, enum
 * - configuration item to have metadata like section where it belongs
 * - instead of copy/pasting or writing code over and over again,
 *   introduce API to describe configuration - list of sections/items/...,
 * - ^ will avoid copy/paste errors, will make code short
 * - ^ will be Markdown-based configuration component which can be externalized
 *
 */
class MarkdownConfigurationRepresentation : public ConfigurationPersistence
{
private:
    static constexpr int AVG_SECTION_SIZE = 300;
    static constexpr int AVG_CONFIGURATION_SIZE = 2*AVG_SECTION_SIZE;

    MarkdownRepositoryConfigurationRepresentation mdRepositoryCfgRepresentation;

public:
    explicit MarkdownConfigurationRepresentation();
    MarkdownConfigurationRepresentation(const MarkdownConfigurationRepresentation&) = delete;
    MarkdownConfigurationRepresentation(const MarkdownConfigurationRepresentation&&) = delete;
    MarkdownConfigurationRepresentation &operator=(const MarkdownConfigurationRepresentation&) = delete;
    MarkdownConfigurationRepresentation &operator=(const MarkdownConfigurationRepresentation&&) = delete;
    virtual ~MarkdownConfigurationRepresentation();

    std::string* to(Configuration& c);

    /**
     * @brief Load configuration from file and return true on success (file exists), otherwise return false.
     */
    virtual bool load(Configuration& c);
    /**
     * @brief Save configuration to file.
     */
    virtual void save(Configuration& c) { save(nullptr, &c); }
    /**
     * @brief Save initial configuration file.
     */
    void save(const filesystem::File& file) { save(&file, nullptr); }

private:
    void configuration(std::vector<MarkdownAstNodeSection*>* ast, Configuration& c);
    void configurationSection(std::string* title, std::vector<std::string*>* body, Configuration& c);
    std::string& to(Configuration* c, std::string& md);
    void save(const filesystem::File* file, Configuration* c);
};

}
#endif // M8R_MARKDOWN_CONFIGURATION_REPRESENTATION_H
