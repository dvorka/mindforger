/*
 left_toolbar_view.cpp     MindForger thinking notebook

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
#include "left_toolbar_view.h"

namespace m8r {

LeftToolbarView::LeftToolbarView(MainWindowView* mainWindowView)
    : QToolBar{tr("Tool Bar"), mainWindowView},
      mainWindow{mainWindowView}
{
    actionLeftToolbarArxiv = addAction(
        QIcon(":/icons/adapt.svg"),
        "Open arXiv and find papers related to the current context... (Alt-1)");

    actionLeftToolbarWikipedia = addAction(
        QIcon(":/icons/adapt.svg"),
        "Open Wikipedia and find entries related to the current context... (Alt-2)");

    actionLeftToolbarStackOverflow = addAction(
        QIcon(":/icons/adapt.svg"),
        "Open StackOverflow and find entries related to the current context... (Alt-3)");

    actionLeftToolbarDuckDuckGo = addAction(
        QIcon(":/icons/adapt.svg"),
        "Open DuckDuckGo and find entries related to the current context... (Alt-5)");

    actionLeftToolbarGitHub = addAction(
        QIcon(":/icons/adapt.svg"),
        "Open GitHub and find entries related to the current context... (Alt-6)");

    actionLeftToolbarBard = addAction(
        QIcon(":/icons/adapt.svg"),
        "Open Bard and chat about the current context... (Alt-7)");
}

LeftToolbarView::~LeftToolbarView()
{
}

} // m8r namespace
