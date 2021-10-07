/*
 markdown_configuration_representation.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_MARKDOWN_REPOSITORY_CONFIGURATION_REPRESENTATION_H
#define M8R_MARKDOWN_REPOSITORY_CONFIGURATION_REPRESENTATION_H

#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "markdown_document.h"
#include "../../config/configuration.h"
#include "../../model/organizer.h"
#include "../../persistence/configuration_persistence.h"

namespace m8r {

class Configuration;
class MarkdownAstNodeSection;

/**
 * @brief Markdown repository configuration representation.
 *
 * Markdown repository configuration hosts repository specific settings
 * like organizers or outline tree.
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
class MarkdownRepositoryConfigurationRepresentation : public RepositoryConfigurationPersistence
{
    static constexpr int AVG_SECTION_SIZE = 300;
    static constexpr int AVG_CONFIGURATION_SIZE = 2*AVG_SECTION_SIZE;

public:
    explicit MarkdownRepositoryConfigurationRepresentation();
    MarkdownRepositoryConfigurationRepresentation(const MarkdownRepositoryConfigurationRepresentation&) = delete;
    MarkdownRepositoryConfigurationRepresentation(const MarkdownRepositoryConfigurationRepresentation&&) = delete;
    MarkdownRepositoryConfigurationRepresentation &operator=(const MarkdownRepositoryConfigurationRepresentation&) = delete;
    MarkdownRepositoryConfigurationRepresentation &operator=(const MarkdownRepositoryConfigurationRepresentation&&) = delete;
    virtual ~MarkdownRepositoryConfigurationRepresentation();

    std::string* to(Configuration& c);

    /**
     * @brief Load repository configuration from file and return true on success (file exists), otherwise return false.
     */
    virtual bool load(Configuration& c);
    /**
     * @brief Save repository configuration to file.
     */
    virtual void save(Configuration& c) { save(nullptr, &c); }
    /**
     * @brief Save initial repository configuration file.
     */
    void save(const File& file) { save(&file, nullptr); }

private:
    void repositoryConfiguration(std::vector<MarkdownAstNodeSection*>* ast, Configuration& c);
    void repositoryConfigurationSection(std::string* title, std::vector<std::string*>* body, Configuration& c);
    void repositoryConfigurationSectionOrganizers(std::vector<std::string*>* body, Configuration& c);
    Organizer* repositoryConfigurationSectionOrganizerAdd(Organizer* o, std::set<std::string>& keys, Configuration& c);
    std::string& to(Configuration* c, std::string& md);
    void save(const File* file, Configuration* c);
};

}
#endif // M8R_MARKDOWN_REPOSITORY_CONFIGURATION_REPRESENTATION_H
