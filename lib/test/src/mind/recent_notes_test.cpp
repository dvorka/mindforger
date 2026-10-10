/*
 recent_notes_test.cpp     MindForger test

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <vector>

#include <gtest/gtest.h>

#include "../../../src/model/recent_notes.h"

using namespace std;

namespace {

m8r::Note* createNote(m8r::Outline* o, const string& name, time_t read, time_t modified)
{
    m8r::Note* n = new m8r::Note{nullptr, o};
    n->setName(name);
    n->setRead(read);
    n->setModified(modified);
    o->addNote(n);
    return n;
}

} // anonymous namespace

TEST(RecentNotesTestCase, ToHistoryViewedOrEdited)
{
    // GIVEN Notes viewed and edited at different times
    m8r::Outline a{nullptr}, b{nullptr};
    m8r::Note* a1 = createNote(&a, "A1", 100, 10);
    m8r::Note* a2 = createNote(&a, "A2", 0, 500);  // edited, never viewed
    m8r::Note* b1 = createNote(&b, "B1", 300, 20);
    m8r::Note* b2 = createNote(&b, "B2", 50, 40);
    vector<m8r::Note*> notes{a1, b2, a2, b1};

    // WHEN
    vector<m8r::RecentNotes::HistoryEntry> history{};
    m8r::RecentNotes::toHistory(notes, m8r::RecentNotes::Mode::VIEWED_OR_EDITED, 0, history);

    // THEN entries are latest first by the later of read/modified timestamps
    ASSERT_EQ(4u, history.size());
    EXPECT_EQ(m8r::RecentNotes::HistoryEntry(&a, a2), history[0]);
    EXPECT_EQ(m8r::RecentNotes::HistoryEntry(&b, b1), history[1]);
    EXPECT_EQ(m8r::RecentNotes::HistoryEntry(&a, a1), history[2]);
    EXPECT_EQ(m8r::RecentNotes::HistoryEntry(&b, b2), history[3]);
}

TEST(RecentNotesTestCase, ToHistoryEditedOnly)
{
    // GIVEN the most recently viewed Note is NOT the most recently edited
    m8r::Outline a{nullptr}, b{nullptr};
    m8r::Note* a1 = createNote(&a, "A1", 900, 10);
    m8r::Note* b1 = createNote(&b, "B1", 0, 20);
    m8r::Note* b2 = createNote(&b, "B2", 0, 30);
    vector<m8r::Note*> notes{a1, b1, b2};

    // WHEN
    vector<m8r::RecentNotes::HistoryEntry> history{};
    m8r::RecentNotes::toHistory(notes, m8r::RecentNotes::Mode::EDITED, 0, history);

    // THEN reads are ignored
    ASSERT_EQ(3u, history.size());
    EXPECT_EQ(b2, history[0].second);
    EXPECT_EQ(b1, history[1].second);
    EXPECT_EQ(a1, history[2].second);
}

TEST(RecentNotesTestCase, ToHistoryLimitAndSkipping)
{
    // GIVEN Notes including one w/o timestamp and one w/o Notebook
    m8r::Outline a{nullptr}, b{nullptr};
    m8r::Note* a1 = createNote(&a, "A1", 0, 300);
    m8r::Note* a2 = createNote(&a, "A2", 0, 0);   // no timestamp
    m8r::Note* b1 = createNote(&b, "B1", 0, 200);
    m8r::Note* b2 = createNote(&b, "B2", 0, 100); // beyond the limit
    m8r::Note orphan{nullptr, nullptr};
    orphan.setModified(1000);
    vector<m8r::Note*> notes{a1, a2, b1, b2, &orphan, nullptr};

    // WHEN
    vector<m8r::RecentNotes::HistoryEntry> history{};
    m8r::RecentNotes::toHistory(notes, m8r::RecentNotes::Mode::EDITED, 2, history);

    // THEN only the 2 most recent Notes w/ Notebook and timestamp are kept
    ASSERT_EQ(2u, history.size());
    EXPECT_EQ(a1, history[0].second);
    EXPECT_EQ(b1, history[1].second);
}

TEST(RecentNotesTestCase, ToHistoryNotebookDescriptors)
{
    // GIVEN Notes and Notebook descriptors (as Notes) - like when Notebooks are included in recent
    m8r::Outline a{nullptr}, b{nullptr};
    m8r::Note* a1 = createNote(&a, "A1", 0, 400);
    m8r::Note* a2 = createNote(&a, "A2", 0, 200);
    a.setModified(300);
    b.setModified(500);
    m8r::Note* aDescriptor = a.getOutlineDescriptorAsNote();
    m8r::Note* bDescriptor = b.getOutlineDescriptorAsNote();
    vector<m8r::Note*> notes{aDescriptor, a1, a2, bDescriptor};

    // WHEN
    vector<m8r::RecentNotes::HistoryEntry> history{};
    m8r::RecentNotes::toHistory(notes, m8r::RecentNotes::Mode::EDITED, 0, history);
    vector<m8r::RecentNotes::Group> groups{};
    m8r::RecentNotes::groupConsecutive(history, groups);

    // THEN descriptors are Notebook entries, so Notebooks are NOT shown as their own Notes
    ASSERT_EQ(4u, history.size());
    EXPECT_EQ(m8r::RecentNotes::HistoryEntry(&b, nullptr), history[0]);
    EXPECT_EQ(m8r::RecentNotes::HistoryEntry(&a, a1), history[1]);
    EXPECT_EQ(m8r::RecentNotes::HistoryEntry(&a, nullptr), history[2]);
    EXPECT_EQ(m8r::RecentNotes::HistoryEntry(&a, a2), history[3]);

    ASSERT_EQ(2u, groups.size());
    EXPECT_EQ(&b, groups[0].outline);
    EXPECT_TRUE(groups[0].notes.empty());
    EXPECT_EQ(&a, groups[1].outline);
    ASSERT_EQ(2u, groups[1].notes.size());
    EXPECT_EQ(a1, groups[1].notes[0]);
    EXPECT_EQ(a2, groups[1].notes[1]);
}

TEST(RecentNotesTestCase, GroupConsecutiveSplitsInterruptedNotebooks)
{
    // GIVEN history where Notebook A is interrupted by Notebook B
    m8r::Outline a{nullptr}, b{nullptr};
    m8r::Note* a1 = createNote(&a, "A1", 0, 1);
    m8r::Note* a2 = createNote(&a, "A2", 0, 1);
    m8r::Note* a3 = createNote(&a, "A3", 0, 1);
    m8r::Note* b1 = createNote(&b, "B1", 0, 1);
    vector<m8r::RecentNotes::HistoryEntry> history{
        {&a, a3}, {&a, a2}, {&b, b1}, {&a, a1}
    };

    // WHEN
    vector<m8r::RecentNotes::Group> groups{};
    m8r::RecentNotes::groupConsecutive(history, groups);

    // THEN A is at the top level twice - only consecutive Notes are grouped
    ASSERT_EQ(3u, groups.size());
    EXPECT_EQ(&a, groups[0].outline);
    ASSERT_EQ(2u, groups[0].notes.size());
    EXPECT_EQ(a3, groups[0].notes[0]);
    EXPECT_EQ(a2, groups[0].notes[1]);
    EXPECT_EQ(&b, groups[1].outline);
    ASSERT_EQ(1u, groups[1].notes.size());
    EXPECT_EQ(b1, groups[1].notes[0]);
    EXPECT_EQ(&a, groups[2].outline);
    ASSERT_EQ(1u, groups[2].notes.size());
    EXPECT_EQ(a1, groups[2].notes[0]);
}

TEST(RecentNotesTestCase, GroupConsecutiveNotebookEntriesAndSkipping)
{
    // GIVEN history w/ Notebook visits (no Note) and an entry w/o Notebook
    m8r::Outline a{nullptr}, b{nullptr};
    m8r::Note* a1 = createNote(&a, "A1", 0, 1);
    vector<m8r::RecentNotes::HistoryEntry> history{
        {&b, nullptr}, {&a, nullptr}, {&a, a1}, {nullptr, a1}, {&a, nullptr}
    };

    // WHEN
    vector<m8r::RecentNotes::Group> groups{};
    m8r::RecentNotes::groupConsecutive(history, groups);

    // THEN Notebook visits create/continue groups w/o Notes, entries w/o Notebook are skipped
    ASSERT_EQ(2u, groups.size());
    EXPECT_EQ(&b, groups[0].outline);
    EXPECT_TRUE(groups[0].notes.empty());
    EXPECT_EQ(&a, groups[1].outline);
    ASSERT_EQ(1u, groups[1].notes.size());
    EXPECT_EQ(a1, groups[1].notes[0]);
}

TEST(RecentNotesTestCase, GroupConsecutiveEmpty)
{
    // GIVEN
    vector<m8r::RecentNotes::HistoryEntry> history{};
    vector<m8r::RecentNotes::Group> groups{};

    // WHEN
    m8r::RecentNotes::groupConsecutive(history, groups);

    // THEN
    EXPECT_TRUE(groups.empty());
}
