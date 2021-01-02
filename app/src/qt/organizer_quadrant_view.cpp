/*
 organizer_quadrant_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "organizer_quadrant_view.h"

namespace m8r {

OrganizerQuadrantView::OrganizerQuadrantView(QWidget* parent)
    : QTableView(parent)
{
    verticalHeader()->setVisible(false);

    // BEWARE this kills performance: verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(fontMetrics().height()*1.5);
    // IMPORTANT this must be in constructors - causes CPU high consuption loop if in paintEvent()!
    horizontalHeader()->setStretchLastSection(true);

    // IMPROVE sorting breaks width (redraw method to be overriden)
    setSortingEnabled(false);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void OrganizerQuadrantView::keyPressEvent(QKeyEvent* event)
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
            emit signalShowSelectedOutline();
            return;
        case Qt::Key_Down:
            QTableView::keyPressEvent(event);
            return;
        case Qt::Key_Up:
        // IMPROVE left to cancel selection
        case Qt::Key_Left:
            QTableView::keyPressEvent(event);
            return;
        }

        return;
    }

    QTableView::keyPressEvent(event);
}

void OrganizerQuadrantView::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    // double click to O/N opens it
    emit signalShowSelectedOutline();
}

} // m8r namespace
