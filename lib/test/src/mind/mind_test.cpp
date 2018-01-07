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

#include "../../../src/model/outline.h"
#include "../../../src/model/note.h"
#include "../../../src/model/tag.h"
#include "../../../src/mind/mind.h"
#include "../../../src/install/installer.h"

#include "../../../src/representations/markdown/markdown_outline_representation.h"

extern char* getMindforgerGitHomePath(void);

using namespace std;

void dumpOutline(m8r::Outline*& outline)
{
    cout << "\n  - Outline -----------------------";
    cout << "\n  " << outline->getKey() << " (key)";
    cout << "\n  "
            << (outline->getPrimaryTag() ?
                    outline->getPrimaryTag()->getName() : "NULL")
            << " (primary tag)";
    if (outline->getTags().size()) {
        for (size_t t = 0; t < outline->getTags().size(); t++) {
            cout << "\n    " << outline->getTags()[t]->getName() << " (tag)";
        }
    }
    cout << "\n  "
            << (outline->getType() ? outline->getType()->getName() : "NULL")
            << " (type)";
    cout << "\n  '" << outline->getTitle() << "' (title)";
    cout << "\n  Description[" << outline->getDescription().size() << "]:";
    for (size_t d = 0; d < outline->getDescription().size(); d++) {
        cout << "\n    '" << *outline->getDescription()[d] << "' (description)";
    }
    cout << "\n  " << outline->getCreated() << " (created)";
    cout << "\n  " << outline->getModified() << " (modified)";
    cout << "\n  " << outline->getRevision() << " (revision)";
    cout << "\n  " << outline->getReads() << " (reads)";
    cout << "\n  " << outline->getRead() << " (read)";
    u_int8_t value = outline->getImportance();
    if (value <= 0) {
        cout << "\n    X (importance)";
    } else {
        cout << "\n    " << +value << " (importance)";
    }
    value = outline->getUrgency();
    if (value <= 0) {
        cout << "\n    X (urgency)";
    } else {
        cout << "\n    " << +value << " (urgency)";
    }
    value = outline->getProgress();
    if (value <= 0) {
        cout << "\n    X (progress)";
    } else {
        cout << "\n    " << +value << " (progress)";
    }
    cout << "\n  Notes[" << outline->getNotes().size() << "]:";
    if (outline->getNotes().size()) {
        for (m8r::Note* note : outline->getNotes()) {
            cout << "\n    '" << note->getTitle() << "' (title)";
            cout << "\n    '" << note->getDepth() << "' (depth)";
            cout << "\n    "
                    << (note->getPrimaryTag() ?
                            note->getPrimaryTag()->getName() : "NULL")
                    << " (primary tag)";
            if (note->getTags().size()) {
                for (size_t t = 0; t < note->getTags().size(); t++) {
                    cout << "\n      " << note->getTags()[t]->getName()
                            << " (tag)";
                }
            }
            cout << "\n    "
                    << (note->getType() ? note->getType()->getName() : "NULL")
                    << " (type)";
            cout << "\n      Description[" << note->getDescription().size()
                    << "]:";
            for (string* description : note->getDescription()) {
                cout << "\n        '" << *description << "' (description)";
            }
            cout << "\n  " << note->getCreated() << " (created)";
            cout << "\n  " << note->getModified() << " (modified)";
            cout << "\n  " << note->getRevision() << " (revision)";
            cout << "\n  " << note->getReads() << " (reads)";
            cout << "\n  " << note->getRead() << " (read)";
        }
    }
}

TEST(MindTestCase, Ontology) {
    cout << "\nOutline label: " << m8r::Tag::KeyCool();
}

TEST(MindTestCase, Mind) {
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

    m8r::Configuration configuration{repositoryDir};
    m8r::Mind mind(configuration);
    mind.think();

    m8r::Memory& memory = mind.remind();
    vector<m8r::Outline*> outlines = memory.getOutlines();
    cout << "\nOutlines[" << outlines.size() << "]:";
    if(outlines.size()>0) {
        for(m8r::Outline*& outline:outlines) {
            dumpOutline(outline);
        }
    } else {
        cout << "\nNO OUTLINES";
    }

    cout << "\n\n- Outline > MD ----------------------------------------------";
    m8r::MarkdownOutlineRepresentation mdr{memory.getOntology()};
    string outlineMd{} ;
    cout << "\n " << *mdr.to(outlines[0], &outlineMd);

    cout << "\n\n- Outline persistence: modify and save a note ----------------------------------------------";
    m8r::Note* noteToSave = outlines[0]->getNotes()[0];
    noteToSave->setTitle(string("SAVE"));
    outlines[0]->notifyChange(noteToSave);
    cout << "\n Saving " << outlines[0]->getKey();
    memory.remember(outlines[0]->getKey());

    cout << "\n- DONE ----------------------------------------------";
    cout << "\n";
}

