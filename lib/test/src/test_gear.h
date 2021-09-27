/*
 test_gear.h     MindForger thinking notebook

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
#ifndef M8R_TEST_GEAR_H
#define M8R_TEST_GEAR_H

#include <string>
#include <map>

#include "../../src/model/outline.h"
#include "../../src/persistence/filesystem_persistence.h"
#include "../../src/representations/html/html_outline_representation.h"

namespace m8r {

struct TestSandbox {
    // Path to home directory.
    std::string testHomePath;
    // Name of MindForger configuration file.
    std::string configFileName;
    // Path to MindForger configuration path.
    std::string configPath;
    // Path to MindForger repository (empty string if no repository).
    std::string repositoryPath;
    // Is sandbox MindForger repository or MindForger config only?
    bool isRepository;

    /**
     * @brief Create new test sandbox.
     *
     * @param configFileName    MindForger config file name (empty string for default name).
     */
    explicit TestSandbox(std::string configFileName, bool repository=false);

    /**
     * @brief Create new MD file test sandbox.
     *
     * This method creates home directory with MindForger configuration file only.
     * MindForger repository is not created in this case i.e. sanbox is meant for
     * testing of Markdown file or directory testing.
     * Information about previous sandbox is rewritten/lost.
     *
     * @param configFileName    MindForger config file name.
     */
    std::string create(std::string configFileName="");
    /**
     * @brief Create new MindForger repository test sandbox.
     *
     * This method creates home directory with MindForger repository.
     * Information about previous sandbox is rewritten/lost.
     *
     * @param configFileName    MindForger config file name.
     */
    std::string createRepository(std::string configFileName="");
    std::string addMdFile(std::string fileName, std::string content="");
};

class HtmlColorsMock : public m8r::HtmlColorsRepresentation {
private:
    std::string textColor;
    std::string backgroundColor;
public:
    virtual std::string& getHtmlTextColor() { return textColor; }
    virtual std::string& getHtmlBackgroundColor() { return backgroundColor; }
};

void printOutlineNotes(Outline* o);
void printLexemType(MarkdownLexemType type);
void printLexems(const std::vector<MarkdownLexem*>& lexems);
void printAst(const std::vector<MarkdownAstNodeSection*>* ast);

void createEmptyRepository(std::string& repositoryDir, std::map<std::string,std::string>& pathToContent);

/**
 * @brief Create new test home in system's temp directory.
 *
 * Unique name of the home directory is created:
 *
 *   /tmp/mindforger-test/home-123
 *
 * ... and path to the home directory is returned.
 */
std::string createTestHome(std::string& tmpPath);

}
#endif // M8R_TEST_GEAR_H
