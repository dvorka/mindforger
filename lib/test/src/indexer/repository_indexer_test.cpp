/*
 repository_indexer_test.cpp     MindForger indexer test

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "../test_utils.h"

using namespace std;

extern char* getMindforgerGitHomePath();
extern void dumpOutline(m8r::Outline*&);
extern void createEmptyRepository(string& repositoryDir, map<string,string>& pathToContent);

TEST(RepositoryIndexerTestCase, RepositoryTypeDetection)
{
    string repositoryPath, directoryName, fileName;

    cout << "MF repository" << endl;
    repositoryPath.assign(FILE_PATH_SEPARATOR + m8r::platformSpecificPath("lib/test/resources/basic-repository"));
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::Repository* r = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    ASSERT_NE(nullptr, r);
    EXPECT_EQ(m8r::Repository::RepositoryType::MINDFORGER, r->getType());
    EXPECT_EQ(m8r::Repository::RepositoryMode::REPOSITORY, r->getMode());
    EXPECT_EQ(repositoryPath, r->getDir());
    EXPECT_TRUE(r->getFile().empty());
    delete r;
    cout << "OK: MF repository" << endl;

    cout << "Markdown repository" << endl;
    repositoryPath.assign(FILE_PATH_SEPARATOR + m8r::platformSpecificPath("lib/test/resources/basic-repository/memory"));
    repositoryPath.insert(0, getMindforgerGitHomePath());
    r = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    ASSERT_NE(nullptr, r);
    EXPECT_EQ(m8r::Repository::RepositoryType::MARKDOWN, r->getType());
    EXPECT_EQ(m8r::Repository::RepositoryMode::REPOSITORY, r->getMode());
    EXPECT_EQ(repositoryPath, r->getDir());
    EXPECT_TRUE(r->getFile().empty());
    delete r;
    cout << "OK: Markdown repository" << endl;

    // MF file
    cout << "MF file" << endl;
    repositoryPath.assign(FILE_PATH_SEPARATOR + m8r::platformSpecificPath("lib/test/resources/basic-repository/memory/flat-metadata.md"));
    repositoryPath.insert(0, getMindforgerGitHomePath());
    directoryName.assign(FILE_PATH_SEPARATOR + m8r::platformSpecificPath("lib/test/resources/basic-repository/memory"));
    directoryName.insert(0, getMindforgerGitHomePath());
    fileName.assign("flat-metadata.md");
    r = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    ASSERT_NE(nullptr, r);
    EXPECT_EQ(m8r::Repository::RepositoryType::MINDFORGER, r->getType());
    EXPECT_EQ(m8r::Repository::RepositoryMode::FILE, r->getMode());
    EXPECT_EQ(directoryName, r->getDir());
    EXPECT_EQ(fileName, r->getFile());
    delete r;
    cout << "OK: MF file" << endl;

    cout << "Markdown file" << endl;
    repositoryPath.assign(FILE_PATH_SEPARATOR + m8r::platformSpecificPath("lib/test/resources/basic-repository/memory/no-metadata.md"));
    repositoryPath.insert(0, getMindforgerGitHomePath());
    directoryName.assign(FILE_PATH_SEPARATOR + m8r::platformSpecificPath("lib/test/resources/basic-repository/memory"));
    directoryName.insert(0, getMindforgerGitHomePath());
    fileName.assign("no-metadata.md");
    r = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    ASSERT_NE(nullptr, r);
    // it's OK - MF is default type BEFORE parsing (MF doesn't delete metadata, MD would erase them)
    EXPECT_EQ(m8r::Repository::RepositoryType::MINDFORGER, r->getType());
    EXPECT_EQ(m8r::Repository::RepositoryMode::FILE, r->getMode());
    EXPECT_EQ(directoryName, r->getDir());
    EXPECT_EQ(fileName, r->getFile());
    delete r;
    cout << "OK: Markdown file" << endl;
}

TEST(RepositoryIndexerTestCase, MindForgerRepository)
{
    string repositoryPath{m8r::platformSpecificPath("/tmp/mf-unit-repository-indexer-mf")};
    map<string,string> pathToContent;
    string path, content;

    path.assign(repositoryPath + FILE_PATH_SEPARATOR + m8r::platformSpecificPath("memory/first.md"));
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

    path.assign(repositoryPath+ FILE_PATH_SEPARATOR + m8r::platformSpecificPath("memory/second.md"));
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
    EXPECT_EQ(repositoryPath, repository->getDir());
    EXPECT_EQ(m8r::Repository::RepositoryType::MINDFORGER, repository->getType());
    EXPECT_EQ(m8r::Repository::RepositoryMode::REPOSITORY, repository->getMode());
    EXPECT_EQ(false, repository->isReadOnly());
    EXPECT_EQ(2, outlineFiles.size());
    EXPECT_EQ(2, repositoryIndexer.getMarkdownFiles().size());
    EXPECT_EQ(0, repositoryIndexer.getNoteStencilsFileNames().size());
    EXPECT_EQ(0, repositoryIndexer.getOutlineStencilsFileNames().size());

    // test that metadata ARE written
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath(m8r::platformSpecificPath("/tmp/cfg-ritc-mfr.md"));
    config.setActiveRepository(config.addRepository(repository), repositoryConfigRepresentation);
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();
    m8r::Memory& memory = mind.remind();
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* outline;
    cout << endl << "Outlines[" << outlines.size() << "]:";
    if(outlines.size()>0) {
        for(m8r::Outline*& o:outlines) {
            dumpOutline(o);
            ASSERT_EQ(m8r::MarkdownDocument::Format::MINDFORGER, o->getFormat());
            if(o->getName() == string("First Outline")) {
                outline = o;
            }
        }
    } else {
        cout << endl << "NO OUTLINES";
    }

    // assert
    ASSERT_NE(nullptr, outline);
    // write outline > metadata to be written
    string* outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(std::string::npos, outlineAsString->find("Metadata"));
    delete outlineAsString;

    mind.remind().remember(outline);

    outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_NE(std::string::npos, outlineAsString->find("Metadata"));
    delete outlineAsString;
}

TEST(RepositoryIndexerTestCase, MarkdownRepository)
{
    string repositoryPath{m8r::platformSpecificPath("/tmp/mf-unit-repository-indexer-md")};
    string path, content;

    m8r::removeDirectoryRecursively(repositoryPath.c_str());
#ifdef _WIN32
    int e = _mkdir(repositoryPath.c_str());
#else
    int e = mkdir(repositoryPath.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
#endif // _WIN32
    ASSERT_EQ(0, e);

    path.assign(repositoryPath+ FILE_PATH_SEPARATOR + "first.md");
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
    EXPECT_EQ(repositoryPath, repository->getDir());
    EXPECT_EQ(m8r::Repository::RepositoryType::MARKDOWN, repository->getType());
    EXPECT_EQ(m8r::Repository::RepositoryMode::REPOSITORY, repository->getMode());
    EXPECT_EQ(false, repository->isReadOnly());
    EXPECT_EQ(2, outlineFiles.size());
    EXPECT_EQ(2, repositoryIndexer.getMarkdownFiles().size());
    EXPECT_EQ(0, repositoryIndexer.getNoteStencilsFileNames().size());
    EXPECT_EQ(0, repositoryIndexer.getOutlineStencilsFileNames().size());

    // test that metadata ARE written
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath(m8r::platformSpecificPath("/tmp/cfg-ritc-mr.md"));
    config.setActiveRepository(config.addRepository(repository), repositoryConfigRepresentation);
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();

    m8r::Memory& memory = mind.remind();
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* outline;
    cout << endl << "Outlines[" << outlines.size() << "]:";
    if(outlines.size()>0) {
        for(m8r::Outline*& o:outlines) {
            dumpOutline(o);
            ASSERT_EQ(m8r::MarkdownDocument::Format::MARKDOWN, o->getFormat());
            if(o->getName() == string("First Markdown")) {
                outline = o;
            }
        }
    } else {
        cout << endl << "NO OUTLINES";
    }

    // assert
    ASSERT_NE(nullptr, outline);
    // write outline > metadata to be written
    string* outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(std::string::npos, outlineAsString->find("Metadata"));
    delete outlineAsString;

    outline->setName("Dirty");
    mind.remind().remember(outline);

    outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(std::string::npos, outlineAsString->find("Metadata"));
    delete outlineAsString;
}

TEST(RepositoryIndexerTestCase, MindForgerFile)
{
    string repositoryPath{m8r::platformSpecificPath("/tmp")};
    string fileName{"repository-indexer-single-mf-file.md"};
    string content;
    string filePath{repositoryPath+FILE_PATH_SEPARATOR+fileName};

    content.assign(
        "# First Outline <!-- Metadata: type: Outline; created: 2017-02-02 06:31:09; reads: 35; read: 2018-01-07 08:05:58; revision: 36; modified: 2018-01-07 08:06:05; importance: 0/5; urgency: 0/5; progress: 0%; -->"
        "\n"
        "\nFirst outline text."
        "\n"
        "\n## Note 1 <!-- Metadata: type: Note; created: 2017-02-02 06:31:09; reads: 35; read: 2018-01-07 08:05:58; revision: 36; modified: 2018-01-07 08:06:05; progress: 20%; -->"
        "\n" // empty line s/d to reproduce bug
        "\nNote 1 text."
        "\n"
        "\n## Note 2 <!-- Metadata: type: Idea; created: 2017-02-02 06:31:09; reads: 1; read: 2018-01-07 08:05:58; revision: 1; modified: 2018-01-07 08:06:05; progress: 10%; -->"
        "\nNote 2 text."
        "\n");
    m8r::stringToFile(filePath, content);

    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    // repository type will be determined by mind.think() > memory.learn()
    repository->setMode(m8r::Repository::RepositoryMode::FILE);
    repository->setFile(fileName);

    m8r::RepositoryIndexer repositoryIndexer{};
    repositoryIndexer.index(repository);
    repositoryIndexer.getRepository()->print();

    EXPECT_EQ(1, repositoryIndexer.getAllOutlineFileNames().size());
    EXPECT_EQ(1, repositoryIndexer.getMarkdownFiles().size());
    EXPECT_EQ(0, repositoryIndexer.getNoteStencilsFileNames().size());
    EXPECT_EQ(0, repositoryIndexer.getOutlineStencilsFileNames().size());

    // test that metadata ARE written
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath(m8r::platformSpecificPath("/tmp/cfg-ritc-mff.md"));
    config.setActiveRepository(config.addRepository(repository), repositoryConfigRepresentation);
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();

    EXPECT_EQ(repositoryPath, repository->getDir());
    EXPECT_EQ(m8r::Repository::RepositoryType::MINDFORGER, repository->getType());
    EXPECT_EQ(m8r::Repository::RepositoryMode::FILE, repository->getMode());
    EXPECT_EQ(false, repository->isReadOnly());

    m8r::Memory& memory = mind.remind();
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* outline;
    cout << endl << "Outlines[" << outlines.size() << "]:";
    if(outlines.size()>0) {
        for(m8r::Outline*& o:outlines) {
            dumpOutline(o);
            ASSERT_EQ(m8r::MarkdownDocument::Format::MINDFORGER, o->getFormat());
            if(o->getName() == string("First Outline")) {
                outline = o;
            }
        }
    } else {
        cout << endl << "NO OUTLINES";
    }

    // assert
    ASSERT_NE(nullptr, outline);
    cout << endl << "Loading saved O from " << outline->getKey() << flush;
    // write outline > metadata to be written
    string* outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_NE(std::string::npos, outlineAsString->find("Metadata"));
    delete outlineAsString;

    outline->setName("Dirty");
    mind.remind().remember(outline);

    outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_NE(std::string::npos, outlineAsString->find("Metadata"));
    delete outlineAsString;
}

TEST(RepositoryIndexerTestCase, MarkdownFile)
{
    string repositoryPath{ m8r::platformSpecificPath("/tmp")};
    string fileName{"repository-indexer-single-md-file.md"};
    string content;
    string filePath{repositoryPath+FILE_PATH_SEPARATOR+fileName};

    content.assign(
        "# First Markdown"
        "\n"
        "\nFirst MD text."
        "\n"
        "\n## Note 1"
        "\n" // empty line s/d to reproduce bug
        "\nNote 1 text."
        "\n"
        "\n## Note 2"
        "\nNote 2 text."
        "\n");
    m8r::stringToFile(filePath, content);

    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    // repository type will be determined by mind.think() > memory.learn()
    repository->setMode(m8r::Repository::RepositoryMode::FILE);
    repository->setFile(fileName);

    m8r::RepositoryIndexer repositoryIndexer{};
    repositoryIndexer.index(repository);
    repositoryIndexer.getRepository()->print();

    EXPECT_EQ(1, repositoryIndexer.getAllOutlineFileNames().size());
    EXPECT_EQ(1, repositoryIndexer.getMarkdownFiles().size());
    EXPECT_EQ(0, repositoryIndexer.getNoteStencilsFileNames().size());
    EXPECT_EQ(0, repositoryIndexer.getOutlineStencilsFileNames().size());

    // test that metadata ARE written
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath(m8r::platformSpecificPath("/tmp/cfg-ritc-mf.md"));
    config.setActiveRepository(config.addRepository(repository), repositoryConfigRepresentation);
    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();

    EXPECT_EQ(repositoryPath, repository->getDir());
    EXPECT_EQ(m8r::Repository::RepositoryType::MARKDOWN, repository->getType());
    EXPECT_EQ(m8r::Repository::RepositoryMode::FILE, repository->getMode());
    EXPECT_EQ(false, repository->isReadOnly());

    m8r::Memory& memory = mind.remind();
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* outline;
    cout << endl << "Outlines[" << outlines.size() << "]:";
    if(outlines.size()>0) {
        for(m8r::Outline*& o:outlines) {
            dumpOutline(o);
            ASSERT_EQ(m8r::MarkdownDocument::Format::MARKDOWN, o->getFormat());
            if(o->getName() == string("First Markdown")) {
                outline = o;
            }
        }
    } else {
        cout << endl << "NO MDs";
    }

    // asserts
    ASSERT_NE(nullptr, outline);
    // assert O contains NO metadata
    string* outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(std::string::npos, outlineAsString->find("Metadata"));
    EXPECT_EQ(*outlineAsString, content);
    delete outlineAsString;

    // assert that load -> save -> load w/o metadata yields identical result
    mind.remind().remember(outline);
    outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(*outlineAsString, content);
    delete outlineAsString;

    // assert write to O writes NO metadata
    outline->setName("Dirty");
    mind.remind().remember(outline);
    outlineAsString = m8r::fileToString(outline->getKey());
    EXPECT_EQ(std::string::npos, outlineAsString->find("Metadata"));
    delete outlineAsString;
}

TEST(RepositoryIndexerTestCase, MakePathRelative)
{
    string repositoryPath{ m8r::platformSpecificPath("/tmp/mf-relativize")};
    map<string,string> pathToContent;
    string path, content;

    path.assign(repositoryPath + FILE_PATH_SEPARATOR + m8r::platformSpecificPath("memory/first.md"));
    content.assign(
        "# First Outline"
        "\nFirst outline text."
        "\n");
    pathToContent[path] = content;

    path.assign(repositoryPath + FILE_PATH_SEPARATOR + m8r::platformSpecificPath("memory/second.md"));
    content.assign(
        "# Second Outline"
        "\nSecond outline text."
        "\n");
    pathToContent[path] = content;

    m8r::createEmptyRepository(repositoryPath, pathToContent);
    m8r::RepositoryIndexer repositoryIndexer{};

    /*
     * MF repository
     */

    MF_DEBUG("RELATIVIZE MF repository: " << repositoryPath << endl);
    m8r::Repository* repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repositoryIndexer.index(repository);
    repositoryIndexer.getRepository()->print();

    // asserts
    EXPECT_EQ(repositoryPath, repository->getDir());
    EXPECT_EQ(2, repositoryIndexer.getAllOutlineFileNames().size());
    EXPECT_EQ(m8r::Repository::RepositoryType::MINDFORGER, repository->getType());
    EXPECT_EQ(m8r::Repository::RepositoryMode::REPOSITORY, repository->getMode());

    // MF/MD: same dir
    EXPECT_EQ("dst.md", repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/dst.md")}));
    EXPECT_EQ("dst.md#n1", repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/dst.md#n1")}));

    // MF/MD: src parent of dst
    EXPECT_EQ(m8r::platformSpecificPath("a/b/c/dst.md"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/dst.md")}));
    EXPECT_EQ(m8r::platformSpecificPath("a/b/c/dst.md#n1"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/dst.md#n1")}));

    // MF/MD: dst parent of src
    EXPECT_EQ(m8r::platformSpecificPath("../../../dst.md"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/dst.md")}));
    EXPECT_EQ(m8r::platformSpecificPath("../../../dst.md#n1"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/dst.md#n1")}));

    // MF/MD: siblings
    EXPECT_EQ(m8r::platformSpecificPath("../../A/B/dst.md"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/A/B/dst.md")}));
    EXPECT_EQ(m8r::platformSpecificPath("../../A/B/dst.md#n1"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/A/B/dst.md#n1")}));

    // MF/MD: same file
    EXPECT_EQ("#n1", repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md")},
		string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md#n1") }));
    EXPECT_EQ("src.md", repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md#")}));

    // out of repo
    EXPECT_EQ(m8r::platformSpecificPath("/tmp/dst.md"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md")},
       string{ m8r::platformSpecificPath("/tmp/dst.md")}));

    // robustness
    EXPECT_EQ(m8r::platformSpecificPath("/"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md")},
       string{ m8r::platformSpecificPath("/")}));
    EXPECT_EQ("", repositoryIndexer.makePathRelative(
       repository,
       string{""},
       string{""}));

    /*
     * MD repository
     */

    repositoryPath += string(FILE_PATH_SEPARATOR) + "memory";
    MF_DEBUG("RELATIVIZE MD repository: " << repositoryPath << endl);
    delete repository;
    repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repositoryIndexer.index(repository);
    repositoryIndexer.getRepository()->print();

    // asserts
    EXPECT_EQ(2, repositoryIndexer.getAllOutlineFileNames().size());
    EXPECT_EQ(m8r::Repository::RepositoryType::MARKDOWN, repository->getType());
    EXPECT_EQ(m8r::Repository::RepositoryMode::REPOSITORY, repository->getMode());

    // MF/MD: same dir
    EXPECT_EQ("dst.md", repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/dst.md")}));
    EXPECT_EQ("dst.md#n1", repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/dst.md#n1")}));

    // MF/MD: src parent of dst
    EXPECT_EQ(m8r::platformSpecificPath("a/b/c/dst.md"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/dst.md")}));
    EXPECT_EQ(m8r::platformSpecificPath("a/b/c/dst.md#n1"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/dst.md#n1")}));

    // MF/MD: dst parent of src
    EXPECT_EQ(m8r::platformSpecificPath("../../../dst.md"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/dst.md")}));
    EXPECT_EQ(m8r::platformSpecificPath("../../../dst.md#n1"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/dst.md#n1")}));

    // MF/MD: siblings
    EXPECT_EQ(m8r::platformSpecificPath("../../A/B/dst.md"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/A/B/dst.md")}));
    EXPECT_EQ(m8r::platformSpecificPath("../../A/B/dst.md#n1"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/A/B/dst.md#n1")}));

    // out of repo
    EXPECT_EQ(m8r::platformSpecificPath("/tmp/dst.md"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md")},
       string{ m8r::platformSpecificPath("/tmp/dst.md")}));

    // robustness
    EXPECT_EQ(m8r::platformSpecificPath("/"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/src.md")},
       string{ m8r::platformSpecificPath("/")}));
    EXPECT_EQ("", repositoryIndexer.makePathRelative(
       repository,
       string{""},
       string{""}));

    /*
     * MD file & MF file - relativization doesn't care about content
     */
    repositoryPath += string(FILE_PATH_SEPARATOR) + "first.md";
    MF_DEBUG("RELATIVIZE MF/MD file: " << repositoryPath << endl);
    delete repository;
    repository = m8r::RepositoryIndexer::getRepositoryForPath(repositoryPath);
    repositoryIndexer.index(repository);
    repositoryIndexer.getRepository()->print();

    // asserts
    EXPECT_EQ(1, repositoryIndexer.getAllOutlineFileNames().size());
    EXPECT_EQ(m8r::Repository::RepositoryType::MINDFORGER, repository->getType());
    EXPECT_EQ(m8r::Repository::RepositoryMode::FILE, repository->getMode());

    // MF/MD: same file
    EXPECT_EQ("#n1", repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/first.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/first.md#n1")}));
    EXPECT_EQ("first.md", repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/first.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/first.md#")}));
    EXPECT_EQ("#n1", repositoryIndexer.makePathRelative(
       repository,
       string{"first.md"},
       string{"first.md#n1"}));

    // MF/MD: same dir
    EXPECT_EQ("dst.md", repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/first.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/dst.md")}));
    EXPECT_EQ("dst.md#n1", repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/first.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/dst.md#n1")}));

    // everything else is absolute

    // MF/MD: src parent of dst
    EXPECT_EQ(m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/dst.md"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/first.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/dst.md")}));
    EXPECT_EQ(m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/dst.md#n1"), repositoryIndexer.makePathRelative(
       repository,
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/first.md")},
       string{ m8r::platformSpecificPath("/tmp/mf-relativize/memory/a/b/c/dst.md#n1")}));

    delete repository;
}
