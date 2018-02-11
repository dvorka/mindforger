/*
 main_window_presenter.h     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_MAIN_WINDOW_PRESENTER_H
#define M8RUI_MAIN_WINDOW_PRESENTER_H

#include "lib/src/config/configuration.h"
#include "lib/src/mind/mind.h"

#include "main_window_view.h"
#include "main_menu_presenter.h"
#include "cli_n_breadcrumbs_presenter.h"
#include "orloj_presenter.h"
#include "status_bar_presenter.h"

#include "dialogs/outline_new_dialog.h"
#include "dialogs/note_new_dialog.h"
#include "dialogs/fts_dialog.h"
#include "dialogs/find_outline_by_name_dialog.h"
#include "dialogs/find_note_by_name_dialog.h"
#include "dialogs/refactor_note_to_outline_dialog.h"

#include <QtWidgets>

namespace m8r {

class MainWindowView;
class MainMenuPresenter;
class CliAndBreadcrumbsPresenter;
class OrlojPresenter;
class StatusBarPresenter;

/**
 * @brief MindForger main window Presenter.
 *
 * This class is used to demonstrate MindForger's MVP frontend architecture
 * and code conventions.
 *
 * Main window presenter:
 *   * Implements core UI application logic for other presenters and views.
 *   * Provides index of all UI presenters.
 */
class MainWindowPresenter : public QObject
{
    Q_OBJECT

private:
    MainWindowView& view;

    Configuration& configuration;
    Mind* mind;

    MarkdownOutlineRepresentation* mdRepresentation;

    MainMenuPresenter* mainMenu;
    CliAndBreadcrumbsPresenter* cli;
    OrlojPresenter* orloj;
    StatusBarPresenter* statusBar;

    OutlineNewDialog* newOutlineDialog;
    NoteNewDialog* newNoteDialog;
    FtsDialog* ftsDialog;
    FindOutlineByNameDialog* findOutlineByNameDialog;
    FindNoteByNameDialog* findNoteByNameDialog;
    RefactorNoteToOutlineDialog* refactorNoteToOutlineDialog;

public:
    explicit MainWindowPresenter(MainWindowView& view, Configuration& configuration);
    MainWindowPresenter(const MainWindowPresenter&) = delete;
    MainWindowPresenter(const MainWindowPresenter&&) = delete;
    MainWindowPresenter &operator=(const MainWindowPresenter&) = delete;
    MainWindowPresenter &operator=(const MainWindowPresenter&&) = delete;
    ~MainWindowPresenter();

    // this presenter view
    MainWindowView& getView() const { return view; }
    void initView();
    const Configuration& getConfiguration() const { return configuration; }
    MarkdownOutlineRepresentation* getMarkdownRepresentation() const { return mdRepresentation; }

    // presenters
    CliAndBreadcrumbsPresenter* getCli() { return cli; }
    OrlojPresenter* getOrloj() const { return orloj; }
    MainMenuPresenter* getMainMenu() const { return mainMenu; }
    StatusBarPresenter* getStatusBar() const { return statusBar; }

    // function
    Mind* getMind() const { return mind; }

public slots:
    // mind
#ifdef DO_MF_DEBUG
    void doActionMindHack();
#endif
    void doActionMindLearn();
    void doActionMindSnapshot();
    void doActionExit();
    // recall
    void doActionFts();
    void handleFts();
    void doActionFindOutlineByName();
    void handleFindOutlineByName();
    void doActionFindNoteByName();
    void handleFindNoteByName();
    void doActionFindNoteByTag();
    // view
    void doActionViewOutlines();
    void doActionViewToggleRecent();
    void doActionCli();
    void doActionViewDistractionFree();
    void doActionViewFullscreen();
    // outline
    void doActionOutlineNew();
    void handleOutlineNew();
    void doActionOutlineClone();
    void doActionOutlineForget();
    void doActionOutlineExport();
    void doActionOutlineImport();
    // note
    void doActionNoteNew();
    void handleNoteNew();
    void doActionNoteFirst();
    void doActionNoteUp();
    void doActionNotePromote();
    void doActionNoteDemote();
    void doActionNoteDown();
    void doActionNoteLast();
    void doActionNoteAttach();
    void doActionNoteClone();
    void doActionRefactorNoteToOutline();
    void handleRefactorNoteToOutline();
    void doActionNoteSave();
    void doActionNoteForget();
    // help
    void doActionHelpDocumentation();
    void doActionHelpWeb();
    void doActionHelpReportBug();
    void doActionHelpCheckForUpdates();
    void doActionHelpAboutMindForger();

    void executeFts(const std::string& command, const bool ignoreCase=false, Outline* scope=nullptr) const;
};

}

#endif // M8RUI_MAIN_WINDOW_PRESENTER_H
