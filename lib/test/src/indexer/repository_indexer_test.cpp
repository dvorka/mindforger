/*
 repository_indexer_test.cpp     MindForger indexer test

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

#include "../../../src/repository_indexer.h"

#include "../test_gear.cpp"

using namespace std;

extern char* getMindforgerGitHomePath();

TEST(RepositoryIndexerTestCase, MindForgerRepository)
{
    std::string repositoryPath("/lib/test/resources/basic-repository");
    repositoryPath.insert(0, getMindforgerGitHomePath());

    // test
    m8r::RepositoryIndexer repositoryIndexer{};
    repositoryIndexer.index(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath));
    auto outlineFiles = repositoryIndexer.getAllOutlineFileNames();

    // asserts
    EXPECT_EQ(3, outlineFiles.size());
}


// TODO make test BELOW MF repo test, for MD do NOT call creation of empty repo!



TEST(RepositoryIndexerTestCase, MarkdownRepository)
{
    string repositoryPath{"/tmp/mf-unit-repository-indexer"};
    map<string,string> pathToContent;
    string path, content;

    path.assign(repositoryPath+"/memory/first.md");
    content.assign(
        "# First Outline"
        "\n"
        "\nFirst outline text."
        "\n"
        "\n## Note 1"
        "\nNote 1 text."
        "\n"
        "\n## Note 2"
        "\nNote 2 text."
        "\n");
    pathToContent[path] = content;

    path.assign(repositoryPath+"/memory/second.md");
    content.assign(
        "# Second Outline"
        "\n"
        "\nSecond outline text."
        "\n"
        "\n## Note 11"
        "\nNote 11 text."
        "\n"
        "\n## Note 22"
        "\nNote 22 text."
        "\n");
    pathToContent[path] = content;

    m8r::createEmptyRepository(repositoryPath, pathToContent);

    // test
    m8r::RepositoryIndexer repositoryIndexer{};
    repositoryIndexer.index(m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath));
    repositoryIndexer.getRepository()->print();
    auto outlineFiles = repositoryIndexer.getAllOutlineFileNames();

    // asserts
    EXPECT_EQ(2, outlineFiles.size());

    // TODO test and assert that metadata are NOT written
}

TEST(RepositoryIndexerTestCase, MindForgerFile)
{
    // TODO TBD
}

TEST(RepositoryIndexerTestCase, MarkdownFile)
{
    // TODO TBD

    // assert size
    // TODO test and assert that metadata are NOT written
}
