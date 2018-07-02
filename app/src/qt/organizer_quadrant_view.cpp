/*
 organizer_quadrant_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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

    // BEFARE this kills performance: verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(fontMetrics().height()*1.5);
    // IMPORTANT this must b in constructors - causes CPU high consuption loop if in paintEvent()!
    horizontalHeader()->setStretchLastSection(true);

    setSortingEnabled(true);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

OrganizerQuadrantView::~OrganizerQuadrantView()
{
}

void OrganizerQuadrantView::paintEvent(QPaintEvent* event)
{
    QTableView::paintEvent(event);
}

} // m8r namespace
