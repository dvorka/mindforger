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
#include "../../../src/mind/mind.h"
#include "../../../src/gear/file_utils.h"

#include "../test_gear.cpp"

using namespace std;

extern char* getMindforgerGitHomePath();
extern void dumpOutline(m8r::Outline*&);

TEST(RepositoryIndexerTestCase, MindForgerRepository)
{
    string repositoryPath{"/tmp/mf-unit-repository-indexer-mf"};
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

    // test repository indexation
    m8r::RepositoryIndexer repositoryIndexer{};
    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repositoryIndexer.index(repository);
    repositoryIndexer.getRepository()->print();
    auto outlineFiles = repositoryIndexer.getAllOutlineFileNames();

    // asserts
    EXPECT_EQ(repository->getPath(), repositoryPath);
    EXPECT_EQ(repository->getType(), m8r::Repository::RepositoryType::MINDFORGER);
    EXPECT_EQ(repository->getRepositoryMode(), m8r::Repository::RepositoryMode::REPOSITORY);
    EXPECT_EQ(repository->isReadOnly(), false);
    EXPECT_EQ(2, outlineFiles.size());
    EXPECT_EQ(2, repositoryIndexer.getMarkdownFiles().size());
    EXPECT_EQ(0, repositoryIndexer.getNoteStencilsFileNames().size());
    EXPECT_EQ(0, repositoryIndexer.getOutlineStencilsFileNames().size());

    // test that metadata ARE written
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.setActiveRepository(config.addRepository(repository));
    m8r::Mind mind(config);
    mind.think();
    m8r::Memory& memory = mind.remind();
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* outline;
    cout << endl << "Outlines[" << outlines.size() << "]:";
    if(outlines.size()>0) {
        for(m8r::Outline*& o:outlines) {
            dumpOutline(o);
            if(o->getTitle() == string("First Outline")) {
                outline = o;
            }
        }
    } else {
        cout << endl << "NO OUTLINES";
    }

    // assert
    ASSERT_NE(outline, nullptr);
    // write outline > metadata to be written
    string* outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(outlineAsString->find("Metadata"), std::string::npos);
    delete outlineAsString;

    mind.remind().remember(outline);

    outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_NE(outlineAsString->find("Metadata"), std::string::npos);
    delete outlineAsString;
}

TEST(RepositoryIndexerTestCase, MarkdownRepository)
{
    string repositoryPath{"/tmp/mf-unit-repository-indexer-md"};
    string path, content;

    m8r::removeDirectoryRecursively(repositoryPath.c_str());
    int e = mkdir(repositoryPath.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
    ASSERT_EQ(e, 0);

    path.assign(repositoryPath+"/first.md");
    content.assign(
        "# First Markdown"
        "\n"
        "\nFirst outline text."
        "\n"
        "\n## Note 1"
        "\nNote 1 text."
        "\n"
        "\n## Note 2"
        "\nNote 2 text."
        "\n");
    m8r::stringToFile(path, content);

    path.assign(repositoryPath+"/second.md");
    content.assign(
        "# Second Markdown"
        "\n"
        "\nSecond outline text."
        "\n"
        "\n## Note 11"
        "\nNote 11 text."
        "\n"
        "\n## Note 22"
        "\nNote 22 text."
        "\n");
    m8r::stringToFile(path, content);

    // test repository indexation
    m8r::RepositoryIndexer repositoryIndexer{};
    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repositoryIndexer.index(repository);
    repositoryIndexer.getRepository()->print();
    auto outlineFiles = repositoryIndexer.getAllOutlineFileNames();

    // asserts
    EXPECT_EQ(repository->getPath(), repositoryPath);
    EXPECT_EQ(repository->getType(), m8r::Repository::RepositoryType::MARKDOWN);
    EXPECT_EQ(repository->getRepositoryMode(), m8r::Repository::RepositoryMode::REPOSITORY);
    EXPECT_EQ(repository->isReadOnly(), false);
    EXPECT_EQ(2, outlineFiles.size());
    EXPECT_EQ(2, repositoryIndexer.getMarkdownFiles().size());
    EXPECT_EQ(0, repositoryIndexer.getNoteStencilsFileNames().size());
    EXPECT_EQ(0, repositoryIndexer.getOutlineStencilsFileNames().size());

    // test that metadata ARE written
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.setActiveRepository(config.addRepository(repository));
    m8r::Mind mind(config);
    mind.think();
    m8r::Memory& memory = mind.remind();
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* outline;
    cout << endl << "Outlines[" << outlines.size() << "]:";
    if(outlines.size()>0) {
        for(m8r::Outline*& o:outlines) {
            dumpOutline(o);
            if(o->getTitle() == string("First Outline")) {
                outline = o;
            }
        }
    } else {
        cout << endl << "NO OUTLINES";
    }

    // assert
    ASSERT_NE(outline, nullptr);
    // write outline > metadata to be written
    string* outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(outlineAsString->find("Metadata"), std::string::npos);
    delete outlineAsString;

    mind.remind().remember(outline);

    outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(outlineAsString->find("Metadata"), std::string::npos);
    delete outlineAsString;
}

TEST(RepositoryIndexerTestCase, MindForgerFile)
{
    // TODO TBD

    // TODO parser must detect metadata occurence, Outline to have a hint to be used on serialization
    // hint to be called 'format' enum - MF, MD (w/ or w/o)
}

TEST(RepositoryIndexerTestCase, MarkdownFile)
{
    // TODO TBD

    // assert size
    // TODO test and assert that metadata are NOT written
}
