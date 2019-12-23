/*
 organizer_view.cpp     MindForger thinking notebook

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
#include "dashboard_view.h"

namespace m8r {

DashboardView::DashboardView(QWidget* parent)
    : QSplitter{Qt::Horizontal,parent}
{
    left = new QSplitter(Qt::Vertical, this);
    left->setStretchFactor(0, 1);
    left->setStretchFactor(1, 1);

    // welcome
    welcomeDashboardlet = new QTextEdit(left);
    welcomeDashboardlet->document()->setHtml(
        QString(
             "<html><body bgcolor='#fff'>"
             "<font color='black'>"
             "<h1>We are MindForger</h1>"
             "Are you drowning in information, but starving for knowledge?"
             "<ul>"
             "<li><a href='https://www.mindforger.com'>Documentation</a></li>"
             "<li>Twitter, Facebook, YouTube</li>"
             "<li>Releases, source code and bugs</li>"
             "<li>Ubuntu</li>"
             "<li></li>"
             "</ul>"
             "</font></body></html>"
        ));

    // recent
    recentDashboardlet = new RecentNotesTableView(left, true);
    recentDashboardlet->size().setHeight(100);
    recentDashboardlet->size().setWidth(100);

    left->addWidget(welcomeDashboardlet);
    left->addWidget(recentDashboardlet);
    addWidget(left);

    right = new QSplitter(Qt::Vertical, this);
    right->setStretchFactor(0, 1);
    right->setStretchFactor(1, 1);

    // navigator
    navigatorDashboardlet = new NavigatorView(right);

    // organizer
    organizerDashboardlet = new OrganizerView(right);

    //right->addWidget(navigatorDashboardlet);
    right->addWidget(organizerDashboardlet);
    addWidget(right);

    setStretchFactor(0, 1);
    setStretchFactor(1, 1);
}

DashboardView::~DashboardView()
{
}

} // m8r namespace
