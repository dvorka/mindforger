/*
 organizer_quadrant_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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

using namespace std;

OrganizerQuadrantView::OrganizerQuadrantView(QWidget* parent, ViewType viewType)
    : QTableView(parent),
      viewType{viewType}
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

#ifdef WIP_DRAG_N_DROP
    // drag & drop (https://doc.qt.io/qt-5/dnd.html)
    setDragDropMode(QAbstractItemView::InternalMove);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
#endif
}

void OrganizerQuadrantView::keyPressEvent(QKeyEvent* event)
{
    if((event->modifiers() & Qt::ControlModifier)) {
        switch(event->key()) {
        case Qt::Key_Right:
            MF_DEBUG("EMIT ctrl-right");
            emit signalMoveNoteToNextQuadrant();
            return;
        }
    }

    if(!(event->modifiers() & Qt::AltModifier)
       && !(event->modifiers() & Qt::ControlModifier)
       && !(event->modifiers() & Qt::ShiftModifier)
    ) {
        switch(event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Right:
            emit signalShowSelectedNote();
            return;
        case Qt::Key_Tab:
            emit signalFocusToNextVisibleQuadrant();
            break;
        case Qt::Key_Backtab:
            emit signalFocusToLastVisibleQuadrant();
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
    if(ViewType::ORGANIZER == this->viewType) {
        emit signalShowSelectedNote();
    } else if(ViewType::KANBAN == this->viewType) {
        emit signalShowSelectedKanbanNote();
    }
}

#ifdef WIP_DRAG_N_DROP

/*
 * Drag & drop.
 *
 * https://www.youtube.com/watch?v=dcrSTeVaW5Y
 */

void OrganizerQuadrantView::dragEnterEvent(QDragEnterEvent* event)
{
    MF_DEBUG("DRAG EVENT: " << event << endl);
    MF_DEBUG("DRAG EVENT: " << event->mimeData() << endl);

    event->acceptProposedAction();

    //if (event->mimeData()->hasFormat("text/plain")) {
    //    event->acceptProposedAction();
    //}
}

void OrganizerQuadrantView::dropEvent(QDropEvent* event)
{
    MF_DEBUG("DROP EVENT: " << event << endl);
    MF_DEBUG("DROP EVENT MIME: " << event->mimeData() << endl);
    MF_DEBUG("DROP EVENT formats: " << event->mimeData()->formats()[0].toStdString() << endl);
    MF_DEBUG("DROP EVENT text: " << event->mimeData()->text().toStdString() << endl);

    // TODO decode: "application/x-qabstractitemmodeldatalist"
    QByteArray encoded = event->mimeData()->data(
        event->mimeData()->formats()[0].toStdString().c_str()
    );
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    while (!stream.atEnd()) {
        int row, col;
        QMap<int,  QVariant> roleDataMap;

        stream >> row >> col >> roleDataMap;
        MF_DEBUG("  row: " << row << " col: " << col << endl);
        //MF_DEBUG("    data: " << roleDataMap.value(Qt::UserRole+1).toString().toStdString() << endl);
    }

    MF_DEBUG("DROP EVENT done: " << event << endl);
    event->acceptProposedAction();
}
#endif

} // m8r namespace
