/*
 memory_test.cpp     MindForger memory test

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "../../../src/gear/file_utils.h"
#include "../../../src/install/installer.h"
#include "../../../src/mind/mind.h"

using namespace std;

namespace {

/**
 * @brief Create repository w/ 1 Outline, learn it and return Mind.
 */
m8r::Mind* createExportTestMind(const string& repositoryDir, const string& configPath)
{
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    m8r::stringToFile(
        repositoryDir+"/memory/o1.md",
        "# Notebook to Export"
        "\nDescription of the Notebook."
        "\n## Note to Export"
        "\nText of the Note."
        "\n");

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath(configPath);
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryDir)),
        repositoryConfigRepresentation
    );
    m8r::Mind* mind = new m8r::Mind{config};
    mind->learn();
    mind->think().get();

    return mind;
}

} // anonymous namespace

TEST(MemoryTestCase, ExportOutlineToMarkdown)
{
    // GIVEN a Notebook to be exported
    unique_ptr<m8r::Mind> mind{createExportTestMind(
        "/tmp/mf-unit-repository-export-md", "/tmp/cfg-export-md.md")};
    ASSERT_EQ(1, mind->remind().getOutlinesCount());
    m8r::Outline* o = mind->getOutlines()[0];
    string exportFile{"/tmp/mf-unit-export.md"};
    std::remove(exportFile.c_str());

    // WHEN exported to a writeable path
    bool exported = mind->remind().exportToMarkdown(o, exportFile);

    // THEN success is reported and the file has the Notebook in it
    EXPECT_TRUE(exported);
    ASSERT_TRUE(m8r::isFile(exportFile.c_str()));
    unique_ptr<string> exportedMd{m8r::fileToString(exportFile)};
    ASSERT_NE(nullptr, exportedMd.get());
    EXPECT_NE(std::string::npos, exportedMd->find("Notebook to Export"));
    EXPECT_NE(std::string::npos, exportedMd->find("Note to Export"));

    // WHEN exported to a path which CANNOT be written - its parent
    // directory does not exist (unlike a read-only location, this fails
    // for any user, so the test stays deterministic)
    string unwriteableFile{"/tmp/mf-unit-export-no-such-dir/export.md"};
    m8r::removeDirectoryRecursively("/tmp/mf-unit-export-no-such-dir");
    bool exportedToUnwriteable = mind->remind().exportToMarkdown(o, unwriteableFile);

    // THEN the failure is reported - w/o it the UI closes the export
    // dialog and tells the user that a non-existent file was written
    EXPECT_FALSE(exportedToUnwriteable);
    EXPECT_FALSE(m8r::isFile(unwriteableFile.c_str()));
}

TEST(MemoryTestCase, ExportOutlineToHtml)
{
    // GIVEN a Notebook to be exported
    unique_ptr<m8r::Mind> mind{createExportTestMind(
        "/tmp/mf-unit-repository-export-html", "/tmp/cfg-export-html.md")};
    ASSERT_EQ(1, mind->remind().getOutlinesCount());
    m8r::Outline* o = mind->getOutlines()[0];
    string exportFile{"/tmp/mf-unit-export.html"};
    std::remove(exportFile.c_str());

    // WHEN exported to a writeable path
    bool exported = mind->remind().exportToHtml(o, exportFile);

    // THEN success is reported and the file has the Notebook in it
    EXPECT_TRUE(exported);
    ASSERT_TRUE(m8r::isFile(exportFile.c_str()));
    unique_ptr<string> exportedHtml{m8r::fileToString(exportFile)};
    ASSERT_NE(nullptr, exportedHtml.get());
    EXPECT_NE(std::string::npos, exportedHtml->find("Notebook to Export"));

    // WHEN exported to a path which CANNOT be written
    string unwriteableFile{"/tmp/mf-unit-export-no-such-dir/export.html"};
    m8r::removeDirectoryRecursively("/tmp/mf-unit-export-no-such-dir");
    bool exportedToUnwriteable = mind->remind().exportToHtml(o, unwriteableFile);

    // THEN the failure is reported
    EXPECT_FALSE(exportedToUnwriteable);
    EXPECT_FALSE(m8r::isFile(unwriteableFile.c_str()));
}
