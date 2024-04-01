/*
 main_window_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_MAIN_WINDOW_PRESENTER_H
#define M8RUI_MAIN_WINDOW_PRESENTER_H

#include <chrono>

#include "../../lib/src/mind/mind.h"
#include "../../lib/src/mind/ai/autolinking_preprocessor.h"
#include "../../lib/src/mind/dikw/filesystem_information.h"
#include "../../lib/src/representations/html/html_outline_representation.h"
#include "../../lib/src/representations/markdown/markdown_configuration_representation.h"
#include "../../lib/src/representations/markdown/markdown_repository_configuration_representation.h"

#include "main_window_view.h"
#include "main_menu_presenter.h"

#include "gear/async_task_notifications_distributor.h"
#include "cli_n_breadcrumbs_presenter.h"
#include "orloj_presenter.h"
#include "status_bar_presenter.h"

#include "dialogs/add_library_dialog.h"
#include "dialogs/sync_library_dialog.h"
#include "dialogs/rm_library_dialog.h"
#include "dialogs/run_tool_dialog.h"
#include "dialogs/wingman_dialog.h"
#include "dialogs/organizer_new_dialog.h"
#include "dialogs/outline_new_dialog.h"
#include "dialogs/note_new_dialog.h"
#include "dialogs/fts_dialog_presenter.h"
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
#include "dialogs/terminal_dialog.h"
#include "dialogs/export_csv_file_dialog.h"
#include "dialogs/export_file_dialog.h"

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
class FtsDialog;
class FtsDialogPresenter;

/**
 * @brief MindForger main window presenter.
 *
 * Main window presenter:
 *
 * * Provides index of all UI presenters.
 * * Hosts cross component signals definitions.
 * * Implements core UI application logic for other presenters and views.
 *
 * This class is used to demonstrate MindForger's MVP frontend architecture
 * and code conventions.
 *
 */
// TODO rename to AppWindowPresenter
class MainWindowPresenter : public QObject
{
    Q_OBJECT

    static QString NEW_MD_FILE_TITLE;
    static QString NEW_MD_FILE_EXTENSION;
    static QString EXPORT_O_TO_HTML_TITLE;
    static QString EXPORT_O_TO_HTML_EXTENSION;

private:
    MainWindowView& view;

    Configuration& config;
    Mind* mind;

    AsyncTaskNotificationsDistributor* distributor;

    MarkdownOutlineRepresentation* mdRepresentation;
    HtmlOutlineRepresentation* htmlRepresentation;
    MarkdownConfigurationRepresentation* mdConfigRepresentation;
    MarkdownRepositoryConfigurationRepresentation* mdRepositoryConfigRepresentation;
    MarkdownDocumentRepresentation* mdDocumentRepresentation;

    MainMenuPresenter* mainMenu;
    CliAndBreadcrumbsPresenter* cli;
    OrlojPresenter* orloj;
    StatusBarPresenter* statusBar;

    AddLibraryDialog* newLibraryDialog;
    SyncLibraryDialog* syncLibraryDialog;
    RemoveLibraryDialog* rmLibraryDialog;
    RunToolDialog* runToolDialog;
    WingmanDialog* wingmanDialog;
    ScopeDialog* scopeDialog;
    OrganizerNewDialog* newOrganizerDialog;
    OutlineNewDialog* newOutlineDialog;
    NoteNewDialog* newNoteDialog;
    FtsDialog* ftsDialog;
    FtsDialogPresenter* ftsDialogPresenter;
    FindOutlineByNameDialog* findOutlineByNameDialog;
    FindOutlineByNameDialog* findThingByNameDialog;
    FindNoteByNameDialog* findNoteByNameDialog;
    FindOutlineByTagDialog* findOutlineByTagDialog;
    FindNoteByTagDialog* findNoteByTagDialog;
    RefactorNoteToOutlineDialog* refactorNoteToOutlineDialog;
    ConfigurationDialog* configDialog;
    TerminalDialog* terminalDialog;
    InsertImageDialog* insertImageDialog;
    InsertLinkDialog* insertLinkDialog;
    RowsAndDepthDialog* rowsAndDepthDialog;
    NewRepositoryDialog* newRepositoryDialog;
    NewFileDialog* newFileDialog;
    ExportFileDialog* exportOutlineToHtmlDialog;
    ExportCsvFileDialog* exportMemoryToCsvDialog;

public:
    explicit MainWindowPresenter(MainWindowView& view);
    MainWindowPresenter(const MainWindowPresenter&) = delete;
    MainWindowPresenter(const MainWindowPresenter&&) = delete;
    MainWindowPresenter& operator =(const MainWindowPresenter&) = delete;
    MainWindowPresenter& operator =(const MainWindowPresenter&&) = delete;
    ~MainWindowPresenter();

