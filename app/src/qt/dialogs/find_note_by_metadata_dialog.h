/*
 find_note_by_metadata_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_FIND_NOTE_BY_METADATA_DIALOG_H
#define M8RUI_FIND_NOTE_BY_METADATA_DIALOG_H

#include <vector>

#include <QtWidgets>

#include "../../lib/src/model/note.h"

#include "../model_meta_definitions.h"

namespace m8r {

/**
 * @brief Find Note by metadata dialog.
 *
 * Table of Notes w/ their metadata (size, reads, writes, created, modified)
 * which cannot be filtered, but can be sorted by any column using the header
 * e.g. to find the Note w/ the longest text or the oldest Notes.
 */
class FindNoteByMetadataDialog : public QDialog
{
    Q_OBJECT

public:
    enum Column {
        COLUMN_NOTE = 0,
        COLUMN_NOTEBOOK,
        COLUMN_SIZE,
        COLUMN_READS,
        COLUMN_WRITES,
        COLUMN_CREATED,
        COLUMN_MODIFIED,

        COLUMN_COUNT
    };

    // item data roles: Note pointer (1st column) and the value used for sorting
    static constexpr int ROLE_NOTE = Qt::UserRole + 1;
    static constexpr int ROLE_SORT = Qt::UserRole + 2;

private:
    QTableView* tableView;
    QStandardItemModel* model;
    // sorting is done by the proxy: rows sorted in QStandardItemModel make
    // its next clearing O(n^2) i.e. extremely slow for thousands of Notes
    QSortFilterProxyModel* proxyModel;

    QPushButton* openButton;
    QPushButton* closeButton;

    Note* choice;

public:
    explicit FindNoteByMetadataDialog(QWidget* parent);
    FindNoteByMetadataDialog(const FindNoteByMetadataDialog&) = delete;
    FindNoteByMetadataDialog(const FindNoteByMetadataDialog&&) = delete;
    FindNoteByMetadataDialog &operator=(const FindNoteByMetadataDialog&) = delete;
    FindNoteByMetadataDialog &operator=(const FindNoteByMetadataDialog&&) = delete;
    ~FindNoteByMetadataDialog();

    /**
     * @brief Show the dialog w/ the given Notes - the largest Notes first.
     *
     * @param showNotebook  show Notebook column - useless if all Notes are
     *                      from the same (opened) Notebook.
     */
    void show(const std::vector<Note*>& notes, bool showNotebook);

    Note* getChoice() const { return choice; }

private:
    void initColumns();
    void addRow(Note* note);

signals:
    void searchFinished();

private slots:
    void handleChoice();
    void handleSelectionChanged();
};

}
#endif // M8RUI_FIND_NOTE_BY_METADATA_DIALOG_H
