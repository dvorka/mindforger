/*
 note_refactoring_test.cpp     MindForger test

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

using namespace std;

TEST(NoteTestCase, AddNewStencilNoteToOutline) {
    // prepare M8R repository and let the mind think...
    string repositoryDir{"/tmp/mf-unit-repository-o"};
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    string stencilFile{"/tmp/mf-unit-repository-o/stencils/outlines/o-s.md"};
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
    stencilFile.assign("/tmp/mf-unit-repository-o/stencils/notes/n-s.md");
    stencilContent.assign("## Stencil Single Note\nNote text.\n");
    m8r::stringToFile(stencilFile,stencilContent);
    string oFile{"/tmp/mf-unit-repository-o/memory/outline.md"};
    string oContent{"# Test Outline\n\nOutline text.\n\n## Note 1\nNote 1 text.\n\n##Note 2\nNote 2 text.\n"};
    m8r::stringToFile(oFile,oContent);

    m8r::Configuration configuration{repositoryDir};
    m8r::Mind mind{configuration};
    m8r::Memory& memory = mind.remind();
    mind.think();

    // get the outline and add to it a stencil note
    vector<m8r::Outline*> outlines = memory.getOutlines();
    m8r::Outline* o = outlines.at(0);
    vector<m8r::Stencil*> stencils = memory.getStencils(m8r::ResourceType::NOTE);
    string title{"!!! STENCIL Note !!!"};
    mind.noteNew(
                o->getKey(),
                0,
                &title,
                mind.ontology().findOrCreateNoteType(m8r::NoteType::KeyAction()),
                mind.ontology().findOrCreateTag(m8r::Tag::KeyImportant()),
                stencils.at(0));
    // insert note in the middle and to the depth 1
    string deepTitle{"!!! DEEP Note !!!"};
    // insert note in the middle and to the depth 1
    m8r::Note* newNote = mind.noteNew(
                o->getKey(),
                2,
                &deepTitle,
                mind.ontology().findOrCreateNoteType(m8r::NoteType::KeyAction()),
                mind.ontology().findOrCreateTag(m8r::Tag::KeyImportant()),
                stencils.at(0));
    newNote->demote();
    //cout << "Begin:" << endl; for(m8r::Note* nn: o->getNotes()) { cout << "  #x" << nn->getDepth() << " " << nn->getTitle() << endl; }

    // asserts
    EXPECT_EQ(oFile, o->getKey());
    EXPECT_EQ(string{"Test Outline"}, o->getTitle());
    EXPECT_EQ(o->getNotesCount(), 4);
    EXPECT_EQ(title, o->getNotes()[0]->getTitle());
    EXPECT_EQ(string{"Note 1"}, o->getNotes()[1]->getTitle());
    EXPECT_EQ(deepTitle, o->getNotes()[2]->getTitle());
    EXPECT_EQ(string{"Note 2"}, o->getNotes()[3]->getTitle());

    // delete NOTE
    mind.noteForget(o->getNotes()[3]);
    //cout << "After delete:" << endl; for(m8r::Note* nn: o->getNotes()) { cout << "  #x" << nn->getDepth() << " " << nn->getTitle() << endl; }
    EXPECT_EQ(o->getNotesCount(), 3);
    EXPECT_EQ(title, o->getNotes()[0]->getTitle());
    EXPECT_EQ(string{"Note 1"}, o->getNotes()[1]->getTitle());
    EXPECT_EQ(deepTitle, o->getNotes()[2]->getTitle());

    // delete NOTE w/ DEEP child
    mind.noteForget(o->getNotes()[1]);
    //cout << "After DEEP delete:" << endl; for(m8r::Note* nn: o->getNotes()) { cout << "  #x" << nn->getDepth() << " " << nn->getTitle() << endl; }
    EXPECT_EQ(o->getNotesCount(), 1);
    EXPECT_EQ(title, o->getNotes()[0]->getTitle());

    // IMPROVE test add note on offset (virified manually that it works)
    mind.noteForget(o->getNotes()[0]);
    EXPECT_EQ(o->getNotesCount(), 0);

    mind.remind().remember(o);

    // delete OUTLINE
    EXPECT_EQ(mind.remind().getOutlinesCount(), 1);
    mind.outlineForget(o->getKey());
    EXPECT_EQ(mind.remind().getOutlinesCount(), 0);
}

// MUST TODO void testOutlineDelete(void)

// MUST TODO void testNoteDelete(void)


// TODO void testNoteBegin(void)
// TODO void testNoteEnd(void)
// MUST TODO void testNoteUp(void)
// MUST TODO void testNoteDown(void)

// TODO void testNoteTop(void)
// TODO void testNoteBottom(void)
// MUST TODO void testNotePromote(void)
// MUST TODO void testNoteDemote(void)

// TODO void testNoteForget(void)

// TODO void testNoteClone(void)
// TODO void testNoteMoveToOtherOutline(void)
// TODO void testNoteMakeItOutline(void)
// TODO void testOutlineMakeItNoteInOutline(void)
// TODO void testOutlineMakeNoteNewOutline(void)

// TODO void testGetFilepathToNote(void)
// TODO void testGetFilepathToOutline(void)