    // this presenter view
    MainWindowView& getView() const { return view; }
    const Configuration& getConfiguration() const { return config; }
    MarkdownOutlineRepresentation* getMarkdownRepresentation() const { return mdRepresentation; }
    MarkdownConfigurationRepresentation* getConfigRepresentation() const { return mdConfigRepresentation; }
    HtmlOutlineRepresentation* getHtmlRepresentation() const { return htmlRepresentation; }
    AsyncTaskNotificationsDistributor* getDistributor() const { return distributor; }

    // presenters
    CliAndBreadcrumbsPresenter* getCli() { return cli; }
    OrlojPresenter* getOrloj() const { return orloj; }
    MainMenuPresenter* getMainMenu() const { return mainMenu; }
    StatusBarPresenter* getStatusBar() const { return statusBar; }

    // function
    Mind* getMind() const { return mind; }

    // hey hello!
    void showInitialView();

    // N view
    void handleNoteViewLinkClicked(const QUrl& url);

public slots:
    // mind
#ifdef DO_MF_DEBUG
    void doActionMindHack();
#endif
    // wingman: dialog
    void handleActionWingman(bool showDialog=true);
    void slotRunWingmanFromDialog(bool showDialog=false);
    void slotWingmanAppendFromDialog();
    void slotWingmanReplaceFromDialog();
    // workspace
    void doActionMindNewRepository();
    void handleMindNewRepository();
    void doActionMindNewFile();
    void handleMindNewFile();
    void doActionMindThink();
    void doActionMindSleep();
    void doActionMindToggleThink();
    void doActionMindToggleAutolink();
    void doActionMindLearnRepository();
    void doActionMindLearnFile();
    void doActionMindRelearn(QString path);
    void doActionMindTimeTagScope();
    void handleMindScope();
    void doActionMindPreferences();
    void handleMindPreferences();
    void doActionMindRemember();
    void doActionMindSnapshot();
    void doActionMindCsvExport();
    void handleMindCsvExport();
    void doActionExit();
    // recall
    void doActionFts();
    void doFts(const QString& pattern, bool doSearch=false);
    void doActionFindOutlineByName(const std::string& phrase="");
    void handleFindOutlineByName();
    void handleFindThingByName();
    void doActionFindNoteByName();
    void handleFindNoteByName();
    void doActionFindOutlineByTag(const std::string& tag="");
    void handleFindOutlineByTag();
    void doActionFindNoteByTag();
    void doTriggerFindNoteByTag(const m8r::Tag* tag);
    void doSwitchFindByTagDialog(bool toFindNotesByTag);
    void handleFindNoteByTag();
    // view
    void sortAndSaveOrganizersConfig();
    void doActionViewOrganizers();
    void doActionViewOrganizer();
    void doActionViewTagCloud();
    bool doActionViewHome();
    void doActionViewOutlines();
    void doActionViewOutlinesMap();
    void doActionViewRecentNotes();
    void doActionViewKnowledgeGraphNavigator();
    void doActionCli();
    void doActionViewTerminal();
    void doActionViewLimbo();
    void doActionViewDistractionFree();
    void doActionViewFullscreen();
    // library
    void doActionLibraryNew();
    void handleNewLibrary();
    void doActionLibrarySync();
    void doActionLibraryOrphans();
    void handleSyncLibrary();
    void doActionLibraryRm();
    void handleRmLibrary();
    // organizer
    void doActionOrganizerNew();
    void handleCreateOrganizer();
    void doActionOrganizerEdit();
    void doActionOrganizerClone();
    void doActionOrganizerFocusToNextVisibleQuadrant();
    void doActionOrganizerFocusToPreviousVisibleQuadrant();
    void doActionOrganizerMoveNoteToNextVisibleQuadrant(Note* note);
    void doActionOrganizerMoveNoteToPreviousVisibleQuadrant(Note* note);
    void doActionOrganizerForget();
    // navigator
    void doActionNavigatorShuffle();
    // format
    void doActionFormatBold();
    void doActionFormatItalic();
    void doActionFormatCode();
    void doActionFormatComment();
    void doActionFormatMath();
    void doActionFormatMathFrac();
    void doActionFormatMathSum();
    void doActionFormatMathInt();
    void doActionFormatMathIiint();
    void doActionFormatMathAlpha();
    void doActionFormatMathBeta();
    void doActionFormatMathDelta();
    void doActionFormatMathGama();
    void doActionFormatMathText();
    void doActionFormatMathBar();
    void doActionFormatMathHat();
    void doActionFormatMathDot();
    void doActionFormatMathOverrightarrow();
    void doActionFormatMathCup();
    void doActionFormatMathCap();
    void doActionFormatMathEmptyset();
    void doActionFormatMathIn();
    void doActionFormatMathNotin();
    void doActionFormatMathSqrt();
    void doActionFormatStrikethrough();
    void doActionFormatKeyboard();
    void handleRowsAndDepth();
    void doActionFormatListBullet();
    void doActionFormatListNumber();
    void doActionFormatListTask();
    void doActionFormatListTaskItem();
    void doActionFormatToc(bool withTags);
    void doActionFormatTocWithTags();
    void doActionFormatTocWithoutTags();
    void doActionFormatTimestamp();
    void doActionFormatCodeBlock();
    void doActionFormatMathBlock();
    void doActionFormatDiagramBlock();
    void doActionFormatDiagramPie();
    void doActionFormatDiagramFlow();
    void doActionFormatDiagramClass();
    void doActionFormatDiagramState();
    void doActionFormatDiagramGantt();
    void doActionFormatDiagramSequence();
    void doActionFormatBlockquote();
    void doActionFormatLinkOrImage(QString link);
    void doActionFormatLink();
    void handleFormatLink();
    void doActionFormatImage();
    void injectImageLinkToEditor(const QString& path, const QString& alternateText);
    void handleFormatImage();
    void doActionFormatTable();
    void doActionFormatHr();
    // Outline
    void doActionOutlineNew();
    void doActionOutlineOrNoteNew();
    void doActionOutlineEdit();
    void handleOutlineNew();
    void doActionOutlineClone();
    void doActionOutlineHome();
    void doActionOutlineForget();
    void doActionOutlineHtmlExport();
    void handleOutlineHtmlExport();
    void doActionOutlineTWikiImport();
    bool checkWingmanAvailability();
    void handleWingmanMenuAction(const std::string& prompt);
    void doActionWingmanOSummarize() { handleWingmanMenuAction(PROMPT_SUMMARIZE); }
    void doActionWingmanOExplain() { handleWingmanMenuAction(PROMPT_EXPLAIN_LIKE_5); }
    void doActionWingmanOFind() { handleWingmanMenuAction(PROMPT_FIND_TASKS); }

