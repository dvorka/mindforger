/*
 main_menu_presenter.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_MAIN_MENU_PRESENTER_H
#define M8RUI_MAIN_MENU_PRESENTER_H

#include <QtWidgets>

#include "main_menu_view.h"
#include "main_window_presenter.h"

namespace m8r {

class MainMenuView;
class MainWindowPresenter;

/**
 * @brief The MindForger main menu.
 *
 * Declares and implements actions w/ application logic,
 * binds them using slots/signals and adds them to menu View.
 */
class MainMenuPresenter : public QObject
{
    Q_OBJECT

private:
    MainMenuView* view;

    MainWindowPresenter* mainWindowPresenter;
    Configuration& config;

public:
    MainMenuPresenter(MainWindowPresenter* mainWindowPresenter);
    MainMenuPresenter(const MainMenuPresenter&) = delete;
    MainMenuPresenter(const MainMenuPresenter&&) = delete;
    MainMenuPresenter &operator=(const MainMenuPresenter&) = delete;
    MainMenuPresenter &operator=(const MainMenuPresenter&&) = delete;
    virtual ~MainMenuPresenter();

    MainMenuView* getView() { return view; }

    void showFacetDashboard();
    void showFacetOrganizerList();
    void showFacetOrganizer();
    void showFacetNavigator();

    void showFacetOutlineList();
    void showFacetOutlineView();
    void showFacetNoteEdit();

    void showFacetMindThink();
    void showFacetMindSleep();

    void showFacetMindAutolink(bool enabled);

    void showFacetLiveNotePreview(bool enabled);

    void addRecentDirectoryOrFile(const QString& path);
};

}

#endif // M8RUI_MAIN_MENU_PRESENTER_H
