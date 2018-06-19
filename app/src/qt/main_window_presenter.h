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

#include "../../lib/src/mind/mind.h"
#include "../../lib/src/representations/html/html_outline_representation.h"
#include "../../lib/src/representations/markdown/markdown_configuration_representation.h"

#include "main_window_view.h"
#include "main_menu_presenter.h"

#include "gear/async_task_notifications_distributor.h"
#include "ner_main_window_worker_thread.h"
#include "cli_n_breadcrumbs_presenter.h"
#include "orloj_presenter.h"
#include "status_bar_presenter.h"

#include "dialogs/outline_new_dialog.h"
#include "dialogs/note_new_dialog.h"
#include "dialogs/fts_dialog.h"
#include "dialogs/find_outline_by_name_dialog.h"
#include "dialogs/find_outline_by_tag_dialog.h"
#include "dialogs/find_note_by_name_dialog.h"
#include "dialogs/find_note_by_tag_dialog.h"
#include "dialogs/refactor_note_to_outline_dialog.h"
#include "dialogs/scope_dialog.h"
#include "dialogs/configuration_dialog.h"
#include "dialogs/insert_image_dialog.h"
#include "dialogs/insert_link_dialog.h"
#include "dialogs/rows_and_depth_dialog.h"
#include "dialogs/new_repository_dialog.h"
#include "dialogs/new_file_dialog.h"
#include "dialogs/ner_choose_tag_types_dialog.h"
#include "dialogs/ner_result_dialog.h"

#include <QtWidgets>
#include <QtConcurrent/QtConcurrent>

namespace m8r {

class MainWindowView;
class MainMenuPresenter;
class CliAndBreadcrumbsPresenter;
class OrlojPresenter;
class StatusBarPresenter;
class AsyncTaskNotificationsDistributor;
class NerMainWindowWorkerThread;

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

    Configuration& config;
    Mind* mind;

    AsyncTaskNotificationsDistributor* distributor;
    NerMainWindowWorkerThread* nerWorker;

    MarkdownOutlineRepresentation* mdRepresentation;
    HtmlOutlineRepresentation* htmlRepresentation;
    MarkdownConfigurationRepresentation* mdConfigRepresentation;

    MainMenuPresenter* mainMenu;
    CliAndBreadcrumbsPresenter* cli;
    OrlojPresenter* orloj;
    StatusBarPresenter* statusBar;

    ScopeDialog* scopeDialog;
    OutlineNewDialog* newOutlineDialog;
    NoteNewDialog* newNoteDialog;
    FtsDialog* ftsDialog;
    FindOutlineByNameDialog* findOutlineByNameDialog;
    FindNoteByNameDialog* findNoteByNameDialog;
    FindOutlineByTagDialog* findOutlineByTagDialog;
    FindNoteByTagDialog* findNoteByTagDialog;
    RefactorNoteToOutlineDialog* refactorNoteToOutlineDialog;
    ConfigurationDialog* configDialog;
    InsertImageDialog* insertImageDialog;
    InsertLinkDialog* insertLinkDialog;
    RowsAndDepthDialog* rowsAndDepthDialog;
    NewRepositoryDialog* newRepositoryDialog;
    NewFileDialog* newFileDialog;
    NerChooseTagTypesDialog *nerChooseTagsDialog;
    NerResultDialog* nerResultDialog;

public:
    explicit MainWindowPresenter(MainWindowView& view);
    MainWindowPresenter(const MainWindowPresenter&) = delete;
    MainWindowPresenter(const MainWindowPresenter&&) = delete;
    MainWindowPresenter &operator=(const MainWindowPresenter&) = delete;
    MainWindowPresenter &operator=(const MainWindowPresenter&&) = delete;
    ~MainWindowPresenter();

    // this presenter view
    MainWindowView& getView() const { return view; }
    const Configuration& getConfiguration() const { return config; }
    MarkdownOutlineRepresentation* getMarkdownRepresentation() const { return mdRepresentation; }
    HtmlOutlineRepresentation* getHtmlRepresentation() const { return htmlRepresentation; }
    AsyncTaskNotificationsDistributor* getDistributor() const { return distributor; }

    // presenters
    CliAndBreadcrumbsPresenter* getCli() { return cli; }
    OrlojPresenter* getOrloj() const { return orloj; }
    MainMenuPresenter* getMainMenu() const { return mainMenu; }
    StatusBarPresenter* getStatusBar() const { return statusBar; }

    // function
    Mind* getMind() const { return mind; }

    // dashboard(s)
    void showInitialView();

    // N view
    void handleNoteViewLinkClicked(const QUrl& url);

    // NER
    NerMainWindowWorkerThread* startNerWorkerThread(Mind* m, OrlojPresenter* o, std::vector<NerNamedEntity>* r, QDialog* d);

public slots:
    // mind
#ifdef DO_MF_DEBUG
    void doActionMindHack();
#endif
    void doActionMindNewRepository();
    void handleMindNewRepository();
    void doActionMindNewFile();
    void handleMindNewFile();
    void doActionMindThink();
    void doActionMindSleep();
    void doActionMindLearnRepository();
    void doActionMindLearnFile();
    void doActionMindRelearn(QString path);
    void doActionMindTimeScope();
    void handleMindScope();
    void doActionMindPreferences();
    void handleMindPreferences();
    void doActionMindRemember();
    void doActionMindSnapshot();
    void doActionExit();
    // recall
    void doActionFts();
    void handleFts();
    void doActionFindOutlineByName();
    void handleFindOutlineByName();
    void doActionFindNoteByName();
    void handleFindNoteByName();
    void doActionFindOutlineByTag();
    void handleFindOutlineByTag();
    void doActionFindNoteByTag();
    void handleFindNoteByTag();
    void doActionFindNerPersons();
    void handleFindNerPersons();
    void chooseNerEntityResult(vector<NerNamedEntity>* nerEntities);
    void handleChooseNerEntityResult();
    void handleFtsNerEntity();
    // view
    bool doActionViewHome();
    void doActionViewOutlines();
    void doActionViewToggleRecent();
    void doActionCli();
    void doActionViewDistractionFree();
    void doActionViewFullscreen();
    // format
    void doActionFormatBold();
    void doActionFormatItalic();
    void doActionFormatCode();
    void doActionFormatStrikethrough();
    void doActionFormatKeyboard();
    void handleRowsAndDepth();
    void doActionFormatListBullet();
    void doActionFormatListNumber();
    void doActionFormatListTask();
    void doActionFormatCodeblock();
    void doActionFormatBlockquote();
    void doActionFormatLink();
    void handleFormatLink();
    void doActionFormatImage();
    void handleFormatImage();
    void doActionFormatTable();
    void doActionFormatHr();
    // Outline
    void doActionOutlineNew();
    void doActionOutlineEdit();
    void handleOutlineNew();
    void doActionOutlineClone();
    void doActionOutlineHome();
    void doActionOutlineForget();
    void doActionOutlineExport();
    void doActionOutlineImport();
    // Note
    void doActionNoteNew();
    void handleNoteNew();
    void doActionNoteHoist();
    void doActionNoteEdit();
    void doActionNoteFirst();
    void doActionNoteUp();
    void doActionNotePromote();
    void doActionNoteDemote();
    void doActionNoteDown();
    void doActionNoteLast();
    void doActionNoteClone();
    void doActionNoteExtract();
    void doActionRefactorNoteToOutline();
    void handleRefactorNoteToOutline();
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
