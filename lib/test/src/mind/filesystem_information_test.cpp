/*
 filesystem_information_test.cpp     MindForger fullt text search test

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

#include <gtest/gtest.h>

#include "../test_utils.h"
#include "../../../src/mind/dikw/filesystem_information.h"

#include "../../src/debug.h"

using namespace std;

TEST(FilesystemInformationTestCase, IndexPdfs) {
    // GIVEN
    // TODO mock PDFs repository w/ empty files having various extensions
    string pdfsLibraryPath{"/home/dvorka/mf/library"};
    m8r::TestSandbox box{"", true};
    string mdFilename{"filesystem-information-pdf-indexation.md"};
    string mdFilePath{box.addMdFile(mdFilename)};

    m8r::Ontology ontology{};
    m8r::MarkdownOutlineRepresentation mdor{ontology, nullptr};
    m8r::HtmlOutlineRepresentation htmlr{ontology, nullptr};
    m8r::FilesystemPersistence persistence{mdor, htmlr};
    m8r::MarkdownDocumentRepresentation mddr{ontology};

    m8r::Repository r{
        box.repositoryPath,
        m8r::Repository::RepositoryType::MINDFORGER,
        m8r::Repository::RepositoryMode::REPOSITORY,
        mdFilename
    };

    // WHEN
    m8r::FilesystemInformationSource is{
        pdfsLibraryPath,
        persistence,
        mddr
    };
    is.indexToMemory(r);

    // THEN
    cout << "Indexed PDFs:" << endl;
    for(auto f:is.getPdfs()) {
        cout << "  " << *f << endl;
    }
    EXPECT_TRUE(is.getPdfs().size());
    // TODO assert Os descriptors existence
}
