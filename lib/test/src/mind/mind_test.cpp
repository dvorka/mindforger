/*
 mind_test.cpp     MindForger mind test

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <stddef.h>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../../../src/config/repository.h"
#include "../../../src/model/outline.h"
#include "../../../src/model/note.h"
#include "../../../src/model/tag.h"
#include "../../../src/mind/mind.h"
#include "../../../src/install/installer.h"

#include "../../../src/representations/markdown/markdown_outline_representation.h"

extern char* getMindforgerGitHomePath();

using namespace std;

void dumpOutline(m8r::Outline*& outline)
{
    cout << endl << "  - Outline -----------------------";
    cout << endl << "  " << outline->getKey() << " (key)";
    cout << endl << "  " << outline->getFormat() << " (format)";
    cout << endl << "  "
            << (outline->getPrimaryTag() ?
                    outline->getPrimaryTag()->getName() : "NULL")
            << " (primary tag)";
    if (outline->getTags()->size()) {
        for (size_t t = 0; t < outline->getTags()->size(); t++) {
            cout << endl << "    " << (*outline->getTags())[t]->getName() << " (tag)";
        }
    }
    cout << endl << "  "
            << (outline->getType() ? outline->getType()->getName() : "NULL")
            << " (type)";
    cout << endl << "  '" << outline->getName() << "' (name)";
    cout << endl << "  Description[" << outline->getDescription().size() << "]:";
    for (size_t d = 0; d < outline->getDescription().size(); d++) {
        cout << endl << "    '" << *outline->getDescription()[d] << "' (description)";
    }
    cout << endl << "  " << outline->getCreated() << " (created)";
    cout << endl << "  " << outline->getModified() << " (modified)";
    cout << endl << "  " << outline->getRevision() << " (revision)";
    cout << endl << "  " << outline->getReads() << " (reads)";
    cout << endl << "  " << outline->getRead() << " (read)";
    u_int8_t value = outline->getImportance();
    if (value <= 0) {
        cout << endl << "    X (importance)";
    } else {
        cout << endl << "    " << +value << " (importance)";
    }
    value = outline->getUrgency();
    if (value <= 0) {
        cout << endl << "    X (urgency)";
    } else {
        cout << endl << "    " << +value << " (urgency)";
    }
    value = outline->getProgress();
    if (value <= 0) {
        cout << endl << "    X (progress)";
    } else {
        cout << endl << "    " << +value << " (progress)";
    }
    cout << endl << "  Notes[" << outline->getNotes().size() << "]:";
    if (outline->getNotes().size()) {
        for (m8r::Note* note : outline->getNotes()) {
            ASSERT_NE(nullptr, note);
            cout << endl << "    '" << note->getName() << "' (name)";
            cout << endl << "    '" << note->getDepth() << "' (depth)";
            cout << endl << "    "
                    << (note->getPrimaryTag() ?
                            note->getPrimaryTag()->getName() : "NULL")
                    << " (primary tag)";
            if (note->getTags()->size()) {
                for (size_t t = 0; t < note->getTags()->size(); t++) {
                    cout << endl << "      " << (*note->getTags())[t]->getName()
                            << " (tag)";
                }
            }
            ASSERT_NE(nullptr, note->getType());
            ASSERT_NE("", note->getType()->getName());
            cout << endl << "    "
                    << (note->getType() ? note->getType()->getName() : "NULL")
                    << " (type)";
            cout << endl << "      Description[" << note->getDescription().size()
                    << "]:";
            for (string* description : note->getDescription()) {
                cout << endl << "        '" << *description << "' (description)";
            }
            cout << endl << "  " << note->getCreated() << " (created)";
            cout << endl << "  " << note->getModified() << " (modified)";
            cout << endl << "  " << note->getRevision() << " (revision)";
            cout << endl << "  " << note->getReads() << " (reads)";
            cout << endl << "  " << note->getRead() << " (read)";
        }
    }
}

TEST(MindTestCase, LearnAndRememberMindForgerRepository) {
    // prepare M8R repository
    string repositoryDir{"/tmp/mf-unit-repository-m"};
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    string repositoryTemplate{"/lib/test/resources/basic-repository/memory"};
    repositoryTemplate.insert(0, getMindforgerGitHomePath());
    string to{};
    string from{};
    from.assign(repositoryTemplate);
    from += "/flat-nometa.md";
    to.assign(repositoryDir);
    to += "/memory/flat-nometa.md";
    m8r::copyFile(from,to);
    from.assign(repositoryTemplate);
    from += "/no-metadata.md";
    to.assign(repositoryDir);
    to += "/memory/no-metadata.md";
    m8r::copyFile(from,to);
    from.assign(repositoryTemplate);
    from += "/outline.md";
    to.assign(repositoryDir);
    to += "/memory/outline.md";
    m8r::copyFile(from,to);

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance(); config.clear();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mtc-larmfr.md");
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryDir)),
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

    m8r::Memory& memory = mind.remind();
    vector<m8r::Outline*> outlines = memory.getOutlines();
    cout << endl << "Outlines[" << outlines.size() << "]:";
    if(outlines.size()>0) {
        for(m8r::Outline*& outline:outlines) {
            dumpOutline(outline);
        }
    } else {
        cout << endl << "NO OUTLINES";
    }

    cout << endl << endl << "- Outline > MD ----------------------------------------------";
    m8r::MarkdownOutlineRepresentation mdr{memory.getOntology(), nullptr};
    string outlineMd{} ;
    cout << endl << " " << *mdr.to(outlines[0], &outlineMd);

    cout << endl << endl << "- Outline persistence: modify and save a note ----------------------------------------------";
    m8r::Note* noteToSave = outlines[0]->getNotes()[0];
    noteToSave->setName(string("SAVE"));
    outlines[0]->notifyChange(noteToSave);
    cout << endl << " Saving " << outlines[0]->getKey();
    memory.remember(outlines[0]->getKey());

    cout << endl << "- DONE ----------------------------------------------";
    cout << endl << "";
}

TEST(MindTestCase, LearnAmnesiaLearn) {
    string repositoryPath{"/lib/test/resources/amnesia-repository"};
    repositoryPath.insert(0, getMindforgerGitHomePath());
    m8r::Repository* repository = new m8r::Repository(
        repositoryPath,
        m8r::Repository::RepositoryType::MINDFORGER,
        m8r::Repository::RepositoryMode::REPOSITORY,
        "",
        false);

    // 1/3 learn
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mtc-lal.md");
    config.setActiveRepository(
        config.addRepository(repository), repositoryConfigRepresentation
    );
    cout << endl << "Active repository:" << endl << "  " << config.getActiveRepository()->getDir();
    cout << endl << "Repositories[" << config.getRepositories().size() << "]:";

    m8r::Mind mind(config);
    m8r::Memory& memory = mind.remind();

    EXPECT_EQ(3, memory.getOntology().getOutlineTypes().size());
    EXPECT_EQ(16, memory.getOntology().getNoteTypes().size());
    EXPECT_EQ(14, memory.getOntology().getTags().size());

    mind.learn();
    mind.think().get();

    // assert learned MF repository attributes (mind, outlines count, notes count, ontology count, ...)
    EXPECT_GE(config.getRepositories().size(), 1);
    EXPECT_EQ(1, memory.getStencils(m8r::ResourceType::OUTLINE).size());
    EXPECT_EQ(1, memory.getStencils(m8r::ResourceType::NOTE).size());
    EXPECT_EQ(3, memory.getOutlines().size());
    EXPECT_EQ(3, memory.getOutlinesCount());
    EXPECT_EQ(8, memory.getNotesCount());
    EXPECT_EQ(3, memory.getOntology().getOutlineTypes().size());
    EXPECT_EQ(16, memory.getOntology().getNoteTypes().size());
    EXPECT_EQ(17, memory.getOntology().getTags().size());

    // 2/3 amnesia - assert mind and memory cleaned (+Valgrind memory check)
    mind.amnesia();
    EXPECT_GE(1, config.getRepositories().size());
    EXPECT_EQ(0, memory.getStencils(m8r::ResourceType::OUTLINE).size());
    EXPECT_EQ(0, memory.getStencils(m8r::ResourceType::NOTE).size());
    EXPECT_EQ(0, memory.getOutlines().size());
    EXPECT_EQ(0, memory.getOutlinesCount());
    EXPECT_EQ(0, memory.getNotesCount());
    EXPECT_EQ(3, memory.getOntology().getOutlineTypes().size());
    EXPECT_EQ(16, memory.getOntology().getNoteTypes().size());
    // tags are kept as it's not a problem - they are used as suggestion on new/edit of Os and Ns
    EXPECT_EQ(17, memory.getOntology().getTags().size());

    // 3/3 learn - MARKDOWN repository (not MINDFORGER repository as above)
    repositoryPath.assign("/tmp/mf-unit-amnesia");
    string path, content;
    m8r::removeDirectoryRecursively(repositoryPath.c_str());
#ifdef _WIN32
    int e = _mkdir(repositoryPath.c_str());
#else
    int e = mkdir(repositoryPath.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
#endif // _WIN32
    ASSERT_EQ(e, 0);
    path.assign(repositoryPath+"/1.md");
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
    path.assign(repositoryPath+"/2.md");
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

    // repository is NOT deleted - it's registered in Configuration and it will take care of it
    repository = new m8r::Repository(
        repositoryPath,
        m8r::Repository::RepositoryType::MARKDOWN,
        m8r::Repository::RepositoryMode::REPOSITORY,
        "",
        false);
    config.setActiveRepository(config.addRepository(repository), repositoryConfigRepresentation);
    mind.learn();
    mind.think().get();

    EXPECT_GE(config.getRepositories().size(), 1);
    EXPECT_EQ(2, memory.getOutlines().size());
    EXPECT_EQ(2, memory.getOutlinesCount());
    EXPECT_EQ(4, memory.getNotesCount());
    EXPECT_EQ(0, memory.getStencils(m8r::ResourceType::OUTLINE).size());
    EXPECT_EQ(0, memory.getStencils(m8r::ResourceType::NOTE).size());
    EXPECT_EQ(3, memory.getOntology().getOutlineTypes().size());
    EXPECT_EQ(16, memory.getOntology().getNoteTypes().size());
    // tags are kept as it's not a problem - they are used as suggestion on new/edit of Os and Ns
    EXPECT_EQ(17, memory.getOntology().getTags().size());
}

TEST(MindTestCase, CommonWordsBlacklist) {
    m8r::CommonWordsBlacklist blacklist{};

    ASSERT_TRUE(blacklist.findWord("I"));
    ASSERT_TRUE(blacklist.findWord("you"));
    ASSERT_TRUE(blacklist.findWord("the"));
}
