/*
 organizer_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "dashboard_view.h"

namespace m8r {

DashboardView::DashboardView(QWidget* parent)
    : QSplitter{Qt::Horizontal, parent},
      isMindForgerRepository(false)
{
    left = new QSplitter{Qt::Vertical, this};
    left->setStretchFactor(0, 1);
    left->setStretchFactor(1, 1);

    middle = new QSplitter{Qt::Vertical, this};
    middle->setStretchFactor(0, 1);
    middle->setStretchFactor(1, 1);

    right = new QSplitter{Qt::Vertical, this};
    right->setStretchFactor(0, 1);
    right->setStretchFactor(1, 1);

    // welcome
    welcomeDashboardlet = new QTextBrowser(left);
    welcomeDashboardlet->setOpenExternalLinks(true);
    left->addWidget(welcomeDashboardlet);

    // recent
    recentDashboardlet = new RecentNotesTableView(left, true);
    left->addWidget(recentDashboardlet);

    // organizer quadrants
    doFirstDashboardlet = new OrganizerQuadrantView(middle);
    middle->addWidget(doFirstDashboardlet);

    // tags
    tagsDashboardlet = new TagsTableView(middle);
    middle->addWidget(tagsDashboardlet);

    // navigator
    navigatorDashboardlet = new NavigatorView(right, true);
    // IMPROVE should go to resize event
    int windowHeight=parent->parentWidget()->parentWidget()->size().height();
    navigatorDashboardlet->setFixedHeight(windowHeight);
    right->addWidget(navigatorDashboardlet);

    // outlines
    outlinesDashboardlet = new OutlinesTableView(right, true);
    right->addWidget(outlinesDashboardlet);

    // self ~ horizontal splitter
    setStretchFactor(0, 1);
    setStretchFactor(1, 1);
    setStretchFactor(2, 1);

    addWidget(left);
    addWidget(middle);
    addWidget(right);
}

void DashboardView::setMindForgerMode(bool isMindForgerRepository)
{
    this->isMindForgerRepository = isMindForgerRepository;
    if(isMindForgerRepository) {
        middle->setVisible(true);
    } else {
        middle->setVisible(false);
    }
}

void DashboardView::resizeEvent(QResizeEvent* event)
{
    UNUSED_ARG(event);

    int normalizedWidth = width()/fontMetrics().averageCharWidth();
    if(!isMindForgerRepository || normalizedWidth < SIMPLIFIED_VIEW_THRESHOLD_WIDTH) {
        middle->setVisible(false);
    } else {
        middle->setVisible(true);
    }

    QSplitter::resizeEvent(event);
}

DashboardView::~DashboardView()
{
}

} // m8r namespace
