/*
 organizer_view.cpp     MindForger thinking notebook

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
#include "organizer_view.h"

namespace m8r {

OrganizerView::OrganizerView(QWidget* parent)
    : QSplitter{Qt::Horizontal,parent}
{
    left = new QSplitter(this);
    addWidget(left);
    right = new QSplitter(this);
    addWidget(right);

    // the first parameter is index
    setStretchFactor(0, 1);
    setStretchFactor(1, 1);
}

OrganizerView::~OrganizerView()
{
}

void OrganizerView::refresh(std::vector<Outline*> outlines)
{
}

} // m8r namespace
