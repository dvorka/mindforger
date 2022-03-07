/*
 outline_tree_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "outline_tree_view.h"

namespace m8r {

OutlineTreeView::OutlineTreeView(QWidget* parent)
    : QTableView(parent)
{
    verticalHeader()->setVisible(false);

    // BEFARE: this kills performance in case of big(ger) tables
    // verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    setSortingEnabled(false);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    // disable TAB and Ctrl+O up/down navigation (Ctrl+O no longer bound)
    setTabKeyNavigation(false);

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}

void OutlineTreeView::keyPressEvent(QKeyEvent* event)
{
    // leave note view navigation
#ifndef __APPLE__
    if(event->modifiers() & Qt::AltModifier) {
        if(event->key()==Qt::Key_Left) {
            emit signalFromOutlineTreeToOutlines();
        }
    } else {
#endif
        // up/down/promote/demote note tree changes
        if(event->modifiers() & Qt::ControlModifier){
            if(event->modifiers() & Qt::ShiftModifier) {
                switch(event->key()) {
                case Qt::Key_Up:
                    emit signalChangeFirst();
                    break;
                case Qt::Key_Down:
                    emit signalChangeLast();
                    break;
                }
            } else {
                switch(event->key()) {
#ifndef __APPLE__
                case Qt::Key_Up:
                    emit signalChangeUp();
                    break;
                case Qt::Key_Down:
                    emit signalChangeDown();
                    break;
                case Qt::Key_Left:
                    emit signalChangePromote();
                    break;
                case Qt::Key_Right:
                    emit signalChangeDemote();
                    break;
#else
                case Qt::Key_L:
                    emit signalFromOutlineTreeToOutlines();
                    break;
                case Qt::Key_D:
                    emit signalForget();
                    break;
#endif
                case Qt::Key_E:
                    emit signalOutlineOrNoteEdit();
                    break;
                case Qt::Key_X:
                    emit signalOutlineOrNoteExternalEdit();
                    break;
                }
            }
        } else {
            // up/down note tree navigation
            switch(event->key()) {
            case Qt::Key_Escape:
                emit signalOutlineShow();
                break;
            case Qt::Key_Up:
                emit signalSelectPreviousRow();
                break;
            case Qt::Key_Down:
                emit signalSelectNextRow();
                break;
            case Qt::Key_Home:
                MF_DEBUG("  OutlineTreeView::keyPressEvent HOME" << endl);
                if(this->model()->rowCount() > 0) {
                    this->selectRow(0);
                }
                return;
            case Qt::Key_End:
                MF_DEBUG("  OutlineTreeView::keyPressEvent END" << endl);
                if(this->model()->rowCount() > 0) {
                    this->selectRow(this->model()->rowCount()-1);
                }
                return;
            case Qt::Key_PageUp: {
                MF_DEBUG("  OutlineTreeView::keyPressEvent PAGE_UP" << endl);
                // get currently selected row
                QModelIndexList indices = selectionModel()->selection().indexes();
                // no indexes > no row > no selection
                for(int i=0; i<indices.count(); i++) {
                    // calculate row to select (based on the number of rows)
                    int currentRow = indices.at(i).row();
                    int newRow{currentRow - PG_UP_DOWN_STEP_SIZE};
                    if(newRow < 0) {
                        newRow = 0;
                    }
                    // select row
                    this->selectRow(newRow);
                }
                // no row selected
                return;
            }
            case Qt::Key_PageDown: {
                MF_DEBUG("  OutlineTreeView::keyPressEvent PAGE_DOWN" << endl);
                // get currently selected row
                QModelIndexList indices = selectionModel()->selection().indexes();
                // no indexes > no row > no selection
                for(int i=0; i<indices.count(); i++) {
                    // calculate row to select (based on the number of rows)
                    int currentRow = indices.at(i).row();
                    int newRow{currentRow + PG_UP_DOWN_STEP_SIZE};
                    if(newRow > this->model()->rowCount()-1) {
                        newRow = this->model()->rowCount()-1;
                    }
                    // select row
                    this->selectRow(newRow);
                }
                // no row selected
                return;
                }
            case Qt::Key_Return:
            case Qt::Key_Right:
                emit signalEdit();
                break;
#ifndef __APPLE__
            case Qt::Key_Delete:
                emit signalForget();
                break;
#endif
            case Qt::Key_Left:
                signalFromOutlineTreeToOutlines();
                break;
            }
        }
#ifndef __APPLE__
    }
#endif

    QWidget::keyPressEvent(event);
}

void OutlineTreeView::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    // double click to N opens it
    emit signalEdit();
}

void OutlineTreeView::resizeEvent(QResizeEvent* event)
{
    MF_DEBUG("OutlineTreeView::resizeEvent " << event << std::endl);

    if(horizontalHeader()->length() > 0) {
        // ensure that 1st column gets the remaining space from others
        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    }
    verticalHeader()->setDefaultSectionSize(fontMetrics().height()*1.5);

    int normalizedWidth = width()/fontMetrics().averageCharWidth();
    if(normalizedWidth < SIMPLIFIED_VIEW_THRESHOLD_WIDTH) {
        this->setColumnHidden(1, true);
        this->setColumnHidden(2, true);
        this->setColumnHidden(3, true);
    } else {
        if(this->isColumnHidden(1)) {
            this->setColumnHidden(1, false);
            this->setColumnHidden(2, false);
            this->setColumnHidden(3, false);
        }
        // progress
        this->setColumnWidth(1, this->fontMetrics().averageCharWidth()*6);
        // rd/wr
        this->setColumnWidth(2, this->fontMetrics().averageCharWidth()*5);
        this->setColumnWidth(3, this->fontMetrics().averageCharWidth()*5);
    }

    // pretty
    this->setColumnWidth(4, this->fontMetrics().averageCharWidth()*12);

    QTableView::resizeEvent(event);
}

} // m8r namespace
