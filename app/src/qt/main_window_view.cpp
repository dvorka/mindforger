/*
 main_window_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "main_window_view.h"

namespace m8r {

MainWindowView::MainWindowView(LookAndFeels& lookAndFeel)
    : QMainWindow(nullptr), // main window has no parent - it is destroyed by main MF class
      lookAndFeel(lookAndFeel)
{
    windowTitleSkeleton = "MindForger - "+tr("Thinking Notebook")+" - "+MINDFORGER_VERSION;
    setWindowTitle(windowTitleSkeleton);

    toolBarView = new MainToolbarView{this};
    // IMPROVE toolbar position to be configurable
    addToolBar(Qt::TopToolBarArea, toolBarView);

    leftToolBarView = new LeftToolbarView{this};
    addToolBar(Qt::LeftToolBarArea, leftToolBarView);

    centralWidget = new QWidget(this);

    centralLayout = new QVBoxLayout{centralWidget};
    orlojView = new OrlojView{centralWidget};
    centralLayout->addWidget(orlojView);

    centralWidget->setLayout(centralLayout);
    setCentralWidget(centralWidget);

    statusBarView = new StatusBarView(statusBar(), lookAndFeel);
}

MainWindowView::~MainWindowView()
{
    delete centralWidget;
}

CliAndBreadcrumbsView* MainWindowView::getCli() const
{
    return toolBarView->getCli();
}

void MainWindowView::setFileOrDirectory(QString f)
{
    setWindowTitle(windowTitleSkeleton+" - "+f+" ");
}

} // m8r namespace
