/*
 kanban_view.cpp     MindForger thinking notebook

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
#include "kanban_view.h"

namespace m8r {

KanbanView::KanbanView(int columnsCount, QWidget* parent)
    : QSplitter{Qt::Horizontal,parent},
    columns{}
{
    if(columnsCount < 0 || columnsCount> 4) {
        throw "Kanban view column index is out of range: " + std::to_string(this->columnsCount);
    }

    for(int i=0; i<columnsCount; i++) {
        KanbanColumnView* k = new KanbanColumnView(this);

        addWidget(k);

        columns.push_back(k);
    }

    // TODO to be stretched
    // setStretchFactor(0, 1);
}

KanbanView::~KanbanView()
{
}

} // m8r namespace
