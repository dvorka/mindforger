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

TEST(RepositoryIndexerTestCase, RepositoryTypeDetection)
{
    string repositoryPath, directoryName, fileName;

    // MF repository
    repositoryPath.assign("/lib/test/resources/basic-repository");
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::Repository* r = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    ASSERT_NE(r, nullptr);
    EXPECT_EQ(r->getType(),m8r::Repository::RepositoryType::MINDFORGER);
    EXPECT_EQ(r->getMode(),m8r::Repository::RepositoryMode::REPOSITORY);
    EXPECT_EQ(r->getPath(),repositoryPath);
    EXPECT_TRUE(r->getFile().empty());
    delete r;

    // MD repository
    repositoryPath.assign("/lib/test/resources/basic-repository/memory");
    repositoryPath.insert(0, getMindforgerGitHomePath());
    r = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    ASSERT_NE(r, nullptr);
    EXPECT_EQ(r->getType(),m8r::Repository::RepositoryType::MARKDOWN);
    EXPECT_EQ(r->getMode(),m8r::Repository::RepositoryMode::REPOSITORY);
    EXPECT_EQ(r->getPath(),repositoryPath);
    EXPECT_TRUE(r->getFile().empty());
    delete r;

    // MF file
    repositoryPath.assign("/lib/test/resources/basic-repository/memory/flat-metadata.md");
    repositoryPath.insert(0, getMindforgerGitHomePath());
    directoryName.assign("/lib/test/resources/basic-repository/memory");
    directoryName.insert(0, getMindforgerGitHomePath());
    fileName.assign("flat-metadata.md");
    r = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    ASSERT_NE(r, nullptr);
    EXPECT_EQ(r->getType(),m8r::Repository::RepositoryType::MINDFORGER);
    EXPECT_EQ(r->getMode(),m8r::Repository::RepositoryMode::FILE);
    EXPECT_EQ(r->getPath(),directoryName);
    EXPECT_EQ(r->getFile(),fileName);
    delete r;

    // MD file
    repositoryPath.assign("/lib/test/resources/basic-repository/memory/no-metadata.md");
    repositoryPath.insert(0, getMindforgerGitHomePath());
    directoryName.assign("/lib/test/resources/basic-repository/memory");
    directoryName.insert(0, getMindforgerGitHomePath());
    fileName.assign("no-metadata.md");
    r = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    ASSERT_NE(r, nullptr);
    // it's OK - MF is default type BEFORE parsing (MF doesn't delete metadata, MD would erase them)
    EXPECT_EQ(r->getType(),m8r::Repository::RepositoryType::MINDFORGER);
    EXPECT_EQ(r->getMode(),m8r::Repository::RepositoryMode::FILE);
    EXPECT_EQ(r->getPath(),directoryName);
    EXPECT_EQ(r->getFile(),fileName);
    delete r;
}

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
    EXPECT_EQ(repository->getMode(), m8r::Repository::RepositoryMode::REPOSITORY);
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
            ASSERT_EQ(o->getFormat(), m8r::Markdown::Format::MINDFORGER);
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
    EXPECT_EQ(repository->getMode(), m8r::Repository::RepositoryMode::REPOSITORY);
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
            ASSERT_EQ(o->getFormat(), m8r::Markdown::Format::MARKDOWN);
            if(o->getTitle() == string("First Markdown")) {
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

    outline->setTitle("Dirty");
    mind.remind().remember(outline);

    outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(outlineAsString->find("Metadata"), std::string::npos);
    delete outlineAsString;
}

