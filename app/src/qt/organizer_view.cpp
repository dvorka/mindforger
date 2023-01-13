/*
 organizer_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "organizer_view.h"

namespace m8r {

OrganizerView::OrganizerView(QWidget* parent)
    : QSplitter{Qt::Horizontal,parent}
{
    left = new QSplitter(Qt::Vertical, this);
    left->setStretchFactor(0, 1);
    left->setStretchFactor(1, 1);
    doSoon = new OrganizerQuadrantView(left);
    doSometime = new OrganizerQuadrantView(left);
    left->addWidget(doSoon);
    left->addWidget(doSometime);
    addWidget(left);

    right = new QSplitter(Qt::Vertical, this);
    right->setStretchFactor(0, 1);
    right->setStretchFactor(1, 1);
    doFirst = new OrganizerQuadrantView(right);
    planDedicatedTime = new OrganizerQuadrantView(right);
    right->addWidget(doFirst);
    right->addWidget(planDedicatedTime);
    addWidget(right);

    setStretchFactor(0, 1);
    setStretchFactor(1, 1);
}

OrganizerView::~OrganizerView()
{
}

} // m8r namespace
