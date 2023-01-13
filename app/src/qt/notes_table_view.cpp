/*
 notes_table_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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

NotesTableView::NotesTableView(QWidget* parent)
    : QTableView(parent)
{
    verticalHeader()->setVisible(false);

    setSortingEnabled(true);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    // disable TAB and Ctrl-O up/down navigation
    setTabKeyNavigation(false);
}

void NotesTableView::keyPressEvent(QKeyEvent* event)
{
    if(!(event->modifiers() & Qt::AltModifier)
         &&
       !(event->modifiers() & Qt::ControlModifier)
         &&
       !(event->modifiers() & Qt::ShiftModifier))
    {
        switch(event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Right:
            emit signalShowSelectedNote();
            return;
        case Qt::Key_Down:
        case Qt::Key_Up:
        case Qt::Key_Left:
            QTableView::keyPressEvent(event);
            return;
        case Qt::Key_Escape:
            emit signalEscape();
        }

        return;
    }

    QTableView::keyPressEvent(event);
}

void NotesTableView::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    // double click to O/N opens it
    emit signalShowSelectedNote();
}

void NotesTableView::resizeEvent(QResizeEvent* event)
{
    MF_DEBUG("NotesTableView::resizeEvent " << event << std::endl);

    if(horizontalHeader()->length() > 0) {
        // ensure that 1st column gets the remaining space from others
        horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
    verticalHeader()->setDefaultSectionSize(fontMetrics().height()*1.5);

    QTableView::resizeEvent(event);
}

} // m8r namespace
