/*
 recent_notes_table_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "recent_notes_table_view.h"

namespace m8r {

RecentNotesTableView::RecentNotesTableView(QWidget* parent)
    : QTableView(parent)
{
    verticalHeader()->setVisible(false);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    setSortingEnabled(true);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

RecentNotesTableView::~RecentNotesTableView()
{
}

void RecentNotesTableView::paintEvent(QPaintEvent* event)
{
    MF_DEBUG("RecentNotesTableView::paintEvent" << event << std::endl);

    // ensure that 1st column gets the remaining space from others
    // IMPROVE may kill performance
    this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    // N
    verticalHeader()->setDefaultSectionSize(fontMetrics().height()*1.5);

    // O
    int normalizedWidth = width()/fontMetrics().averageCharWidth();
    if(normalizedWidth < SIMPLIFIED_VIEW_THRESHOLD_WIDTH) {
        this->setColumnWidth(1, 0);
    } else {
        this->setColumnWidth(1, this->fontMetrics().averageCharWidth()*50);
    }

    // rds/wrs
    this->setColumnWidth(2, this->fontMetrics().averageCharWidth()*5);
    this->setColumnWidth(3, this->fontMetrics().averageCharWidth()*5);

    // pretty: rd/wr
    this->setColumnWidth(4, this->fontMetrics().averageCharWidth()*12);
    // pretty
    this->setColumnWidth(5, this->fontMetrics().averageCharWidth()*12);

    QTableView::paintEvent(event);
}

} // m8r namespace
