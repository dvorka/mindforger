/*
 ner_leaderboard_view.cpp     MindForger thinking notebook

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
#include "ner_leaderboard_view.h"

namespace m8r {

NerLeaderboardView::NerLeaderboardView(QWidget* parent)
    : QTableView(parent)
{
    verticalHeader()->setVisible(false);
    // BEFARE this kills performance: verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // IMPORTANT this must b in constructors - causes CPU high consuption loop if in paintEvent()!
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    setSortingEnabled(true);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);

    // ensure HTML cells rendering
    HtmlDelegate* delegate = new HtmlDelegate();
    setItemDelegate(delegate);
}

NerLeaderboardView::~NerLeaderboardView()
{
}

void NerLeaderboardView::paintEvent(QPaintEvent* event)
{
    // ensure that 1st column gets the remaining space from others
    // IMPROVE may kill performance
    this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    verticalHeader()->setDefaultSectionSize(fontMetrics().height()*1.5);

    // type
    this->setColumnWidth(1, this->fontMetrics().averageCharWidth()*15);
    // %
    this->setColumnWidth(2, this->fontMetrics().averageCharWidth()*12);

    QTableView::paintEvent(event);
}

} // m8r namespace