TEST(RepositoryIndexerTestCase, MindForgerFile)
{
    string repositoryPath{"/tmp"};
    string path, content;

    path.assign(repositoryPath+"/repository-indexer-single-mf-file.md");
    remove(path.c_str());

    content.assign(
        "# First Outline <!-- Metadata: type: Outline; created: 2017-02-02 06:31:09; reads: 35; read: 2018-01-07 08:05:58; revision: 36; modified: 2018-01-07 08:06:05; importance: 0/5; urgency: 0/5; progress: 0%; -->"
        "\n"
        "\nFirst outline text."
        "\n"
        "\n## Note 1 <!-- Metadata: type: Note; created: 2017-02-02 06:31:09; reads: 35; read: 2018-01-07 08:05:58; revision: 36; modified: 2018-01-07 08:06:05; progress: 20%; -->"
        "\nNote 1 text."
        "\n"
        "\n## Note 2 <!-- Metadata: type: Idea; created: 2017-02-02 06:31:09; reads: 1; read: 2018-01-07 08:05:58; revision: 1; modified: 2018-01-07 08:06:05; progress: 10%; -->"
        "\nNote 2 text."
        "\n");
    m8r::stringToFile(path, content);

    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    // repository type will be determined by mind.think() > memory.learn()
    repository->setMode(m8r::Repository::RepositoryMode::FILE);
    repository->setFile(path);

    m8r::RepositoryIndexer repositoryIndexer{};
    repositoryIndexer.index(repository);
    repositoryIndexer.getRepository()->print();

    EXPECT_EQ(1, repositoryIndexer.getAllOutlineFileNames().size());
    EXPECT_EQ(1, repositoryIndexer.getMarkdownFiles().size());
    EXPECT_EQ(0, repositoryIndexer.getNoteStencilsFileNames().size());
    EXPECT_EQ(0, repositoryIndexer.getOutlineStencilsFileNames().size());

    // test that metadata ARE written
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.setActiveRepository(config.addRepository(repository));
    m8r::Mind mind(config);
    mind.think();

    EXPECT_EQ(repository->getPath(), repositoryPath);
    EXPECT_EQ(repository->getType(), m8r::Repository::RepositoryType::MINDFORGER);
    EXPECT_EQ(repository->getMode(), m8r::Repository::RepositoryMode::FILE);
    EXPECT_EQ(repository->isReadOnly(), false);

    m8r::Memory& memory = mind.remind();
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* outline;
    cout << endl << "Outlines[" << outlines.size() << "]:";
    if(outlines.size()>0) {
        for(m8r::Outline*& o:outlines) {
            dumpOutline(o);
            ASSERT_EQ(o->getFormat(), m8r::Markdown::Format::MINDFORGER);
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
    EXPECT_NE(outlineAsString->find("Metadata"), std::string::npos);
    delete outlineAsString;

    outline->setTitle("Dirty");
    mind.remind().remember(outline);

    outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_NE(outlineAsString->find("Metadata"), std::string::npos);
    delete outlineAsString;
}

TEST(RepositoryIndexerTestCase, MarkdownFile)
{
    string repositoryPath{"/tmp"};
    string path, content;

    path.assign(repositoryPath+"/repository-indexer-single-md-file.md");
    remove(path.c_str());

    content.assign(
        "# First Markdown"
        "\n"
        "\nFirst MD text."
        "\n"
        "\n## Note 1"
        "\nNote 1 text."
        "\n"
        "\n## Note 2"
        "\nNote 2 text."
        "\n");
    m8r::stringToFile(path, content);

    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    // repository type will be determined by mind.think() > memory.learn()
    repository->setMode(m8r::Repository::RepositoryMode::FILE);
    repository->setFile(path);

    m8r::RepositoryIndexer repositoryIndexer{};
    repositoryIndexer.index(repository);
    repositoryIndexer.getRepository()->print();

    EXPECT_EQ(1, repositoryIndexer.getAllOutlineFileNames().size());
    EXPECT_EQ(1, repositoryIndexer.getMarkdownFiles().size());
    EXPECT_EQ(0, repositoryIndexer.getNoteStencilsFileNames().size());
    EXPECT_EQ(0, repositoryIndexer.getOutlineStencilsFileNames().size());

    // test that metadata ARE written
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.setActiveRepository(config.addRepository(repository));
    m8r::Mind mind(config);
    mind.think();

    EXPECT_EQ(repository->getPath(), repositoryPath);
    EXPECT_EQ(repository->getType(), m8r::Repository::RepositoryType::MARKDOWN);
    EXPECT_EQ(repository->getMode(), m8r::Repository::RepositoryMode::FILE);
    EXPECT_EQ(repository->isReadOnly(), false);

    m8r::Memory& memory = mind.remind();
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* outline;
    cout << endl << "Outlines[" << outlines.size() << "]:";
    if(outlines.size()>0) {
        for(m8r::Outline*& o:outlines) {
            dumpOutline(o);
            ASSERT_EQ(o->getFormat(), m8r::Markdown::Format::MARKDOWN);
            if(o->getTitle() == string("First Markdown")) {
                outline = o;
            }
        }
    } else {
        cout << endl << "NO MDs";
    }

    // assert
    ASSERT_NE(outline, nullptr);
    // write outline > metadata to be written
    string* outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(outlineAsString->find("Metadata"), std::string::npos);
    delete outlineAsString;

    outline->setTitle("Dirty");
    mind.remind().remember(outline);

    outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(outlineAsString->find("Metadata"), std::string::npos);
    delete outlineAsString;
}