    void doActionWingmanNSummarize() { handleWingmanMenuAction(PROMPT_SUMMARIZE); }
    void doActionWingmanNFixGrammar() { handleWingmanMenuAction(PROMPT_FIND_GRAMMAR); }
    void doActionWingmanNRewrite() { handleWingmanMenuAction(PROMPT_TRANSLATE_EN); }

    void doActionWingmanEFixGrammar() { handleWingmanMenuAction(PROMPT_FIX_GRAMMAR); }
    void doActionWingmanEExplain() { handleWingmanMenuAction(PROMPT_EXPLAIN_LIKE_5_TXT); }
    void doActionWingmanEFinishText() { handleWingmanMenuAction(PROMPT_COMPLETE_TEXT); }
    void doActionWingmanERewriteText() { handleWingmanMenuAction(PROMPT_REWRITE_FORMALLY); }

    // Note
    void doActionNoteNew();
    bool withWriteableOutline(const std::string& outlineKey);
    void handleNoteNew();
    void doActionNoteHoist();
    void doActionOutlineOrNoteEdit();
    void doActionOutlineShow();
    void doActionNoteEdit();
    void doActionNoteExternalEdit();
    void selectNoteInOutlineTree(Note* note, Outline::Patch& patch, bool onUp);
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
    void doActionNoteLeave();
    void doActionNoteForget();
    // edit
    void doActionEditFind();
    void doActionEditFindAgain();
    void doActionEditWordWrapToggle();
    void doActionEditPasteImageData(QImage image);
    void doActionRunToolDialogAnywhere();
    void doActionOpenRunToolDialog(QString& phrase, QString& toolId, bool showDialog=true);
    void handleRunTool();
    void doActionToggleLiveNotePreview();
    void doActionNameDescFocusSwap();
    void doActionSpellCheck();
    // help
    void doActionHelpDocumentation();
    void doActionHelpWeb();
    void doActionHelpMarkdown();
    void doActionHelpMathLivePreview();
    void doActionHelpMathQuickReference();
    void doActionHelpDiagrams();
    void doActionHelpReportBug();
    void doActionHelpCheckForUpdates();
    void doActionEmojisDialog();
    void doActionHelpAboutMindForger();

    void slotHandleFts();
    void slotMainToolbarVisibilityChanged(bool visibility);

private:
    void injectMarkdownText(const QString& text, bool newline=false, int offset=0);
    void injectDiagramBlock(const QString& diagramText);
    void copyLinkOrImageToRepository(const std::string& srcPath, QString& path);

    void statusInfoPreviewFlickering();
};

}

#endif // M8RUI_MAIN_WINDOW_PRESENTER_H
