/*
 recent_notes.cpp     MindForger thinking notebook

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
#include "recent_notes.h"

#include <algorithm>

namespace m8r {

using namespace std;

time_t RecentNotes::getTimestamp(const Note* note, Mode mode)
{
    if(mode == Mode::EDITED) {
        return note->getModified();
    }

    // modification does NOT update read timestamp > the latest of both
    return std::max(note->getRead(), note->getModified());
}

void RecentNotes::toHistory(
    const vector<Note*>& notes,
    Mode mode,
    size_t limit,
    vector<HistoryEntry>& history)
{
    history.clear();

    vector<Note*> sorted{};
    sorted.reserve(notes.size());
    for(Note* n:notes) {
        if(n && n->getOutline() && getTimestamp(n, mode) > 0) {
            sorted.push_back(n);
        }
    }

    // stable sort keeps the input order of Notes w/ the same timestamp > deterministic
    std::stable_sort(
        sorted.begin(),
        sorted.end(),
        [mode](const Note* n1, const Note* n2) {
            return getTimestamp(n1, mode) > getTimestamp(n2, mode);
        }
    );
    if(limit && sorted.size() > limit) {
        sorted.resize(limit);
    }

    history.reserve(sorted.size());
    for(Note* n:sorted) {
        history.push_back(HistoryEntry{n->getOutline(), n});
    }
}

void RecentNotes::groupConsecutive(
    const vector<HistoryEntry>& history,
    vector<Group>& groups)
{
    groups.clear();

    for(const HistoryEntry& entry:history) {
        if(!entry.first) {
            continue;
        }

        // new group whenever the Notebook differs from the previous entry's Notebook
        if(groups.empty() || groups.back().outline != entry.first) {
            groups.push_back(Group{entry.first, {}});
        }
        if(entry.second) {
            groups.back().notes.push_back(entry.second);
        }
    }
}

} // m8r namespace
