/*
 file_utils_test.cpp     MindForger application test

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

#include <gtest/gtest.h>

#include "../../../src/gear/file_utils.h"
#include "../../../src/install/installer.h"

using namespace std;

TEST(FileGearTestCase, FilesystemPath)
{
    // GIVEN
    string strBasePath{FILE_PATH_SEPARATOR};
    strBasePath.append("a");
    strBasePath.append(FILE_PATH_SEPARATOR);
    strBasePath.append("b");

    string expectedPath{strBasePath};
    expectedPath.append(FILE_PATH_SEPARATOR);
    expectedPath.append("c");

    m8r::filesystem::Path basePath{strBasePath};

    // WHEN
    m8r::filesystem::Path path = basePath / "c";

    // THEN
    ASSERT_EQ(expectedPath, path.toString());
}

using namespace m8r::filesystem;

TEST(FileGearTestCase, FilesystemPathUsage)
{
    // base path
    cout << Path{"/a/b"}
         << endl;

    // assembled path
    cout << Path{"/a/b"} / "c" / "d"
         << endl;
}

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

TEST(FileGearTestCase, StringToFileReportsWriteFailure)
{
    // GIVEN a writeable path and a path which CANNOT be written - its
    // parent directory does not exist (unlike a read-only location, this
    // fails for any user, so the test stays deterministic)
    string writeableFile{"/tmp/mf-unit-string-to-file.md"};
    std::remove(writeableFile.c_str());
    string unwriteableFile{"/tmp/mf-unit-string-to-file-no-such-dir/file.md"};
    m8r::removeDirectoryRecursively("/tmp/mf-unit-string-to-file-no-such-dir");
    string content{"# Notebook\n\nText.\n"};

    // WHEN
    bool written = m8r::stringToFile(writeableFile, content);
    bool notWritten = m8r::stringToFile(unwriteableFile, content);

    // THEN: the successful write is reported and round-trips
    EXPECT_TRUE(written);
    string* readBack = m8r::fileToString(writeableFile);
    ASSERT_NE(nullptr, readBack);
    EXPECT_EQ(content, *readBack);
    delete readBack;

    // AND: the failed write is reported instead of being swallowed
    EXPECT_FALSE(notWritten);
    EXPECT_FALSE(m8r::isFile(unwriteableFile.c_str()));
}

TEST(FileGearTestCase, GuiSessionAvailableForX11OrWayland)
{
    // GIVEN the four combinations of DISPLAY (X11) and WAYLAND_DISPLAY
    // values seen across desktop sessions - including a Flatpak sandbox
    // on a native Wayland compositor, where only WAYLAND_DISPLAY is set
    // and DISPLAY is deliberately left empty by the sandbox

    // WHEN neither is set (plain text console/tty)
    // THEN no GUI session is detected
    EXPECT_FALSE(m8r::isGuiSessionAvailable(nullptr, nullptr));
    EXPECT_FALSE(m8r::isGuiSessionAvailable("", ""));

    // WHEN only DISPLAY is set (X11 session)
    // THEN a GUI session is detected
    EXPECT_TRUE(m8r::isGuiSessionAvailable(":0", nullptr));

    // WHEN only WAYLAND_DISPLAY is set (native Wayland session, e.g. Flatpak)
    // THEN a GUI session is detected
    EXPECT_TRUE(m8r::isGuiSessionAvailable(nullptr, "wayland-0"));

    // WHEN both are set (XWayland session)
    // THEN a GUI session is detected
    EXPECT_TRUE(m8r::isGuiSessionAvailable(":0", "wayland-0"));
}
