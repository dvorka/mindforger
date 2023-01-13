/*
 filesystem_information_test.cpp     MindForger fullt text search test

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "../test_utils.h"
#include "../../../src/mind/dikw/filesystem_information.h"

#include "../../src/debug.h"

using namespace std;

extern char* getMindforgerGitHomePath();

/**
 * @brief Test PDFs indexation to library.
 */
TEST(FilesystemInformationTestCase, IndexPdfs) {
    // GIVEN
    // mock PDFs repository w/ empty files having various extensions
    string pdfsLibraryPath{
        string{getMindforgerGitHomePath()}
        + "/lib/test/resources/pdfs-library"
    };
    m8r::TestSandbox box{"", true};
    string mdFilename{"filesystem-information-pdf-indexation.md"};
    string mdFilePath{box.addMdFile(mdFilename)};
    cout << "PDFs library path: " << pdfsLibraryPath << endl;

    m8r::Ontology ontology{};
    m8r::MarkdownDocumentRepresentation mddr{ontology};

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance(); config.clear();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mtc-larmfr.md");
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(box.repositoryPath)),
        repositoryConfigRepresentation
    );
    cout << "Loading repository: " << config.getActiveRepository()->getPath() << endl;

    m8r::Mind mind(config);
    mind.learn();
    ASSERT_EQ(m8r::Configuration::MindState::SLEEPING, config.getMindState());
    mind.think().get();
    ASSERT_EQ(m8r::Configuration::MindState::THINKING, config.getMindState());
    cout << "Statistics:" << endl
    << "  Outlines: " << mind.remind().getOutlinesCount() << endl
    << "  Bytes   : " << mind.remind().getOutlineMarkdownsSize() << endl;
    ASSERT_LE(1, mind.remind().getOutlinesCount());

    // WHEN
    m8r::FilesystemInformationSource is{
        pdfsLibraryPath,
        mind,
        mddr
    };
    is.indexToMemory(*config.getActiveRepository());

    // THEN
    cout << "Indexed PDFs:" << endl;
    for(auto f:is.getPdfs()) {
        cout << "  " << *f << endl;
    }
    EXPECT_TRUE(is.getPdfs().size());
    // TODO assert Os descriptors existence
}
