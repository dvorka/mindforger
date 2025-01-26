/*
 main_toolbar_view.h     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
    QAction* actionViewOutlines;
    QAction* actionViewOrganizers;
    QAction* actionViewNavigator;
    QAction* actionViewTags;
    QAction* actionViewRecentNotes;
    QAction* actionHomeOutline;
    QAction* actionThink;
    QAction* actionScope;
    QAction* actionAdapt;
    QAction* actionHelp;

    // IMPORTANT: hide event hidden as it was causing undesired configuration
    // changes and toolbar hiding on Qt's spontaneous hide/show events. Citation
    // from Qt doc:
    //
    // "Note: A widget receives spontaneous show and hide events when its mapping
    // status is changed by the window system, e.g. a spontaneous hide event when
    // the user minimizes the window, and a spontaneous show event when the window
    // is restored again. After receiving a spontaneous hide event, a widget is
    // still considered visible in the sense of isVisible()."
    //
    // Even use of this->isVisible() within the event didn't fixed the problem.
    //
    // @see https://github.com/dvorka/mindforger/issues/1437
    //
    // void hideEvent(QHideEvent *) override;

public:
    explicit MainToolbarView(MainWindowView* mainWindowView);
    MainToolbarView(const MainToolbarView&) = delete;
    MainToolbarView(const MainToolbarView&&) = delete;
    MainToolbarView &operator=(const MainToolbarView&) = delete;
    MainToolbarView &operator=(const MainToolbarView&&) = delete;
    ~MainToolbarView();

    CliAndBreadcrumbsView* getCli() const { return cli;}

signals:
    void signalMainToolbarVisibilityChanged(bool visibility);
};

}
#endif // M8RUI_MAIN_TOOLBAR_VIEW_H
