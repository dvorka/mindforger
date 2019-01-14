/*
 notes_table_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "notes_table_view.h"

namespace m8r {

NotesTableView::NotesTableView(QWidget *parent)
    : QTableView(parent)
{
    verticalHeader()->setVisible(false);

    setSortingEnabled(true);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    // disable TAB and Ctrl-O up/down navigation
    setTabKeyNavigation(false);

    verticalHeader()->setDefaultSectionSize(fontMetrics().height()*1.5);

    QHeaderView* header = this->horizontalHeader();
    // IMPORTANT resize mode MUST NOT be set in paintEvent, otherwise it causes high CPU consumption loop
    header->setSectionResizeMode(QHeaderView::Stretch);
}

} // m8r namespace
