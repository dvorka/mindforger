/*
 main_window_view.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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

#ifndef M8RUI_MAIN_WINDOW_VIEW_H
#define M8RUI_MAIN_WINDOW_VIEW_H

#include <QtWidgets>

#include "../../lib/src/version.h"

#include "main_window_presenter.h"
#include "main_toolbar_view.h"
#include "left_toolbar_view.h"
#include "main_menu_presenter.h"
#include "cli_n_breadcrumbs_view.h"
#include "orloj_view.h"
#include "status_bar_view.h"

namespace m8r {

class OrlojView;
class MainWindowPresenter;
class MainToolbarView;
class LeftToolbarView;
class CliAndBreadcrumbsView;

/**
 * @brief MindForger main window View.
 *
 * Main window is a special (and the only) case that violates
 * MVP - View instantiates Presenter. However, this View class
 * aims to be application logic code and backend dependency free.
 */
class MainWindowView: public QMainWindow
{
    Q_OBJECT

private:
    QString windowTitleSkeleton;

    LookAndFeels& lookAndFeel;

    QWidget* centralWidget;
    QVBoxLayout *centralLayout;

    OrlojView* orlojView;
    MainToolbarView* toolBarView;
    LeftToolbarView* leftToolBarView;
    StatusBarView* statusBarView;

public:
    explicit MainWindowView(LookAndFeels& lookAndFeel);
    MainWindowView(const MainWindowView&) = delete;
    MainWindowView(const MainWindowView&&) = delete;
    MainWindowView &operator=(const MainWindowView&) = delete;
    MainWindowView &operator=(const MainWindowView&&) = delete;
    virtual ~MainWindowView();

    QMenuBar* getMenuBar() const { return menuBar(); }
    MainToolbarView* getToolBar() const { return toolBarView; }
    LeftToolbarView* getLeftToolBar() const { return leftToolBarView; }
    CliAndBreadcrumbsView* getCli() const;
    OrlojView* getOrloj() const { return orlojView; }
    StatusBarView* getStatusBar() const { return statusBarView; }

    void setFileOrDirectory(QString f);
};

}

#endif // M8RUI_MAIN_WINDOW_VIEW_H
