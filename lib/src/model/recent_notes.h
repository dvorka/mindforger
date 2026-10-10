/*
 recent_notes.h     MindForger thinking notebook

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
#ifndef M8R_RECENT_NOTES_H
#define M8R_RECENT_NOTES_H

#include <ctime>
#include <utility>
#include <vector>

#include "outline.h"
#include "note.h"

namespace m8r {

/**
 * @brief Recent Notes - history of Notes grouped by Notebook.
 *
 * History is a list of (Notebook, Note) pairs ordered by recency (latest
 * first). Only consecutive history entries from the same Notebook are
 * grouped, therefore a Notebook may have more groups - one for each
 * uninterrupted sequence of its Notes in the history.
 */
class RecentNotes
{
public:
    /**
     * @brief Which Note timestamps make Note recent.
     */
    enum class Mode {
        // Note was viewed (read) or edited (modified)
        VIEWED_OR_EDITED,
        // Note was edited (modified)
        EDITED
    };

    /**
     * @brief History entry: Notebook and its Note - nullptr Note means Notebook itself.
     */
    typedef std::pair<Outline*, Note*> HistoryEntry;

    /**
     * @brief Notebook w/ its consecutive recent Notes (latest first).
     */
    struct Group {
        Outline* outline;
        std::vector<Note*> notes;
    };

public:
    /**
     * @brief Get the timestamp which determines Note's recency in given mode.
     */
    static time_t getTimestamp(const Note* note, Mode mode);

    /**
     * @brief Create history from Notes.
     *
     * @param notes     Notes (any order) - Notes w/o Notebook or timestamp are skipped,
     *                  Notebook descriptor Notes become Notebook entries (nullptr Note).
     * @param mode      timestamp to be used to determine Note recency.
     * @param limit     max number of (most recent) Notes to keep, 0 means no limit.
     * @param history   result: (Notebook, Note) pairs ordered by recency (latest first).
     */
    static void toHistory(
        const std::vector<Note*>& notes,
        Mode mode,
        size_t limit,
        std::vector<HistoryEntry>& history);

    /**
     * @brief Group consecutive history entries from the same Notebook.
     *
     * @param history   (Notebook, Note) pairs - entries w/o Notebook are skipped.
     * @param groups    result: groups in the history order.
     */
    static void groupConsecutive(
        const std::vector<HistoryEntry>& history,
        std::vector<Group>& groups);
};

}
#endif // M8R_RECENT_NOTES_H
