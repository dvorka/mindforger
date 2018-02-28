/*
 outline_tree_view.cpp     MindForger thinking notebook

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
#include "outline_tree_view.h"

namespace m8r {

OutlineTreeView::OutlineTreeView(QWidget* parent)
    : QTableView(parent)
{
    verticalHeader()->setVisible(false);
    // BEFARE this kills performance: verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    setSortingEnabled(false);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    // disable TAB and Ctrl+O up/down navigation (Ctrl+O no longer bound)
    setTabKeyNavigation(false);
}

void OutlineTreeView::keyPressEvent(QKeyEvent* event)
{
    // leave note view navigation
    if(event->modifiers() & Qt::AltModifier){
        if(event->key()==Qt::Key_Left) {
            signalFromOutlineTreeToOutlines();
        }
    } else {
        // TODO up/down/promote/demote note tree changes
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
                //case Qt::Key_F:
                //    emit signalChangeFirst();
                //    break;
                //case Qt::Key_L:
                //    emit signalChangeLast();
                //    break;
                }
            }
        } else {
            // up/down note tree navigation
            switch(event->key()) {
            case Qt::Key_Up:
                emit signalSelectPreviousRow();
                break;
            case Qt::Key_Down:
                emit signalSelectNextRow();
                break;
            case Qt::Key_Return:
                emit signalEdit();
                break;
            case Qt::Key_Delete:
                emit signalForget();
                break;
            }
        }
    }

    QWidget::keyPressEvent(event);
}

void OutlineTreeView::paintEvent(QPaintEvent* event)
{
    // ensure that 1st column gets the remaining space from others
    // IMPROVE may kill performance
    this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(fontMetrics().height()*1.5);

    // progress
    this->setColumnWidth(1, this->fontMetrics().averageCharWidth()*6);
    // rd/wr
    this->setColumnWidth(2, this->fontMetrics().averageCharWidth()*5);
    this->setColumnWidth(3, this->fontMetrics().averageCharWidth()*5);
    // pretty
    this->setColumnWidth(4, this->fontMetrics().averageCharWidth()*12);

    QTableView::paintEvent(event);
}

}
