/*
 note_test.cpp     MindForger test

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

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../../../src/gear/file_utils.h"
#include "../../../src/model/outline.h"
#include "../../../src/model/outline_type.h"
#include "../../../src/model/note.h"
#include "../../../src/model/tag.h"
#include "../../../src/model/stencil.h"
#include "../../../src/model/resource_types.h"
#include "../../../src/mind/mind.h"
#include "../../../src/install/installer.h"

#include "../test_gear.h"

using namespace std;

TEST(NoteTestCase, AddNewStencilNoteToOutline) {
    // prepare M8R repository and let the mind think...
    string repositoryDir{"/tmp/mf-unit-repository-o"};
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    string stencilFile{repositoryDir+"/stencils/notebooks/o-s.md"};
    string stencilContent{
        "# Stencil Test Outline"
        "\n"
        "\nOutline text."
        "\n"
        "\n## Stencil Note 1"
        "\nNote 1 text."
        "\n"
        "\n## Stencil Note 2"
        "\nNote 2 text."
        "\n"};
    m8r::stringToFile(stencilFile,stencilContent);
    stencilFile.assign(repositoryDir+"/stencils/notes/n-s.md");
    stencilContent.assign("## Stencil Single Note\nNote text.\n");
    m8r::stringToFile(stencilFile,stencilContent);
    string oFile{repositoryDir+"/memory/outline.md"};
    string oContent{"# Test Outline\n\nOutline text.\n\n## Note 1\nNote 1 text.\n\n##Note 2\nNote 2 text.\n"};
    m8r::stringToFile(oFile,oContent);

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-ntc-ansnto.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryDir)));
    m8r::Mind mind{config};
    m8r::Memory& memory = mind.remind();
    mind.learn();
    mind.think().get();

    // get the outline and add to it a stencil note
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* o = outlines.at(0);
    vector<const m8r::Tag*> tags{};
    tags.push_back(mind.getOntology().findOrCreateTag(m8r::Tag::KeyImportant()));
    m8r::Stencil* stencil = memory.getStencils(m8r::ResourceType::NOTE).at(0);
    string name{"!!! STENCIL Note !!!"};
    mind.noteNew(
                o->getKey(),
                0,
                &name,
                mind.getOntology().findOrCreateNoteType(m8r::NoteType::KeyAction()),
                0,
                &tags,
                0,
                stencil);
    // insert note in the middle and to the depth 1
    string deepName{"!!! DEEP Note !!!"};
    // insert note in the middle and to the depth 1
    m8r::Note* newNote = mind.noteNew(
                o->getKey(),
                2,
                &deepName,
                mind.getOntology().findOrCreateNoteType(m8r::NoteType::KeyAction()),
                0,
                &tags,
                0,
                stencil);
    newNote->demote();
    //cout << "Begin:" << endl; for(m8r::Note* nn: o->getNotes()) { cout << "  #x" << nn->getDepth() << " " << nn->getName() << endl; }

    // asserts
    EXPECT_EQ(oFile, o->getKey());
    EXPECT_EQ(string{"Test Outline"}, o->getName());
    EXPECT_EQ(4, o->getNotesCount());
    EXPECT_EQ(name, o->getNotes()[0]->getName());
    EXPECT_EQ(string{"Note 1"}, o->getNotes()[1]->getName());
    EXPECT_EQ(deepName, o->getNotes()[2]->getName());
    EXPECT_EQ(string{"Note 2"}, o->getNotes()[3]->getName());

    // delete NOTE
    mind.noteForget(o->getNotes()[3]);
    //cout << "After delete:" << endl; for(m8r::Note* nn: o->getNotes()) { cout << "  #x" << nn->getDepth() << " " << nn->getName() << endl; }
    EXPECT_EQ(3, o->getNotesCount());
    EXPECT_EQ(name, o->getNotes()[0]->getName());
    EXPECT_EQ(string{"Note 1"}, o->getNotes()[1]->getName());
    EXPECT_EQ(deepName, o->getNotes()[2]->getName());

    // delete NOTE w/ DEEP child
    //cout << "Before DEEP delete:" << endl; for(m8r::Note* nn: o->getNotes()) { cout << "  #x" << nn->getDepth() << " " << nn->getName() << endl; }
    mind.noteForget(o->getNotes()[1]);
    //cout << "After DEEP delete:" << endl; for(m8r::Note* nn: o->getNotes()) { cout << "  #x" << nn->getDepth() << " " << nn->getName() << endl; }
    EXPECT_EQ(2, o->getNotesCount());
    EXPECT_EQ(name, o->getNotes()[0]->getName());

    // IMPROVE test add note on offset (virified manually that it works)
    mind.noteForget(o->getNotes()[0]);
    EXPECT_EQ(0, o->getNotesCount());

    mind.remind().remember(o);

    // delete OUTLINE
    EXPECT_EQ(1, mind.remind().getOutlinesCount());
    mind.outlineForget(o->getKey());
    EXPECT_EQ(0, mind.remind().getOutlinesCount());
}

TEST(NoteTestCase, PromoteDemoteUpDown) {
    // prepare M8R repository and let the mind think...
    string repositoryDir{"/tmp/mf-unit-repository-o"};
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    string oFile{repositoryDir+"/memory/o.md"};
    string oContent{
        "# Note Operations Test Outline"
        "\nOutline text."
        "\n"
        "\n# 1"
        "\nT1."
        "\n"
        "\n# 2"
        "\nT2."
        "\n"
        "\n# 3"
        "\nT3."
        "\n"};
    m8r::stringToFile(oFile,oContent);

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-ntc-pdud.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryDir)));
    m8r::Mind mind{config};
    m8r::Memory& memory = mind.remind();
    mind.learn();
    mind.think().get();

    // test
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* o = outlines.at(0);

    m8r::Outline::Patch patch{m8r::Outline::Patch::Diff::NO,0,0};

    EXPECT_EQ(3, o->getNotesCount());
    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ(0, o->getNotes()[0]->getDepth());
    EXPECT_EQ(0, o->getNotes()[1]->getDepth());
    EXPECT_EQ(0, o->getNotes()[2]->getDepth());

    // demote

    o->demoteNote(o->getNotes()[2], &patch);

    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ(0, o->getNotes()[0]->getDepth());
    EXPECT_EQ(0, o->getNotes()[1]->getDepth());
    EXPECT_EQ(1, o->getNotes()[2]->getDepth());
    EXPECT_EQ(m8r::Outline::Patch::Diff::CHANGE, patch.diff);
    EXPECT_EQ(2, patch.start);
    EXPECT_EQ(0, patch.count);

    o->demoteNote(o->getNotes()[1]);

    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ(0, o->getNotes()[0]->getDepth());
    EXPECT_EQ(1, o->getNotes()[1]->getDepth());
    EXPECT_EQ(2, o->getNotes()[2]->getDepth());
    EXPECT_EQ(m8r::Outline::Patch::Diff::CHANGE, patch.diff);
    EXPECT_EQ(2, patch.start);
    EXPECT_EQ(0, patch.count);

    // promote

    o->promoteNote(o->getNotes()[1], &patch);
    o->promoteNote(o->getNotes()[2], &patch);

    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ(0, o->getNotes()[0]->getDepth());
    EXPECT_EQ(0, o->getNotes()[1]->getDepth());
    EXPECT_EQ(0, o->getNotes()[2]->getDepth());
    EXPECT_EQ(m8r::Outline::Patch::Diff::CHANGE, patch.diff);
    EXPECT_EQ(2, patch.start);
    EXPECT_EQ(0, patch.count);

    // up

    o->moveNoteUp(o->getNotes()[1], &patch);

    EXPECT_EQ("2", o->getNotes()[0]->getName());
    EXPECT_EQ("1", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(0, patch.start);
    EXPECT_EQ(1, patch.count);

    o->moveNoteUp(o->getNotes()[2], &patch);

    EXPECT_EQ("2", o->getNotes()[0]->getName());
    EXPECT_EQ("3", o->getNotes()[1]->getName());
    EXPECT_EQ("1", o->getNotes()[2]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(1, patch.start);
    EXPECT_EQ(1, patch.count);

    o->moveNoteUp(o->getNotes()[1], &patch);

    EXPECT_EQ("3", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("1", o->getNotes()[2]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(0, patch.start);
    EXPECT_EQ(1, patch.count);

    // down

    o->moveNoteDown(o->getNotes()[0], &patch);

    EXPECT_EQ("2", o->getNotes()[0]->getName());
    EXPECT_EQ("3", o->getNotes()[1]->getName());
    EXPECT_EQ("1", o->getNotes()[2]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(0, patch.start);
    EXPECT_EQ(1, patch.count);

    o->moveNoteDown(o->getNotes()[1], &patch);

    EXPECT_EQ("2", o->getNotes()[0]->getName());
    EXPECT_EQ("1", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(1, patch.start);
    EXPECT_EQ(1, patch.count);

    o->moveNoteDown(o->getNotes()[0], &patch);

    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(0, patch.start);
    EXPECT_EQ(1, patch.count);
}

TEST(NoteTestCase, DeepUpDownFirstLastClone) {
    // prepare M8R repository and let the mind think...
    string repositoryDir{"/tmp/mf-unit-repository-o"};
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    string oFile{repositoryDir+"/memory/o.md"};
    string oContent{
        "# Note Operations Test Outline"
        "\nOutline text."
        "\n"
        "\n# 1"
        "\nT1."
        "\n"
        "\n# 2"
        "\nT2."
        "\n"
        "\n# 3"
        "\nT3."
        "\n"
        "\n## 33"
        "\nT33."
        "\n"
        "\n### 333"
        "\nT333."
        "\n"
        "\n# 4"
        "\nT4."
        "\n"
        "\n## 44"
        "\nT44."
        "\n"
        "\n# 5"
        "\nT5."
        "\n"
        "\n# 6"
        "\nT6."
        "\n"
        "\n"};
    m8r::stringToFile(oFile,oContent);

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-ntc-dudflc.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryDir)));
    m8r::Mind mind{config};
    m8r::Memory& memory = mind.remind();
    mind.learn();
    mind.think().get();

    // test
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* o = outlines.at(0);

    m8r::Outline::Patch patch{m8r::Outline::Patch::Diff::NO,0,0};

    //printOutlineNotes(o);
    EXPECT_EQ(9, o->getNotesCount());
    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ("33", o->getNotes()[3]->getName());
    EXPECT_EQ("333", o->getNotes()[4]->getName());
    EXPECT_EQ("4", o->getNotes()[5]->getName());
    EXPECT_EQ("44", o->getNotes()[6]->getName());
    EXPECT_EQ("5", o->getNotes()[7]->getName());
    EXPECT_EQ("6", o->getNotes()[8]->getName());

    // up

    o->moveNoteUp(o->getNotes()[5], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("4", o->getNotes()[2]->getName());
    EXPECT_EQ("44", o->getNotes()[3]->getName());
    EXPECT_EQ("3", o->getNotes()[4]->getName());
    EXPECT_EQ("33", o->getNotes()[5]->getName());
    EXPECT_EQ("333", o->getNotes()[6]->getName());
    EXPECT_EQ("5", o->getNotes()[7]->getName());
    EXPECT_EQ("6", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(2, patch.start);
    EXPECT_EQ(4, patch.count);

    // down

    o->moveNoteDown(o->getNotes()[2], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ("33", o->getNotes()[3]->getName());
    EXPECT_EQ("333", o->getNotes()[4]->getName());
    EXPECT_EQ("4", o->getNotes()[5]->getName());
    EXPECT_EQ("44", o->getNotes()[6]->getName());
    EXPECT_EQ("5", o->getNotes()[7]->getName());
    EXPECT_EQ("6", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(2, patch.start);
    EXPECT_EQ(4, patch.count);

    o->moveNoteDown(o->getNotes()[5], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ("33", o->getNotes()[3]->getName());
    EXPECT_EQ("333", o->getNotes()[4]->getName());
    EXPECT_EQ("5", o->getNotes()[5]->getName());
    EXPECT_EQ("4", o->getNotes()[6]->getName());
    EXPECT_EQ("44", o->getNotes()[7]->getName());
    EXPECT_EQ("6", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(5, patch.start);
    EXPECT_EQ(2, patch.count);

    o->moveNoteDown(o->getNotes()[6], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ("33", o->getNotes()[3]->getName());
    EXPECT_EQ("333", o->getNotes()[4]->getName());
    EXPECT_EQ("5", o->getNotes()[5]->getName());
    EXPECT_EQ("6", o->getNotes()[6]->getName());
    EXPECT_EQ("4", o->getNotes()[7]->getName());
    EXPECT_EQ("44", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(6, patch.start);
    EXPECT_EQ(2, patch.count);

    o->moveNoteDown(o->getNotes()[7], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("2", o->getNotes()[1]->getName());
    EXPECT_EQ("3", o->getNotes()[2]->getName());
    EXPECT_EQ("33", o->getNotes()[3]->getName());
    EXPECT_EQ("333", o->getNotes()[4]->getName());
    EXPECT_EQ("5", o->getNotes()[5]->getName());
    EXPECT_EQ("6", o->getNotes()[6]->getName());
    EXPECT_EQ("4", o->getNotes()[7]->getName());
    EXPECT_EQ("44", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::NO, patch.diff);

    // up

    o->moveNoteUp(o->getNotes()[2], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("1", o->getNotes()[0]->getName());
    EXPECT_EQ("3", o->getNotes()[1]->getName());
    EXPECT_EQ("33", o->getNotes()[2]->getName());
    EXPECT_EQ("333", o->getNotes()[3]->getName());
    EXPECT_EQ("2", o->getNotes()[4]->getName());
    EXPECT_EQ("5", o->getNotes()[5]->getName());
    EXPECT_EQ("6", o->getNotes()[6]->getName());
    EXPECT_EQ("4", o->getNotes()[7]->getName());
    EXPECT_EQ("44", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(1, patch.start);
    EXPECT_EQ(3, patch.count);

    o->moveNoteUp(o->getNotes()[1], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("3", o->getNotes()[0]->getName());
    EXPECT_EQ("33", o->getNotes()[1]->getName());
    EXPECT_EQ("333", o->getNotes()[2]->getName());
    EXPECT_EQ("1", o->getNotes()[3]->getName());
    EXPECT_EQ("2", o->getNotes()[4]->getName());
    EXPECT_EQ("5", o->getNotes()[5]->getName());
    EXPECT_EQ("6", o->getNotes()[6]->getName());
    EXPECT_EQ("4", o->getNotes()[7]->getName());
    EXPECT_EQ("44", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(0, patch.start);
    EXPECT_EQ(3, patch.count);

    o->moveNoteUp(o->getNotes()[0], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("3", o->getNotes()[0]->getName());
    EXPECT_EQ("33", o->getNotes()[1]->getName());
    EXPECT_EQ("333", o->getNotes()[2]->getName());
    EXPECT_EQ("1", o->getNotes()[3]->getName());
    EXPECT_EQ("2", o->getNotes()[4]->getName());
    EXPECT_EQ("5", o->getNotes()[5]->getName());
    EXPECT_EQ("6", o->getNotes()[6]->getName());
    EXPECT_EQ("4", o->getNotes()[7]->getName());
    EXPECT_EQ("44", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::NO, patch.diff);

    // first

    o->moveNoteToFirst(o->getNotes()[4], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("2", o->getNotes()[0]->getName());
    EXPECT_EQ("3", o->getNotes()[1]->getName());
    EXPECT_EQ("33", o->getNotes()[2]->getName());
    EXPECT_EQ("333", o->getNotes()[3]->getName());
    EXPECT_EQ("1", o->getNotes()[4]->getName());
    EXPECT_EQ("5", o->getNotes()[5]->getName());
    EXPECT_EQ("6", o->getNotes()[6]->getName());
    EXPECT_EQ("4", o->getNotes()[7]->getName());
    EXPECT_EQ("44", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(0, patch.start);
    EXPECT_EQ(4, patch.count);

    o->moveNoteToFirst(o->getNotes()[7], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("4", o->getNotes()[0]->getName());
    EXPECT_EQ("44", o->getNotes()[1]->getName());
    EXPECT_EQ("2", o->getNotes()[2]->getName());
    EXPECT_EQ("3", o->getNotes()[3]->getName());
    EXPECT_EQ("33", o->getNotes()[4]->getName());
    EXPECT_EQ("333", o->getNotes()[5]->getName());
    EXPECT_EQ("1", o->getNotes()[6]->getName());
    EXPECT_EQ("5", o->getNotes()[7]->getName());
    EXPECT_EQ("6", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(0, patch.start);
    EXPECT_EQ(8, patch.count);

    o->moveNoteToFirst(o->getNotes()[0], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("4", o->getNotes()[0]->getName());
    EXPECT_EQ("44", o->getNotes()[1]->getName());
    EXPECT_EQ("2", o->getNotes()[2]->getName());
    EXPECT_EQ("3", o->getNotes()[3]->getName());
    EXPECT_EQ("33", o->getNotes()[4]->getName());
    EXPECT_EQ("333", o->getNotes()[5]->getName());
    EXPECT_EQ("1", o->getNotes()[6]->getName());
    EXPECT_EQ("5", o->getNotes()[7]->getName());
    EXPECT_EQ("6", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::NO, patch.diff);

    // last

    o->moveNoteToLast(o->getNotes()[6], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("4", o->getNotes()[0]->getName());
    EXPECT_EQ("44", o->getNotes()[1]->getName());
    EXPECT_EQ("2", o->getNotes()[2]->getName());
    EXPECT_EQ("3", o->getNotes()[3]->getName());
    EXPECT_EQ("33", o->getNotes()[4]->getName());
    EXPECT_EQ("333", o->getNotes()[5]->getName());
    EXPECT_EQ("5", o->getNotes()[6]->getName());
    EXPECT_EQ("6", o->getNotes()[7]->getName());
    EXPECT_EQ("1", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(6, patch.start);
    EXPECT_EQ(2, patch.count);

    o->moveNoteToLast(o->getNotes()[3], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("4", o->getNotes()[0]->getName());
    EXPECT_EQ("44", o->getNotes()[1]->getName());
    EXPECT_EQ("2", o->getNotes()[2]->getName());
    EXPECT_EQ("5", o->getNotes()[3]->getName());
    EXPECT_EQ("6", o->getNotes()[4]->getName());
    EXPECT_EQ("1", o->getNotes()[5]->getName());
    EXPECT_EQ("3", o->getNotes()[6]->getName());
    EXPECT_EQ("33", o->getNotes()[7]->getName());
    EXPECT_EQ("333", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::MOVE, patch.diff);
    EXPECT_EQ(3, patch.start);
    EXPECT_EQ(5, patch.count);

    o->moveNoteToLast(o->getNotes()[6], &patch);

    //printOutlineNotes(o);
    EXPECT_EQ("4", o->getNotes()[0]->getName());
    EXPECT_EQ("44", o->getNotes()[1]->getName());
    EXPECT_EQ("2", o->getNotes()[2]->getName());
    EXPECT_EQ("5", o->getNotes()[3]->getName());
    EXPECT_EQ("6", o->getNotes()[4]->getName());
    EXPECT_EQ("1", o->getNotes()[5]->getName());
    EXPECT_EQ("3", o->getNotes()[6]->getName());
    EXPECT_EQ("33", o->getNotes()[7]->getName());
    EXPECT_EQ("333", o->getNotes()[8]->getName());
    EXPECT_EQ(m8r::Outline::Patch::Diff::NO, patch.diff);

    // clone

    o->cloneNote(o->getNotes()[3]);

    //printOutlineNotes(o);
    EXPECT_EQ("4", o->getNotes()[0]->getName());
    EXPECT_EQ("44", o->getNotes()[1]->getName());
    EXPECT_EQ("2", o->getNotes()[2]->getName());
    EXPECT_EQ("5", o->getNotes()[3]->getName());
    EXPECT_EQ("5", o->getNotes()[4]->getName());
    EXPECT_EQ("6", o->getNotes()[5]->getName());
    EXPECT_EQ("1", o->getNotes()[6]->getName());
    EXPECT_EQ("3", o->getNotes()[7]->getName());
    EXPECT_EQ("33", o->getNotes()[8]->getName());
    EXPECT_EQ("333", o->getNotes()[9]->getName());
    // Note data
    EXPECT_EQ(0, o->getNotes()[4]->getDepth());
    EXPECT_EQ(1, o->getNotes()[4]->getReads());
    EXPECT_EQ(1, o->getNotes()[4]->getRevision());

    o->cloneNote(o->getNotes()[0]);

    //printOutlineNotes(o);
    EXPECT_EQ("4", o->getNotes()[0]->getName());
    EXPECT_EQ("44", o->getNotes()[1]->getName());
    EXPECT_EQ("4", o->getNotes()[2]->getName());
    EXPECT_EQ("44", o->getNotes()[3]->getName());
    EXPECT_EQ("2", o->getNotes()[4]->getName());
    EXPECT_EQ("5", o->getNotes()[5]->getName());
    EXPECT_EQ("5", o->getNotes()[6]->getName());
    EXPECT_EQ("6", o->getNotes()[7]->getName());
    EXPECT_EQ("1", o->getNotes()[8]->getName());
    EXPECT_EQ("3", o->getNotes()[9]->getName());
    EXPECT_EQ("33", o->getNotes()[10]->getName());
    EXPECT_EQ("333", o->getNotes()[11]->getName());
    // Note data
    EXPECT_EQ(0, o->getNotes()[2]->getDepth());
    EXPECT_EQ(1, o->getNotes()[2]->getReads());
    EXPECT_EQ(1, o->getNotes()[2]->getRevision());
    EXPECT_EQ(1, o->getNotes()[3]->getDepth());
    EXPECT_EQ(1, o->getNotes()[3]->getReads());
    EXPECT_EQ(1, o->getNotes()[3]->getRevision());

    o->cloneNote(o->getNotes()[9]);

    //printOutlineNotes(o);
    EXPECT_EQ("4", o->getNotes()[0]->getName());
    EXPECT_EQ("44", o->getNotes()[1]->getName());
    EXPECT_EQ("4", o->getNotes()[2]->getName());
    EXPECT_EQ("44", o->getNotes()[3]->getName());
    EXPECT_EQ("2", o->getNotes()[4]->getName());
    EXPECT_EQ("5", o->getNotes()[5]->getName());
    EXPECT_EQ("5", o->getNotes()[6]->getName());
    EXPECT_EQ("6", o->getNotes()[7]->getName());
    EXPECT_EQ("1", o->getNotes()[8]->getName());
    EXPECT_EQ("3", o->getNotes()[9]->getName());
    EXPECT_EQ("33", o->getNotes()[10]->getName());
    EXPECT_EQ("333", o->getNotes()[11]->getName());
    EXPECT_EQ("3", o->getNotes()[12]->getName());
    EXPECT_EQ("33", o->getNotes()[13]->getName());
    EXPECT_EQ("333", o->getNotes()[14]->getName());
    // Note data
    EXPECT_EQ(0, o->getNotes()[12]->getDepth());
    EXPECT_EQ(1, o->getNotes()[12]->getReads());
    EXPECT_EQ(1, o->getNotes()[12]->getRevision());
    EXPECT_EQ(1, o->getNotes()[13]->getDepth());
    EXPECT_EQ(1, o->getNotes()[13]->getReads());
    EXPECT_EQ(1, o->getNotes()[13]->getRevision());
    EXPECT_EQ(2, o->getNotes()[14]->getDepth());
    EXPECT_EQ(1, o->getNotes()[14]->getReads());
    EXPECT_EQ(1, o->getNotes()[14]->getRevision());

    o->cloneNote(o->getNotes()[13]);

    //printOutlineNotes(o);
    EXPECT_EQ("4", o->getNotes()[0]->getName());
    EXPECT_EQ("44", o->getNotes()[1]->getName());
    EXPECT_EQ("4", o->getNotes()[2]->getName());
    EXPECT_EQ("44", o->getNotes()[3]->getName());
    EXPECT_EQ("2", o->getNotes()[4]->getName());
    EXPECT_EQ("5", o->getNotes()[5]->getName());
    EXPECT_EQ("5", o->getNotes()[6]->getName());
    EXPECT_EQ("6", o->getNotes()[7]->getName());
    EXPECT_EQ("1", o->getNotes()[8]->getName());
    EXPECT_EQ("3", o->getNotes()[9]->getName());
    EXPECT_EQ("33", o->getNotes()[10]->getName());
    EXPECT_EQ("333", o->getNotes()[11]->getName());
    EXPECT_EQ("3", o->getNotes()[12]->getName());
    EXPECT_EQ("33", o->getNotes()[13]->getName());
    EXPECT_EQ("333", o->getNotes()[14]->getName());
    EXPECT_EQ("33", o->getNotes()[15]->getName());
    EXPECT_EQ("333", o->getNotes()[16]->getName());
    // Note data
    EXPECT_EQ(1, o->getNotes()[15]->getDepth());
    EXPECT_EQ(1, o->getNotes()[15]->getReads());
    EXPECT_EQ(1, o->getNotes()[15]->getRevision());
    EXPECT_EQ(2, o->getNotes()[16]->getDepth());
    EXPECT_EQ(1, o->getNotes()[16]->getReads());
    EXPECT_EQ(1, o->getNotes()[16]->getRevision());
}


TEST(NoteTestCase, RefactorNote) {
    // prepare M8R repository and let the mind think...
    string repositoryDir{"/tmp/mf-unit-repository-refactor"};
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    // source O
    string sFile{repositoryDir+"/memory/source.md"};
    string sContent{
        "# Source Test Outline"
        "\nOutline text."
        "\n"
        "\n# 1"
        "\nT1."
        "\n"
        "\n# 2"
        "\nT2."
        "\n"
        "\n# 3"
        "\nT3."
        "\n"
        "\n## 33"
        "\nT33."
        "\n"
        "\n### 333"
        "\nT333."
        "\n"
        "\n# 4"
        "\nT4."
        "\n"
        "\n## 44"
        "\nT44."
        "\n"
        "\n# 5"
        "\nT5."
        "\n"
        "\n# 6"
        "\nT6."
        "\n"
        "\n"};
    m8r::stringToFile(sFile,sContent);

    // target O
    string tFile{repositoryDir+"/memory/target.md"};
    string tContent{
        "# Target Test Outline"
        "\nOutline text."
        "\n"
        "\n# A"
        "\nTA."
        "\n"
        "\n# B"
        "\nTB."
        "\n"
        "\n# C"
        "\nTC."
        "\n"
        "\n## CC"
        "\nTCC."
        "\n"
        "\n### CCC"
        "\nTCCC."
        "\n"
        "\n# D"
        "\nTD."
        "\n"
        "\n## DD"
        "\nTDD."
        "\n"
        "\n# E"
        "\nTE."
        "\n"
        "\n# F"
        "\nTF."
        "\n"
        "\n"};
    m8r::stringToFile(tFile,tContent);

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-ntc-rn.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryDir)));
    m8r::Mind mind{config};
    mind.learn();
    mind.think().get();

    // test
    m8r::Outline* s = mind.remind().getOutline(sFile);
    m8r::Outline* t = mind.remind().getOutline(tFile);
    EXPECT_TRUE(s != nullptr);
    EXPECT_TRUE(t != nullptr);
    EXPECT_EQ(2, mind.remind().getOutlinesCount());
    EXPECT_EQ(18, mind.remind().getNotesCount());

    EXPECT_EQ("3", s->getNotes()[2]->getName());

    mind.noteRefactor(s->getNotes()[2], t->getKey());

    // asserts
    EXPECT_EQ(9-3, s->getNotesCount());
    EXPECT_EQ("1", s->getNotes()[0]->getName());
    EXPECT_EQ("2", s->getNotes()[1]->getName());
    EXPECT_EQ("4", s->getNotes()[2]->getName());
    EXPECT_EQ(9+3, t->getNotesCount());
    EXPECT_EQ("3", t->getNotes()[0]->getName());
    EXPECT_EQ("33", t->getNotes()[1]->getName());
    EXPECT_EQ("333", t->getNotes()[2]->getName());
    EXPECT_EQ("A", t->getNotes()[3]->getName());

    // IMPROVE assert Os and Ns modified
}

TEST(NoteTestCase, MangleNoteName) {
    string repositoryDir{"/tmp/mf-unit-repository-m"};
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    string oFile{repositoryDir+"/memory/o.md"};
    string oContent{
        "# Section Links Mangling"
        "\n"
        "\nThis document elaborates mangling of section names to links."
        "\n"
        "\n# Section with space"
        "\n`section-with-space`"
        "\n"
        "\n# Section with 123 number"
        "\n`section-with-123-number`"
        "\n"
        "\n# Question?"
        "\n`question`"
        "\n"
        "\n#   Leading    within and trailing spaces"
        "\n`leading----within-and-trailing-spaces`"
        "\n"
        "\n# ?"
        "\n`` ... empty"
        "\n"
        "\n# @$%^&*("
        "\n`-1` ... probably GitHub bug (should be empty string)"
        "\n"};
    m8r::stringToFile(oFile,oContent);

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-ntc-mnn.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryDir)));
    m8r::Mind mind{config};
    m8r::Memory& memory = mind.remind();
    mind.learn();
    mind.think().get(); // ensure that ASYNC learning finishes

    // test
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* o = outlines.at(0);

    EXPECT_EQ(6, o->getNotesCount());
    EXPECT_EQ("section-with-space", o->getNotes()[0]->getMangledName());
    EXPECT_EQ("section-with-123-number", o->getNotes()[1]->getMangledName());
    EXPECT_EQ("question", o->getNotes()[2]->getMangledName());
    EXPECT_EQ("leading----within-and-trailing-spaces", o->getNotes()[3]->getMangledName());
    EXPECT_EQ("", o->getNotes()[4]->getMangledName());
    EXPECT_EQ("", o->getNotes()[5]->getMangledName());
}

TEST(NoteTestCase, DirectNoteChildren) {
    // prepare M8R repository and let the mind think...
    string repositoryDir{"/tmp/mf-unit-repository-n-child-n"};
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    string oFile{repositoryDir+"/memory/o.md"};
    /*
     * O . . .
     * . N . .
     * . . . 1
     * . . 2 .
     * . . . 3
     * . . 4 .
     * . 5 . .
     * 6 . . .
     *
     * Ns: 1, 2 and 4 to be returned.
     */
    string oContent{
        "# Outline"
        "\nO1."
        "\n## N"
        "\nTN."
        "\n#### 1" // direct child
        "\nT1."
        "\n### 2"
        "\nT2."
        "\n#### 3"
        "\nT3."
        "\n### 4" // direct child
        "\nT4"
        "\n## 5"
        "\nT5."
        "\n# 6"
        "\nT6."
        "\n"};
    m8r::stringToFile(oFile,oContent);

    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/cfg-ntc-ncn.md");
    config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryDir)));
    m8r::Mind mind{config};
    m8r::Memory& memory = mind.remind();
    mind.learn();
    mind.think().get();

    // test
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* o = outlines.at(0);
    m8r::Note* n = o->getNotes()[0];

    // asserts
    EXPECT_EQ(1, mind.remind().getOutlinesCount());
    EXPECT_EQ("N", n->getName());

    vector<m8r::Note*> directChildren{};
    o->getDirectNoteChildren(n, directChildren);
    EXPECT_EQ(3, directChildren.size());

    EXPECT_EQ("1", directChildren[0]->getName());
    EXPECT_EQ("2", directChildren[1]->getName());
    EXPECT_EQ("4", directChildren[2]->getName());
}
