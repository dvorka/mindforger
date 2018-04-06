/*
 mind_test.cpp     MindForger mind test

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
            ASSERT_TRUE(note != nullptr);
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
            ASSERT_TRUE(note->getType() != nullptr);
            ASSERT_NE(note->getType()->getName(), "");
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
    from.append("/flat-nometa.md");
    to.assign(repositoryDir);
    to.append("/memory/flat-nometa.md");
    m8r::copyFile(from,to);
    from.assign(repositoryTemplate);
    from.append("/no-metadata.md");
    to.assign(repositoryDir);
    to.append("/memory/no-metadata.md");
    m8r::copyFile(from,to);
    from.assign(repositoryTemplate);
    from.append("/outline.md");
    to.assign(repositoryDir);
    to.append("/memory/outline.md");
    m8r::copyFile(from,to);

    m8r::Configuration& config = m8r::Configuration::getInstance(); config.clear();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mtc-larmfr.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryDir)));
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
    m8r::MarkdownOutlineRepresentation mdr{memory.getOntology()};
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
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-mtc-lal.md");
    config.setActiveRepository(config.addRepository(repository));
    cout << endl << "Active repository:" << endl << "  " << config.getActiveRepository()->getDir();
    cout << endl << "Repositories[" << config.getRepositories().size() << "]:";

    m8r::Mind mind(config);
    m8r::Memory& memory = mind.remind();

    EXPECT_EQ(memory.getOntology().getOutlineTypes().size(), 2);
    EXPECT_EQ(memory.getOntology().getNoteTypes().size(), 16);
    EXPECT_EQ(memory.getOntology().getTags().size(), 12);

    mind.learn();
    mind.think().get();

    // assert learned MF repository attributes (mind, outlines count, notes count, ontology count, ...)
    EXPECT_GE(config.getRepositories().size(), 1);
    EXPECT_EQ(memory.getStencils(m8r::ResourceType::OUTLINE).size(), 1);
    EXPECT_EQ(memory.getStencils(m8r::ResourceType::NOTE).size(), 1);
    EXPECT_EQ(memory.getOutlines().size(), 3);
    EXPECT_EQ(memory.getOutlinesCount(), 3);
    EXPECT_EQ(memory.getNotesCount(), 8);
    EXPECT_EQ(memory.getOntology().getOutlineTypes().size(), 2);
    EXPECT_EQ(memory.getOntology().getNoteTypes().size(), 16);
    EXPECT_EQ(memory.getOntology().getTags().size(), 15);

    // 2/3 amnesia - assert mind and memory cleaned (+Valgrind memory check)
    mind.amnesia();
    EXPECT_GE(config.getRepositories().size(), 1);
    EXPECT_EQ(memory.getStencils(m8r::ResourceType::OUTLINE).size(), 0);
    EXPECT_EQ(memory.getStencils(m8r::ResourceType::NOTE).size(), 0);
    EXPECT_EQ(memory.getOutlines().size(), 0);
    EXPECT_EQ(memory.getOutlinesCount(), 0);
    EXPECT_EQ(memory.getNotesCount(), 0);
    EXPECT_EQ(memory.getOntology().getOutlineTypes().size(), 2);
    EXPECT_EQ(memory.getOntology().getNoteTypes().size(), 16);
    EXPECT_EQ(memory.getOntology().getTags().size(), 15); // tags are kept as it's not a problem - they are used as suggestion on new/edit of Os and Ns

    // 3/3 learn - MARKDOWN repository (not MINDFORGER repository as above)
    repositoryPath.assign("/tmp/mf-unit-amnesia");
    string path, content;
    m8r::removeDirectoryRecursively(repositoryPath.c_str());
    int e = mkdir(repositoryPath.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
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
    config.setActiveRepository(config.addRepository(repository));
    mind.learn();
    mind.think().get();

    EXPECT_GE(config.getRepositories().size(), 1);
    EXPECT_EQ(memory.getOutlines().size(), 2);
    EXPECT_EQ(memory.getOutlinesCount(), 2);
    EXPECT_EQ(memory.getNotesCount(), 4);
    EXPECT_EQ(memory.getStencils(m8r::ResourceType::OUTLINE).size(), 0);
    EXPECT_EQ(memory.getStencils(m8r::ResourceType::NOTE).size(), 0);
    EXPECT_EQ(memory.getOntology().getOutlineTypes().size(), 2);
    EXPECT_EQ(memory.getOntology().getNoteTypes().size(), 16);
    EXPECT_EQ(memory.getOntology().getTags().size(), 15); // tags are kept as it's not a problem - they are used as suggestion on new/edit of Os and Ns
}

// TODO start w/ empty repository (do I need it?)
// TODO learn MD repository
// TODO learn MF repository (might be test above)
// TODO learn MD file
// TODO learn MF file
// TODO test readonly
