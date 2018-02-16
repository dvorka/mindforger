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
    cout << endl << "  "
            << (outline->getPrimaryTag() ?
                    outline->getPrimaryTag()->getName() : "NULL")
            << " (primary tag)";
    if (outline->getTags().size()) {
        for (size_t t = 0; t < outline->getTags().size(); t++) {
            cout << endl << "    " << outline->getTags()[t]->getName() << " (tag)";
        }
    }
    cout << endl << "  "
            << (outline->getType() ? outline->getType()->getName() : "NULL")
            << " (type)";
    cout << endl << "  '" << outline->getTitle() << "' (title)";
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
            cout << endl << "    '" << note->getTitle() << "' (title)";
            cout << endl << "    '" << note->getDepth() << "' (depth)";
            cout << endl << "    "
                    << (note->getPrimaryTag() ?
                            note->getPrimaryTag()->getName() : "NULL")
                    << " (primary tag)";
            if (note->getTags().size()) {
                for (size_t t = 0; t < note->getTags().size(); t++) {
                    cout << endl << "      " << note->getTags()[t]->getName()
                            << " (tag)";
                }
            }
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

TEST(MindTestCase, Ontology) {
    cout << endl << "Outline label: " << m8r::Tag::KeyCool();

    // TODO TBD
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

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryDir)));
    m8r::Mind mind(config);
    mind.think();

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
    noteToSave->setTitle(string("SAVE"));
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
        false);

    // 1/3 learn
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.setActiveRepository(config.addRepository(repository));
    m8r::Mind mind(config);
    mind.think();
    // TODO remove - m8r::Memory& memory = mind.remind();

    cout << endl << "Active repository:" << endl << "  " << config.getActiveRepository()->getPath();
    cout << endl << "Repositories[" << config.getRepositories().size() << "]:";
    for(auto& r:config.getRepositories()) {
        cout << endl << "  " << r.first;
    }
    cout << endl;

    // assert learned MF repository attributes (mind, outlines count, notes count, ontology count, ...)
    EXPECT_EQ(config.getRepositories().size(), 1);
    // ...

    // 2/3 amnesia
    mind.amnesia();

    // assert mind and memory cleaned (+Valgrind memory check)
    EXPECT_EQ(config.getRepositories().size(), 0);
    // ...

    // 3/3 learn
    // ...
    // assert ANOTHER learned MF repository attributes (mind, outlines count, notes count, different ontology count, ...)
}

// TODO start w/ empty repository (do I need it?)
// TODO learn MD repository
// TODO learn MF repository (might be test above)
// TODO learn MD file
// TODO learn MF file
// TODO test readonly
// TODO test metadata not written
