/*
 main_toolbar_view.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_MAIN_TOOLBAR_VIEW_H
#define M8RUI_MAIN_TOOLBAR_VIEW_H

#include <QtWidgets>

#include "main_window_view.h"
#include "cli_n_breadcrumbs_presenter.h"

namespace m8r {

class MainWindowView;
class CliAndBreadcrumbsView;

class MainToolbarView : public QToolBar
{
    Q_OBJECT

    MainWindowView* mainWindow;

    CliAndBreadcrumbsView* cli;

public:
    QAction* actionNewOutlineOrNote;
    QAction* actionOpenRepository;
    QAction* actionOpenFile;
    QAction* actionViewDashboard;
    QAction* actionViewOutlines;
    QAction* actionViewEisenhower;
    QAction* actionViewNavigator;
    QAction* actionViewTags;
    QAction* actionViewRecentNotes;
    QAction* actionFindFts;
    QAction* actionHomeOutline;
    QAction* actionThink;
    QAction* actionScope;
    QAction* actionAdapt;
    QAction* actionHelp;

public:
    explicit MainToolbarView(MainWindowView* mainWindowView);
    MainToolbarView(const MainToolbarView&) = delete;
    MainToolbarView(const MainToolbarView&&) = delete;
    MainToolbarView &operator=(const MainToolbarView&) = delete;
    MainToolbarView &operator=(const MainToolbarView&&) = delete;
    ~MainToolbarView();

    CliAndBreadcrumbsView* getCli() const { return cli;}
};

}
#endif // M8RUI_MAIN_TOOLBAR_VIEW_H
