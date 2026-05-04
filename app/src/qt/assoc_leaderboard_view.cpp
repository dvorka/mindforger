/*
 assoc_leaderboard_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "assoc_leaderboard_view.h"

namespace m8r {

AssocLeaderboardView::AssocLeaderboardView(QWidget* parent)
    : QTableView(parent)
{
    verticalHeader()->setVisible(false);
    // BEFARE this kills performance: verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // IMPORTANT this must be in constructors - causes CPU high consuption loop if in an event handler
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    setSortingEnabled(false);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void AssocLeaderboardView::resizeEvent(QResizeEvent* event)
{
    MF_DEBUG("AssocLeaderboardView::resizeEvent " << event << std::endl);

    if(horizontalHeader()->length() > 0) {
        // ensure that 1st column gets the remaining space from others
        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    }
    verticalHeader()->setDefaultSectionSize(fontMetrics().height()*1.5);

    // % associativity
    this->setColumnWidth(1, this->fontMetrics().averageCharWidth()*12);

    QTableView::resizeEvent(event);
}

} // m8r namespace
