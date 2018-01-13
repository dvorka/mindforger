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
#include "lib/src/representations/markdown/markdown_outline_representation.h"

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

    MarkdownOutlineRepresentation* markdownRepresentation;

    MainMenuPresenter* mainMenu;
    CliAndBreadcrumbsPresenter* cli;
    OrlojPresenter* orloj;
    StatusBarPresenter* statusBar;

    OutlineNewDialog* newOutlineDialog;
    NoteNewDialog* newNoteDialog;
    FtsDialog* ftsDialog;
    FindOutlineByNameDialog* findOutlineByNameDialog;
    FindNoteByNameDialog* findNoteByNameDialog;

public:
    explicit MainWindowPresenter(MainWindowView& view, Configuration& configuration);
    MainWindowPresenter(const MainWindowPresenter&) = delete;
    MainWindowPresenter(const MainWindowPresenter&&) = delete;
    MainWindowPresenter &operator=(const MainWindowPresenter&) = delete;
    MainWindowPresenter &operator=(const MainWindowPresenter&&) = delete;
    ~MainWindowPresenter();

    // this presenter view
    MainWindowView& getView(void) const { return view; }
    void initView(void);
    const Configuration& getConfiguration(void) const { return configuration; }

    // presenters
    CliAndBreadcrumbsPresenter* getCli(void) { return cli; }
    OrlojPresenter* getOrloj(void) const { return orloj; }
    StatusBarPresenter* getStatusBar(void) const { return statusBar; }       

    // function
    Mind* getMind(void) const { return mind; }

public slots:
    // mindforger
#ifdef DO_MF_DEBUG
    void doActionMindHack(void);
#endif
    void doActionRepositoryOpen(void);
    void doActionRepositoryRecent(void);
    void doActionPreferences(void);
    void doActionMindSnapshot(void);
    void doActionExit(void);
    // find
    void doActionFts(void);
    void handleFts(void);
    void doActionFindOutlineByName(void);
    void handleFindOutlineByName(void);
    void doActionFindNoteByName(void);
    void doActionFindNoteByTag(void);
    // view
    void doActionViewOutlines(void);
    void doActionViewToggleRecent(void);
    void doActionCli(void);
    void doActionViewDistractionFree(void);
    void doActionViewFullscreen(void);
    // outline
    void doActionOutlineNew(void);
    void handleOutlineNew(void);
    void doActionOutlineOpen(void);
    void doActionOutlineClose(void);
    void doActionOutlineForget(void);
    void doActionOutlineExport(void);
    void doActionOutlineImport(void);
    // note
    void doActionNoteNew(void);
    void handleNoteNew(void);
    void doActionNoteOpen(void);
    void doActionNoteFirst(void);
    void doActionNoteUp(void);
    void doActionNotePromote(void);
    void doActionNoteDemote(void);
    void doActionNoteDown(void);
    void doActionNoteLast(void);
    void doActionNoteAttach(void);
    void doActionNoteSave(void);
    void doActionNoteForget(void);
    // help
    void doActionHelpDocumentation(void);
    void doActionHelpWeb(void);
    void doActionHelpReportBug(void);
    void doActionHelpCheckForUpdates(void);
    void doActionHelpAboutMindForger(void);

    void executeFts(const std::string& command, const bool ignoreCase=false, Outline* scope=nullptr) const;
};

}

#endif // M8RUI_MAIN_WINDOW_PRESENTER_H
