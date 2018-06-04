/*
 file_utils_test.cpp     MindForger application test

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

#include <gtest/gtest.h>

#include "../../../src/gear/file_utils.h"
#include "../../../src/install/installer.h"

using namespace std;

TEST(FileGearTestCase, DeepCopy)
{
    string srcRepositoryDir{"/tmp/mf-file-gear-repository-SRC"};
    m8r::removeDirectoryRecursively(srcRepositoryDir.c_str());
    string dstRepositoryDir{"/tmp/mf-file-gear-repository-DST"};
    m8r::removeDirectoryRecursively(dstRepositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(srcRepositoryDir);
    // O
    string oFile{srcRepositoryDir+"/memory/mindforger-doc/o1.md"};
    string oContent{"# Test Outline\n\nOutline text.\n\n## Note 1\nNote 1 text.\n"};
    m8r::createDirectory(string{srcRepositoryDir+"/memory/mindforger-doc"});
    m8r::stringToFile(oFile,oContent);
    // O stencil
    oFile.assign(srcRepositoryDir+"/stencils/notebooks/s-o1.md");
    oContent.assign("# Test Outline\n\nOutline text.\n\n## Note 1\nNote 1 text.\n");
    m8r::stringToFile(oFile,oContent);
    // N stencil
    oFile.assign(srcRepositoryDir+"/stencils/notes/s-n1.md");
    oContent.assign("# Test Note\n\nNote stencil text.\n\n");
    m8r::stringToFile(oFile,oContent);
    string p;

    // asserts
    p.assign(srcRepositoryDir); p.append("/memory/mindforger-doc/o1.md");
    ASSERT_TRUE(m8r::isDirectoryOrFileExists(p.c_str()));
    p.assign(srcRepositoryDir); p.append("/stencils/notebooks/s-o1.md");
    ASSERT_TRUE(m8r::isDirectoryOrFileExists(p.c_str()));

    // test
    m8r::copyDirectoryRecursively(srcRepositoryDir.c_str(), dstRepositoryDir.c_str());

    // asserts
    p.assign(dstRepositoryDir); p.append("/memory/mindforger-doc/o1.md");
    ASSERT_TRUE(m8r::isDirectoryOrFileExists(p.c_str()));
    p.assign(dstRepositoryDir); p.append("/stencils/notebooks/s-o1.md");
    ASSERT_TRUE(m8r::isDirectoryOrFileExists(p.c_str()));
}

// deep copy of the real production documentation - this test to be disabled and run MANUALLY
TEST(FileGearTestCase, DISABLED_DeepProductionCopy)
{
    string srcRepositoryDir{"/usr/share/doc/mindforger"};
    m8r::removeDirectoryRecursively(srcRepositoryDir.c_str());
    string dstRepositoryDir{"/tmp/mf-file-gear-REAL-doc"};
    m8r::removeDirectoryRecursively(dstRepositoryDir.c_str());
    string p;

    // asserts
    p.assign("/usr/share/doc/mindforger/README.md");
    ASSERT_TRUE(m8r::isDirectoryOrFileExists(p.c_str()));

    // test
    m8r::copyDirectoryRecursively(srcRepositoryDir.c_str(), dstRepositoryDir.c_str(), true);

    // asserts
    p.assign(dstRepositoryDir); p.append("/stencils/notebook/swot.md");
    ASSERT_TRUE(m8r::isDirectoryOrFileExists(p.c_str()));
}

TEST(FileGearTestCase, DeepCopyToExisting)
{
    string srcRepositoryDir{"/tmp/mf-file-gear-repository-SRCE"};
    m8r::removeDirectoryRecursively(srcRepositoryDir.c_str());
    string dstRepositoryDir{"/tmp/mf-file-gear-repository-DSTE"};
    m8r::removeDirectoryRecursively(dstRepositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(srcRepositoryDir);
    installer.createEmptyMindForgerRepository(dstRepositoryDir);
    // O
    string oFile{srcRepositoryDir+"/memory/mindforger-doc/o1.md"};
    string oContent{"# Test Outline\n\nOutline text.\n\n## Note 1\nNote 1 text.\n"};
    m8r::createDirectory(string{srcRepositoryDir+"/memory/mindforger-doc"});
    m8r::stringToFile(oFile,oContent);
    // O stencil
    oFile.assign(srcRepositoryDir+"/stencils/notebooks/s-o1.md");
    oContent.assign("# Test Outline\n\nOutline text.\n\n## Note 1\nNote 1 text.\n");
    m8r::stringToFile(oFile,oContent);
    // N stencil
    oFile.assign(srcRepositoryDir+"/stencils/notes/s-n1.md");
    oContent.assign("# Test Note\n\nNote stencil text.\n\n");
    m8r::stringToFile(oFile,oContent);
    string p;

    // asserts
    p.assign(srcRepositoryDir); p.append("/memory/mindforger-doc/o1.md");
    ASSERT_TRUE(m8r::isDirectoryOrFileExists(p.c_str()));
    p.assign(srcRepositoryDir); p.append("/stencils/notebooks/s-o1.md");
    ASSERT_TRUE(m8r::isDirectoryOrFileExists(p.c_str()));

    // test
    m8r::copyDirectoryRecursively(srcRepositoryDir.c_str(), dstRepositoryDir.c_str());

    // asserts
    p.assign(dstRepositoryDir); p.append("/memory/mindforger-doc/o1.md");
    ASSERT_TRUE(m8r::isDirectoryOrFileExists(p.c_str()));
    p.assign(dstRepositoryDir); p.append("/stencils/notebooks/s-o1.md");
    ASSERT_TRUE(m8r::isDirectoryOrFileExists(p.c_str()));
}
