/*
 main_window_presenter.cpp     MindForger thinking notebook

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
#include "main_window_presenter.h"

#include <QShortcut>

#include "kanban_column_presenter.h"

using namespace std;
using namespace m8r::filesystem;

namespace m8r {

MainWindowPresenter::MainWindowPresenter(MainWindowView& view)
    : view(view),
      config(Configuration::getInstance())
{
    mind = new Mind{config};

    // representations
    this->htmlRepresentation
        = mind->getHtmlRepresentation();
    this->mdRepresentation
        = &htmlRepresentation->getMarkdownRepresentation();
    this->mdConfigRepresentation
        = new MarkdownConfigurationRepresentation{};
    this->mdRepositoryConfigRepresentation
        = new MarkdownRepositoryConfigurationRepresentation{};
    this->mdDocumentRepresentation
        = new MarkdownDocumentRepresentation{mind->getOntology()};

    // assemble presenters w/ UI
    statusBar = new StatusBarPresenter{view.getStatusBar(), mind};
    mainMenu = new MainMenuPresenter{
        this}; view.getOrloj()->setMainMenu(mainMenu->getView());
    cli = new CliAndBreadcrumbsPresenter{this, view.getCli(), mind};
    orloj = new OrlojPresenter{this, view.getOrloj(), mind};

    // initialize components
    newLibraryDialog = new AddLibraryDialog{&view};
    syncLibraryDialog = new SyncLibraryDialog{&view};
    rmLibraryDialog = new RemoveLibraryDialog(&view);
    runToolDialog = new RunToolDialog{&view};
    wingmanDialog = new WingmanDialog{&view};
    scopeDialog = new ScopeDialog{mind->getOntology(), &view};
    newOrganizerDialog = new OrganizerNewDialog{mind->getOntology(), &view};
    newOutlineDialog = new OutlineNewDialog{
        QString::fromStdString(config.getMemoryPath()), mind->getOntology(), &view};
    newNoteDialog = new NoteNewDialog{mind->remind().getOntology(), &view};
    ftsDialog = new FtsDialog{&view};
    ftsDialogPresenter = new FtsDialogPresenter(ftsDialog, mind, orloj);
    findOutlineByNameDialog = new FindOutlineByNameDialog{&view};
    findThingByNameDialog = new FindOutlineByNameDialog{&view};
    findNoteByNameDialog = new FindNoteByNameDialog{&view};
    findOutlineByTagDialog = new FindOutlineByTagDialog{
        mind->remind().getOntology(), &view};
    findNoteByTagDialog = new FindNoteByTagDialog{mind->remind().getOntology(), &view};
    refactorNoteToOutlineDialog = new RefactorNoteToOutlineDialog{&view};
    configDialog = new ConfigurationDialog{&view};
    terminalDialog = new TerminalDialog{&view};
    insertImageDialog = new InsertImageDialog{&view};
    insertLinkDialog = new InsertLinkDialog{&view};
    rowsAndDepthDialog = new RowsAndDepthDialog(&view);
    newRepositoryDialog = new NewRepositoryDialog(&view);
    newFileDialog = new NewFileDialog(&view);
    exportOutlineToHtmlDialog
       = new ExportFileDialog(
             tr("Export Notebook to HTML"),
             tr("Export"),
             QString::fromStdString(File::EXTENSION_HTML),
             &view
    );
    exportMemoryToCsvDialog
       = new ExportCsvFileDialog(
             tr("Export Memory to CSV"),
             tr("Export"),
             QString::fromStdString(File::EXTENSION_CSV),
             &view
    );
#ifdef MF_NER
    nerChooseTagsDialog = new NerChooseTagTypesDialog(&view);
    nerResultDialog = new NerResultDialog(&view);
#endif
    // show/hide widgets based on configuration
    handleMindPreferences();

    // wire signals
    QObject::connect(
        wingmanDialog->getRunButton(), SIGNAL(clicked()),
        this, SLOT(handleActionWingman()));
    QObject::connect(
        runToolDialog->getRunButton(), SIGNAL(clicked()),
        this, SLOT(handleRunTool()));
    QObject::connect(
        newLibraryDialog->getCreateButton(), SIGNAL(clicked()),
        this, SLOT(handleNewLibrary()));
    QObject::connect(
        syncLibraryDialog->getSyncButton(), SIGNAL(clicked()),
        this, SLOT(handleSyncLibrary()));
    QObject::connect(
        rmLibraryDialog->getRemoveButton(), SIGNAL(clicked()),
        this, SLOT(handleRmLibrary()));
    QObject::connect(
        scopeDialog->getSetButton(), SIGNAL(clicked()), this, SLOT(handleMindScope()));
    QObject::connect(
        newOutlineDialog, SIGNAL(accepted()), this, SLOT(handleOutlineNew()));
    QObject::connect(
        newNoteDialog, SIGNAL(accepted()), this, SLOT(handleNoteNew()));
    QObject::connect(
        findOutlineByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindOutlineByName()));
    QObject::connect(
        findThingByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindThingByName()));
    QObject::connect(
        findNoteByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindNoteByName()));
    QObject::connect(
        findOutlineByTagDialog, SIGNAL(searchFinished()), this, SLOT(handleFindOutlineByTag()));
    QObject::connect(
        findOutlineByTagDialog, SIGNAL(switchDialogs(bool)), this, SLOT(doSwitchFindByTagDialog(bool)));
    QObject::connect(
        findNoteByTagDialog, SIGNAL(searchFinished()), this, SLOT(handleFindNoteByTag()));
    QObject::connect(
        findNoteByTagDialog, SIGNAL(switchDialogs(bool)), this, SLOT(doSwitchFindByTagDialog(bool)));
    QObject::connect(
        newOrganizerDialog, SIGNAL(createFinished()), this, SLOT(handleCreateOrganizer()));
    QObject::connect(
        refactorNoteToOutlineDialog, SIGNAL(searchFinished()), this, SLOT(handleRefactorNoteToOutline()));
    QObject::connect(
        insertImageDialog->getInsertButton(), SIGNAL(clicked()), this, SLOT(handleFormatImage()));
    QObject::connect(
        insertLinkDialog->getInsertButton(), SIGNAL(clicked()), this, SLOT(handleFormatLink()));
    QObject::connect(
        rowsAndDepthDialog->getGenerateButton(), SIGNAL(clicked()), this, SLOT(handleRowsAndDepth()));
    QObject::connect(
        newRepositoryDialog->getNewButton(), SIGNAL(clicked()), this, SLOT(handleMindNewRepository()));
    QObject::connect(
        newFileDialog->getNewButton(), SIGNAL(clicked()), this, SLOT(handleMindNewFile()));
    QObject::connect(
        exportOutlineToHtmlDialog->getNewButton(), SIGNAL(clicked()), this, SLOT(handleOutlineHtmlExport()));
    QObject::connect(
        exportMemoryToCsvDialog->getNewButton(), SIGNAL(clicked()), this, SLOT(handleMindCsvExport()));
    QObject::connect(
        orloj->getDashboard()->getView()->getNavigatorDashboardlet(), SIGNAL(clickToSwitchFacet()),
        this, SLOT(doActionViewKnowledgeGraphNavigator())
    );
    QObject::connect(
        orloj->getNoteEdit()->getView()->getNoteEditor(), SIGNAL(signalDnDropUrl(QString)),
        this, SLOT(doActionFormatLinkOrImage(QString))
    );
    QObject::connect(
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor(), SIGNAL(signalDnDropUrl(QString)),
        this, SLOT(doActionFormatLinkOrImage(QString))
    );
    QObject::connect(
        orloj->getNoteEdit()->getView()->getNoteEditor(), SIGNAL(signalPasteImageData(QImage)),
        this, SLOT(doActionEditPasteImageData(QImage))
    );
    QObject::connect(
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor(), SIGNAL(signalPasteImageData(QImage)),
        this, SLOT(doActionEditPasteImageData(QImage))
    );
    // wire LEFT toolbar signals
    /*
    QObject::connect(
        new QShortcut(QKeySequence("Alt+1"), view.getOrloj()), SIGNAL(activated()),
        this, SLOT(doActionArxivToolbar())
    );
    QObject::connect(
        view.getLeftToolBar()->actionLeftToolbarArxiv, SIGNAL(triggered()),
        this, SLOT(doActionArxivToolbar())
    );
    QObject::connect(
        new QShortcut(QKeySequence("Alt+2"), view.getOrloj()), SIGNAL(activated()),
        this, SLOT(doActionWikipediaToolbar())
    );
    QObject::connect(
        view.getLeftToolBar()->actionLeftToolbarWikipedia, SIGNAL(triggered()),
        this, SLOT(doActionWikipediaToolbar())
    );
    QObject::connect(
        new QShortcut(QKeySequence("Alt+3"), view.getOrloj()), SIGNAL(activated()),
        this, SLOT(doActionStackOverflowToolbar())
    );
    QObject::connect(
        view.getLeftToolBar()->actionLeftToolbarStackOverflow, SIGNAL(triggered()),
        this, SLOT(doActionStackOverflowToolbar())
    );
    QObject::connect(
        new QShortcut(QKeySequence("Alt+5"), view.getOrloj()), SIGNAL(activated()),
        this, SLOT(doActionDuckDuckGoToolbar())
    );
    QObject::connect(
        view.getLeftToolBar()->actionLeftToolbarDuckDuckGo, SIGNAL(triggered()),
        this, SLOT(doActionDuckDuckGoToolbar())
    );
    QObject::connect(
        new QShortcut(QKeySequence("Alt+6"), view.getOrloj()), SIGNAL(activated()),
        this, SLOT(doActionGitHubToolbar())
    );
    QObject::connect(
        view.getLeftToolBar()->actionLeftToolbarGitHub, SIGNAL(triggered()),
        this, SLOT(doActionGitHubToolbar())
    );
    QObject::connect(
        new QShortcut(QKeySequence("Alt+7"), view.getOrloj()), SIGNAL(activated()),
        this, SLOT(doActionBardToolbar())
    );
    QObject::connect(
        view.getLeftToolBar()->actionLeftToolbarBard, SIGNAL(triggered()),
        this, SLOT(doActionBardToolbar())
    );
    */
    // wire TOP toolbar signals
    QObject::connect(
        view.getToolBar()->actionNewOutlineOrNote, SIGNAL(triggered()),
        this, SLOT(doActionOutlineOrNoteNew())
    );
    QObject::connect(
        view.getToolBar()->actionOpenRepository, SIGNAL(triggered()),
        this, SLOT(doActionMindLearnRepository())
    );
    QObject::connect(view.getToolBar()->actionOpenFile, SIGNAL(triggered()), this, SLOT(doActionMindLearnFile()));
#ifdef MF_DEPRECATED
    QObject::connect(view.getToolBar()->actionViewDashboard, SIGNAL(triggered()), this, SLOT(doActionViewDashboard()));
#endif
#ifdef ONE_ORGANIZER
    QObject::connect(view.getToolBar()->actionViewEisenhower, SIGNAL(triggered()), this, SLOT(doActionViewOrganizer()));
#else
    QObject::connect(view.getToolBar()->actionViewOrganizers, SIGNAL(triggered()), this, SLOT(doActionViewOrganizers()));
#endif
    QObject::connect(view.getToolBar()->actionViewOutlines, SIGNAL(triggered()), this, SLOT(doActionViewOutlines()));
    QObject::connect(view.getToolBar()->actionViewNavigator, SIGNAL(triggered()), this, SLOT(doActionViewKnowledgeGraphNavigator()));
    QObject::connect(view.getToolBar()->actionViewTags, SIGNAL(triggered()), this, SLOT(doActionViewTagCloud()));
    QObject::connect(view.getToolBar()->actionViewRecentNotes, SIGNAL(triggered()), this, SLOT(doActionViewRecentNotes()));
    QObject::connect(view.getToolBar()->actionHomeOutline, SIGNAL(triggered()), this, SLOT(doActionViewHome()));
    QObject::connect(view.getToolBar()->actionThink, SIGNAL(triggered()), this, SLOT(doActionMindToggleThink()));
    QObject::connect(view.getToolBar()->actionScope, SIGNAL(triggered()), this, SLOT(doActionMindTimeTagScope()));
    QObject::connect(view.getToolBar()->actionAdapt, SIGNAL(triggered()), this, SLOT(doActionMindPreferences()));
    QObject::connect(
        view.getToolBar()->actionHelp, SIGNAL(triggered()),
        this, SLOT(doActionHelpDocumentation())
    );
    QObject::connect(
        view.getToolBar(), SIGNAL(signalMainToolbarVisibilityChanged(bool)),
        this, SLOT(slotMainToolbarVisibilityChanged(bool))
    );


#ifdef MF_NER
    QObject::connect(nerChooseTagsDialog->getChooseButton(), SIGNAL(clicked()), this, SLOT(handleFindNerEntities()));
    QObject::connect(nerResultDialog, SIGNAL(choiceFinished()), this, SLOT(handleFtsNerEntity()));
#endif

    // async task 2 GUI events distributor
    distributor = new AsyncTaskNotificationsDistributor(this);
    // setup callback for cleanup when it finishes
    QObject::connect(distributor, SIGNAL(finished()), distributor, SLOT(deleteLater()));
    distributor->start();
#ifdef MF_NER
    // NER worker
    nerWorker = nullptr;
#endif

    // send signal to components to be updated on a configuration change
    QObject::connect(configDialog, SIGNAL(saveConfigSignal()), this, SLOT(handleMindPreferences()));
    QObject::connect(configDialog, SIGNAL(saveConfigSignal()), orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor(), SLOT(slotConfigurationUpdated()));
    QObject::connect(configDialog, SIGNAL(saveConfigSignal()), orloj->getNoteEdit()->getView()->getNoteEditor(), SLOT(slotConfigurationUpdated()));
    QObject::connect(configDialog, SIGNAL(saveConfigSignal()), distributor, SLOT(slotConfigurationUpdated()));

    // let Mind to learn active repository & preserve desired state
    mind->learn();
}

MainWindowPresenter::~MainWindowPresenter()
{
    if(mind) delete mind;
    if(mainMenu) delete mainMenu;
    if(statusBar) delete statusBar;
    if(newOutlineDialog) delete newOutlineDialog;
    if(ftsDialogPresenter) delete ftsDialogPresenter;
    if(ftsDialog) delete ftsDialog;
    if(findOutlineByNameDialog) delete findOutlineByNameDialog;
    if(findThingByNameDialog) delete findThingByNameDialog;
    if(findNoteByNameDialog) delete findNoteByNameDialog;
    if(findOutlineByTagDialog) delete findOutlineByTagDialog;
    if(configDialog) delete configDialog;
    //if(findNoteByNameDialog) delete findNoteByNameDialog;
    if(insertImageDialog) delete insertImageDialog;
    if(newLibraryDialog) delete newLibraryDialog;

    // TODO deletes
    delete this->mdConfigRepresentation;
    delete this->mdRepositoryConfigRepresentation;
    delete this->mdDocumentRepresentation;
}

/**
 * @brief Initial view assembly.
 *
 * This method builds initial view on MindForger boot.
 */
void MainWindowPresenter::showInitialView()
{
    MF_DEBUG("Initial view to show " << mind->getOutlines().size() << " Os (scope is applied if active)" << endl);

    static vector<Note*> emptyVector{};

    // UI
    if(mind->getOutlines().size()) {
        if(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY) {
            if(config.getActiveRepository()->isGithubRepository()) {
                string key{config.getActiveRepository()->getDir()};
                key += FILE_PATH_SEPARATOR;
                key += "README.md";
                Outline* o = mind->remind().getOutline(key);
                if(o) {
                    orloj->showFacetOutline(o);
                } else {
                    orloj->showFacetOutlineList(mind->getOutlines());
                }
            } else if(config.getActiveRepository()->getType()==Repository::RepositoryType::MINDFORGER) {
                if(!string{START_TO_DASHBOARD}.compare(config.getStartupView())) {
                    orloj->showFacetDashboard();
                } else if(!string{START_TO_OUTLINES}.compare(config.getStartupView())) {
                    orloj->showFacetOutlineList(mind->getOutlines());
                } else if(!string{START_TO_OUTLINES_TREE}.compare(config.getStartupView())) {
                    orloj->showFacetOutlinesMap(mind->outlinesMapGet());
                } else if(!string{START_TO_TAGS}.compare(config.getStartupView())) {
                    orloj->showFacetTagCloud();
                } else if(!string{START_TO_RECENT}.compare(config.getStartupView())) {
                    vector<Note*> notes{};
                    orloj->showFacetRecentNotes(mind->getAllNotes(notes));
                } else if(!string{START_TO_EISENHOWER_MATRIX}.compare(config.getStartupView())) {
                    orloj->showFacetEisenhowerMatrix(
                         nullptr,
                         emptyVector,
                         mind->getOutlines(),
                         emptyVector
                    );
                } else if(!string{START_TO_HOME_OUTLINE}.compare(config.getStartupView())) {
                    if(!doActionViewHome()) {
                        // fallback
                        orloj->showFacetOutlineList(mind->getOutlines());
                    }
                } else {
                    orloj->showFacetOutlineList(mind->getOutlines());
                }
            } else {
                view.getCli()->setBreadcrumbPath("/outlines");
                orloj->showFacetOutlineList(mind->getOutlines());
            }
        } else { // file
            // IMPROVE move this method to breadcrumps
            QString m{"/outlines/"};
            m += QString::fromStdString((*mind->getOutlines().begin())->getName());
            view.getCli()->setBreadcrumbPath(m);

            orloj->showFacetOutline(*mind->getOutlines().begin());
        }
    } else {
        // NO Os > nothing to show
        // IMPROVE show homepage once it's implemented
        mind->amnesia();
        orloj->showFacetOutlineList(mind->getOutlines());
    }

    view.setFileOrDirectory(
        QString::fromStdString(config.getActiveRepository()->getPath())
    );

    // config > menu
    mainMenu->showFacetMindAutolink(config.isAutolinking());
    mainMenu->showFacetLiveNotePreview(config.isUiLiveNotePreview());
    orloj->setAspect(
        config.isUiLiveNotePreview()
        ?OrlojPresenterFacetAspect::ASPECT_LIVE_PREVIEW
        :OrlojPresenterFacetAspect::ASPECT_NONE
    );

    // move Mind to configured state
    if(config.getDesiredMindState()==Configuration::MindState::THINKING) {
        MF_DEBUG("InitialView: asking Mind to THINK..." << endl);
        shared_future<bool> f = mind->think(); // move
        if(f.wait_for(chrono::microseconds(0)) == future_status::ready) {
            if(!f.get()) {
                mainMenu->showFacetMindSleep();
                statusBar->showError(
                    tr(
                        "Cannot think - either Mind already dreaming or "
                        "workspace too big"
                    )
                );
            }
            statusBar->showMindStatistics();
        } else {
            statusBar->showMindStatistics();
            // ask notifications distributor to repaint status bar later
            AsyncTaskNotificationsDistributor::Task* task
                = new AsyncTaskNotificationsDistributor::Task{f,AsyncTaskNotificationsDistributor::TaskType::DREAM_TO_THINK};
            distributor->add(task);
        }
    }
}

/* Link handling hints.
 *
 * PROBLEM:
 *
 *   QWebView RESOLVES clicked link and then delegates it's handling to this
 *   method. The problem is that this handler does NOT get original link, but
 *   RESOLVED link - which might be resolved differently than I expected.
 *     For link resolution is IMPORTANT baseUrl specified within HTML source
 *   passed to QWebView for rendering.
 *
 * Input:
 *
 *   Qt URL    ... URL resolved by QWebView using a.href and html@baseUrl
 *   Current O ... link clicked in description of a N from O
 *   Current N ... link clicked in description of a N from O
 *
 * Outline link types:
 *
 *   ABSOLUTE link
 *     - a.href: /home/user/mf/memory/d/o.md
 *     - Qt URL: file:///home/user/mf/memory/d/o.md
 *   RELATIVE link SAME directory:
 *     - a.href: o.md
 *     - Qt URL: file:///home/user/mf/memory/d/o.md
 *               HTML.baseUrl + a.href
 *   RELATIVE link DIFFERENT directory:
 *     - a.href: ../d/o.md
 *     - Qt URL: file:///home/user/mf/memory/d/o.md
 *               HTML.baseUrl + a.href
 *
 * Note link types
 *
 *   RELATIVE LINKS:
 *     - a.href: #mangled-note-name
 *     - Qt URL:
 *   ... and all O links above w/ #mangled-note-name suffix
 *
 */
void MainWindowPresenter::handleNoteViewLinkClicked(const QUrl& url)
{
#ifdef DO_MF_DEBUG
    MF_DEBUG("HTML clickHandler: " << endl);
    MF_DEBUG("  Qt URL     : " << url.toString().toStdString() << endl);
    MF_DEBUG("  Memory path: " << config.getMemoryPath() << endl);
    MF_DEBUG("  Current O  : " << orloj->getOutlineView()->getCurrentOutline()->getKey() << endl);
#endif

    statusBar->showInfo(QString(tr("Hyperlink %1 clicked...")).arg(url.toString()));
    Outline* currentOutline = orloj->getOutlineView()->getCurrentOutline();
    if(url.toString().size()) {
        if(url.toString().startsWith(QString::fromStdString(AutolinkingPreprocessor::MF_URL_PREFIX))) {
            MF_DEBUG("  URL type   : MindForger" << endl);
            findThingByNameDialog->setWindowTitle(tr("Autolinked Notebooks and Notes"));
            findThingByNameDialog->getKeywordsCheckbox()->setChecked(false);
            findThingByNameDialog->setSearchedString(
                QString::fromStdString(url.toString().toStdString().substr(AutolinkingPreprocessor::MF_URL_PREFIX.size())));

            vector<Thing*> allThings{};
            vector<string>* thingsNames = new vector<string>{};
            mind->getAllThings(allThings, thingsNames);

            findThingByNameDialog->show(allThings, thingsNames, false, false);
            return;
        }

        if(url.toString().startsWith(QString::fromStdString(AutolinkingPreprocessor::FILE_URL_PROTOCOL))) {
            string key{url.toString().toStdString()};
#if defined(WIN32) || defined(WIN64)
            key.erase(0,8); // remove file prefix
            std::replace(key.begin(), key.end(), '/', '\\');
#else
            key.erase(0,7); // remove file prefix
#endif
            size_t offset;
            if((offset = key.find("#")) != string::npos) {
                // it CAN be Note

                // HANDLE relative N link: #mangled-section-name
                string currentDir{}, currentFile{};
                pathToDirectoryAndFile(currentOutline->getKey(), currentDir, currentFile);
                string relativeLinkPrefix{currentDir};
                relativeLinkPrefix.append(FILE_PATH_SEPARATOR);
                relativeLinkPrefix.append("#");
                MF_DEBUG("  Relative prefix: " << relativeLinkPrefix << endl);
                if(stringStartsWith(key, relativeLinkPrefix)) {
                    // it's a relative link within current O
                    string mangledNoteName = key.substr(offset+1);
                    MF_DEBUG("HTML clickHandler - N lookup using: " << mangledNoteName << endl);
                    Outline* o=orloj->getMind()->remind().getOutline(orloj->getOutlineView()->getCurrentOutline()->getKey());
                    if(o) {
                        Note* n = o->getNoteByMangledName(mangledNoteName);
                        if(n) {
                            if(orloj->avoidDataLossOnLinkClick()) {
                                return;
                            }
                            orloj->showFacetNoteView(n);
                            return;
                        }
                    }
                    // if N not found, then link is broken - do nothing
                    statusBar->showInfo(QString(tr("Link target not found for relative link %1")).arg(QString::fromStdString(mangledNoteName)));
                    return;
                } else {
                    // HANDLE O#N link - O can be in a memory SUBDIRECTORY
                    string mangledNoteName = key.substr(offset+1);
                    key.erase(offset);
                    MF_DEBUG("HTML clickHandler - O lookup using key: " << key << endl);

                    // IMPROVE find note within outline
                    Outline* o=orloj->getMind()->remind().getOutline(key);
                    if(o) {
                        Note* n = o->getNoteByMangledName(mangledNoteName);
                        if(n) {
                            if(orloj->avoidDataLossOnLinkClick()) {
                                return;
                            }
                            orloj->showFacetNoteView(n);
                            return;
                        } else {
                            if(orloj->avoidDataLossOnLinkClick()) {
                                return;
                            }
                            // fallback to Notebook for hyperlink found
                            orloj->showFacetOutline(o);
                            return;
                        }
                    } // else fallback to open using desktop services
                }
            } else {
                // it CAN be Outline

                // QWebView resolves URL (it is NEVER relative) - use resolved URL as is
                MF_DEBUG("  O lookup using path: " << key << std::endl);
                Outline* o=orloj->getMind()->remind().getOutline(key);
                if(o) {
                    if(orloj->avoidDataLossOnLinkClick()) {
                        return;
                    }
                    orloj->showFacetOutline(o);
                    return;
                } // else fallback to open using desktop services
            }

            // IMPROVE let Qt to open also directories and external files
            MF_DEBUG("Unable to find Notebook/Note for hyperlink: " << url.toString().toStdString() << " > delegating to OS" << std::endl);
            if(!QDesktopServices::openUrl(url)) {
                MF_DEBUG("FAILED to open hyperlink: " << url.toString().toStdString() << std::endl);
            }
        } else {
            if(orloj->avoidDataLossOnLinkClick()) {
                return;
            }
            // launch URL in browser
            QDesktopServices::openUrl(url);
        }
    }
}

#ifdef DO_MF_DEBUG
void MainWindowPresenter::doActionMindHack()
{
    MF_DEBUG("MindHack" << endl);
}
#endif

void MainWindowPresenter::doActionMindNewRepository()
{
    newRepositoryDialog->show();
}

void MainWindowPresenter::handleMindNewRepository()
{
    // if directory exists, then fail
    if(isDirectoryOrFileExists(
        newRepositoryDialog->getRepositoryPath().toStdString().c_str())
    ) {
        QMessageBox::critical(
            &view,
            tr("New Workspace Error"),
            tr("Specified workspace path already exists!")
        );
        return;
    }

    // create repository
    if(!config.getInstaller()->createEmptyMindForgerRepository(
        newRepositoryDialog->getRepositoryPath().toStdString())
    ) {
        QMessageBox::critical(
            &view,
            tr("New Workspace Error"),
            tr("Failed to create empty workspace!")
        );
        return;
    }

    // copy doc and stencils
    if(!config.getInstaller()->initMindForgerRepository(
        newRepositoryDialog->isCopyDoc(),
        newRepositoryDialog->isCopyStencils(),
        newRepositoryDialog->getRepositoryPath().toStdString().c_str()
    )) {
        statusBar->showError(
            tr(
                "ERROR: workspace created, but attempt to copy documentation "
                "and/or stencils failed"
            )
        );
    }

    // open new repository
    doActionMindRelearn(newRepositoryDialog->getRepositoryPath());
    mainMenu->addRecentDirectoryOrFile(newRepositoryDialog->getRepositoryPath());
}

void MainWindowPresenter::doActionMindNewFile()
{
    newFileDialog->show();
}

void MainWindowPresenter::handleMindNewFile()
{
    if(isDirectoryOrFileExists(newFileDialog->getFilePath().toStdString().c_str())) {
        QMessageBox::critical(&view, tr("New Markdown File Error"), tr("Specified file path already exists!"));
        return;
    }

    // create foo file ...
    stringToFile(
        newFileDialog->getFilePath().toStdString(),
        DEFAULT_NEW_OUTLINE);

    // ... and open it
    doActionMindRelearn(newFileDialog->getFilePath());
    mainMenu->addRecentDirectoryOrFile(newFileDialog->getFilePath());
}

void MainWindowPresenter::doActionMindThink()
{
    shared_future<bool> f = mind->think(); // move
    if(f.wait_for(chrono::microseconds(0)) == future_status::ready) {
        // sync
        if(f.get()) {
            mainMenu->showFacetMindThink();
            if(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY) {
                orloj->showFacetOutlineList(mind->getOutlines());
            } else {
                if(mind->getOutlines().size()>0) {
                    orloj->showFacetOutline(*mind->getOutlines().begin());
                }
            }
            statusBar->showMindStatistics();
        } else {
            mainMenu->showFacetMindSleep();
            statusBar->showError(tr("Cannot think - either Mind already dreaming or repository too big"));
        }
    } else {
        statusBar->showMindStatistics();
        // ask notifications distributor to repaint status bar later
        AsyncTaskNotificationsDistributor::Task* task
            = new AsyncTaskNotificationsDistributor::Task{f,AsyncTaskNotificationsDistributor::TaskType::DREAM_TO_THINK};
        distributor->add(task);
    }
}

void MainWindowPresenter::doActionMindSleep()
{
    if(mind->sleep()) {
        mainMenu->showFacetMindSleep();
        statusBar->showMindStatistics();
    } else {
        statusBar->showMindStatistics();
        statusBar->showError(tr("Cannot start sleeping - please wait until dreaming finishes and then try again"));
    }

    orloj->getOutlineView()->getAssocLeaderboard()->getView()->hide();
}

void MainWindowPresenter::doActionMindToggleThink()
{
    if(config.getMindState()==Configuration::MindState::THINKING) {
        doActionMindSleep();
    } else {
        doActionMindThink();
    }
}

void MainWindowPresenter::doActionMindToggleAutolink()
{
    if(config.isAutolinking()) {
        config.setAutolinking(false);
    } else {
        config.setAutolinking(true);
    }
    mainMenu->showFacetMindAutolink(config.isAutolinking());
    mdConfigRepresentation->save(config);

    // refresh view
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE))
    {
        orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE))
    {
        orloj->showFacetNoteView(orloj->getOutlineView()->getOutlineTree()->getCurrentNote());
    }
}

void MainWindowPresenter::doActionNameDescFocusSwap()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        if(orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->hasFocus()) {
            orloj->getOutlineHeaderEdit()->getView()->focusName();
        } else {
            orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->setFocus();
        }
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        if(orloj->getNoteEdit()->getView()->getNoteEditor()->hasFocus()) {
            orloj->getNoteEdit()->getView()->focusName();
        } else {
            orloj->getNoteEdit()->getView()->getNoteEditor()->setFocus();
        }
    }
}

void MainWindowPresenter::doActionToggleLiveNotePreview()
{
    MF_DEBUG("Toggling live N preview" << endl);

    // toggle config
    if(config.isUiLiveNotePreview()) {
        config.setUiLiveNotePreview(false);
    } else {
        config.setUiLiveNotePreview(true);
    }
    mdConfigRepresentation->save(config);

    // menu
    mainMenu->showFacetLiveNotePreview(config.isUiLiveNotePreview());

    // aspect
    if(config.isUiLiveNotePreview()) {
        orloj->setAspect(OrlojPresenterFacetAspect::ASPECT_LIVE_PREVIEW);
    } else {
        orloj->setAspect(OrlojPresenterFacetAspect::ASPECT_NONE);
    }

    // view
    orloj->refreshLiveNotePreview();

    if(config.isUiLiveNotePreview()) {
        statusInfoPreviewFlickering();
    }
}

void MainWindowPresenter::doActionMindLearnRepository()
{
    QString homeDirectory
        = QStandardPaths::locate(
            QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory
        );

    QFileDialog learnDialog{&view};
    learnDialog.setWindowTitle(tr("Learn Directory or MindForger Workspace"));
    // learnDialog.setFileMode(QFileDialog::Directory|QFileDialog::ExistingFiles);
    //   not supported, therefore
    // >
    // ASK user: directory/repository or file (choice)
    // >
    // open dialog configured as required
    learnDialog.setFileMode(QFileDialog::Directory);
    learnDialog.setDirectory(homeDirectory);
    learnDialog.setViewMode(QFileDialog::Detail);

    QStringList directoryNames{};
    if(learnDialog.exec()) {
        directoryNames = learnDialog.selectedFiles();
        if(directoryNames.size()==1) {
            mainMenu->addRecentDirectoryOrFile(directoryNames[0]);
            doActionMindRelearn(directoryNames[0]);
        } // else too many files
    } // else directory closed / nothing choosen
}

void MainWindowPresenter::doActionMindLearnFile()
{
    QString homeDirectory
        = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);

    QFileDialog learnDialog{&view};
    learnDialog.setWindowTitle(tr("Learn Markdown File"));
    learnDialog.setFileMode(QFileDialog::ExistingFile);
    learnDialog.setDirectory(homeDirectory);
    learnDialog.setViewMode(QFileDialog::Detail);

    QStringList directoryNames{};
    if(learnDialog.exec()) {
        directoryNames = learnDialog.selectedFiles();
        if(directoryNames.size()==1) {
            mainMenu->addRecentDirectoryOrFile(directoryNames[0]);
            doActionMindRelearn(directoryNames[0]);
        } // else too many files
    } // else directory closed / nothing choosen
}

void MainWindowPresenter::doActionMindRelearn(QString path)
{
    Repository* r = RepositoryIndexer::getRepositoryForPath(path.toStdString());
    if(r) {
        config.setActiveRepository(
            config.addRepository(r), *this->mdRepositoryConfigRepresentation
        );
        // remember new repository
        mdConfigRepresentation->save(config);
        // learn and show
        mind->learn();
        showInitialView();
    } else {
        QMessageBox::critical(
            &view,
            tr("Learn"),
            tr("This is neither valid MindForger/Markdown workspace nor file."));
    }
}

void MainWindowPresenter::doActionExit()
{
    QApplication::quit();
}

void MainWindowPresenter::doActionFts()
{
    doFts(QString{}, false);
}

void MainWindowPresenter::doFts(const QString& pattern, bool doSearch)
{
    if(pattern.size()) {
        ftsDialog->setSearchPattern(pattern);
    }

    if(orloj->isFacetActiveOutlineOrNoteView()) {
        ftsDialog->setWindowTitle(tr("Notebook Full-text Search"));
        ftsDialog->setScope(
            ResourceType::OUTLINE,
            orloj->getOutlineView()->getCurrentOutline());
    } else if(orloj->isFacetActiveOutlineOrNoteEdit()) {
        ftsDialog->setWindowTitle(tr("Note Full-text Search"));
        ftsDialog->setScope(
            ResourceType::NOTE,
            orloj->getOutlineView()->getCurrentOutline());
    } else {
        ftsDialog->setWindowTitle(tr("Full-text Search"));
        ftsDialog->clearScope();
    }
    ftsDialog->show();

    if(doSearch) {
        ftsDialogPresenter->doSearch();
    }
}

void MainWindowPresenter::slotHandleFts()
{
    ftsDialog->hide();

    QString searchedString = ftsDialog->getSearchPattern();
    switch(ftsDialog->getScopeType()) {
    case ResourceType::NOTE:
        if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
            orloj->getNoteEdit()->getView()->getNoteEditor()->findString(
                searchedString,
                ftsDialog->isEditorReverse(),
                ftsDialog->isEditorCaseInsensitive(),
                ftsDialog->isEditorWords());
        } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
            orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->findString(
                searchedString,
                ftsDialog->isEditorReverse(),
                ftsDialog->isEditorCaseInsensitive(),
                ftsDialog->isEditorWords());
        }
        break;
    default:
        // repository or O FTS
        if(ftsDialogPresenter->getSelectedNote()) {
            orloj->showFacetOutline(ftsDialogPresenter->getSelectedNote()->getOutline());
            orloj->showFacetNoteView(ftsDialogPresenter->getSelectedNote());
            orloj->getNoteView()->getView()->getViever()->findText(searchedString);
        }
    }
}

void MainWindowPresenter::slotMainToolbarVisibilityChanged(bool visibility)
{
    MF_DEBUG("Main toolbar visibility changed: " << boolalpha << visibility << endl);
    this->config.setUiShowToolbar(visibility);
    mdConfigRepresentation->save(config);
}

void MainWindowPresenter::doActionFindOutlineByName(const std::string& phrase)
{
    // IMPROVE rebuild model ONLY if dirty i.e. an outline name was changed on save
    vector<Outline*> os{mind->getOutlines()};
    Outline::sortByRead(os);
    vector<Thing*> es{os.begin(),os.end()};

    findOutlineByNameDialog->show(es);
    if(phrase.size()) {
        findOutlineByNameDialog->setSearchedString(QString::fromStdString(phrase));
    }
}

void MainWindowPresenter::handleFindOutlineByName()
{
    if(findOutlineByNameDialog->getChoice()) {
        Outline* o = (Outline*)findOutlineByNameDialog->getChoice();
        if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
            orloj->getOutlinesMap()->selectRowByOutlineKey(o->getKey());
            return;
        } else {
            orloj->showFacetOutline(o);
        }

        // IMPROVE make this more efficient
        statusBar->showInfo(QString(tr("Notebook "))+QString::fromStdString(findOutlineByNameDialog->getChoice()->getName()));
    } else {
        statusBar->showInfo(QString(tr("Notebook not found")+": ") += findOutlineByNameDialog->getSearchedString());
    }
}

void MainWindowPresenter::handleFindThingByName()
{
    if(findThingByNameDialog->getChoice()) {
        if(mind->remind().getOutline(findThingByNameDialog->getChoice()->getKey())) {
            orloj->showFacetOutline((Outline*)findThingByNameDialog->getChoice());
            statusBar->showInfo(QString(tr("Notebook "))+QString::fromStdString(findThingByNameDialog->getChoice()->getKey()));
        } else {
            orloj->showFacetNoteView((Note*)findThingByNameDialog->getChoice());
            statusBar->showInfo(QString(tr("Note "))+QString::fromStdString(findThingByNameDialog->getChoice()->getKey()));
        }
    } else {
        statusBar->showInfo(QString(tr("Thing not found")+": ") += findThingByNameDialog->getSearchedString());
    }
}

void MainWindowPresenter::doActionFindOutlineByTag(const string& tag)
{
    // IMPROVE rebuild model ONLY if dirty i.e. an outline name was changed on save
    vector<Outline*> os{mind->getOutlines()};
    Outline::sortByName(os);
    vector<Thing*> outlines{os.begin(),os.end()};

    findOutlineByTagDialog->show(outlines, nullptr, nullptr, tag);
}

void MainWindowPresenter::handleFindOutlineByTag()
{
    if(findOutlineByTagDialog->getChoice()) {
        orloj->showFacetOutline((Outline*)findOutlineByTagDialog->getChoice());
        // IMPROVE make this more efficient
        statusBar->showInfo(QString(tr("Notebook "))+QString::fromStdString(findOutlineByTagDialog->getChoice()->getName()));
    } else {
        statusBar->showInfo(QString(tr("Notebook not found")));
    }
}

void MainWindowPresenter::doActionFindNoteByTag()
{
    // IMPROVE rebuild model ONLY if dirty i.e. an outline name was changed on save
    if(orloj->isFacetActiveOutlineOrNoteView() || orloj->isFacetActiveOutlineOrNoteEdit()) {
        findNoteByTagDialog->setWindowTitle(tr("Find Note by Tags in Notebook"));
        findNoteByTagDialog->setScope(orloj->getOutlineView()->getCurrentOutline());
        vector<Note*> allNotes(findNoteByTagDialog->getScope()->getNotes());
        findNoteByTagDialog->show(allNotes);
    } else {
        findNoteByTagDialog->setWindowTitle(tr("Find Note by Tags"));
        findNoteByTagDialog->clearScope();
        vector<Note*> allNotes{};
        mind->getAllNotes(allNotes);
        findNoteByTagDialog->show(allNotes);
    }
}

void MainWindowPresenter::doTriggerFindNoteByTag(const Tag* tag)
{
    findNoteByTagDialog->setWindowTitle(tr("Find Note by Tags"));
    findNoteByTagDialog->clearScope();
    vector<Note*> allNotes{};
    mind->getAllNotes(allNotes);
    vector<const Tag*> tags{};
    tags.push_back(tag);
    findNoteByTagDialog->show(allNotes, &tags);
}

void MainWindowPresenter::doSwitchFindByTagDialog(bool toFindNotesByTag)
{
    // switch dialogs and transfer selected tags
    vector<const Tag*>* tags = new vector<const Tag*>{};
    if(toFindNotesByTag) {
        findOutlineByTagDialog->hide();
        findOutlineByTagDialog->getChosenTags(tags);

        vector<Note*> allNotes{};
        mind->getAllNotes(allNotes);
        findNoteByTagDialog->show(allNotes, tags);

    } else {
        findNoteByTagDialog->hide();
        findNoteByTagDialog->getChosenTags(tags);

        vector<Outline*> os{mind->getOutlines()};
        Outline::sortByName(os);
        vector<Thing*> outlines{os.begin(),os.end()};
        findOutlineByTagDialog->show(outlines, tags);
    }
    delete tags;
}

void MainWindowPresenter::handleFindNoteByTag()
{
    if(findNoteByTagDialog->getChoice()) {
        Note* choice = (Note*)findNoteByTagDialog->getChoice();

        choice->incReads();
        choice->makeDirty();

        orloj->showFacetOutline(choice->getOutline());
        orloj->getNoteView()->refresh(choice);
        orloj->showFacetNoteView();
        orloj->getOutlineView()->selectRowByNote(choice);
        // IMPROVE make this more efficient
        statusBar->showInfo(QString(tr("Note "))+QString::fromStdString(choice->getName()));
    } else {
        statusBar->showInfo(QString(tr("Note not found")+": ") += findNoteByNameDialog->getSearchedString());
    }
}

void MainWindowPresenter::doActionRefactorNoteToOutline()
{
    // IMPROVE rebuild model ONLY if dirty i.e. an outline name was changed on save
    vector<Outline*> os{mind->getOutlines()};
    Outline::sortByName(os);
    vector<Thing*> es{os.begin(),os.end()};

    refactorNoteToOutlineDialog->show(es);
}

void MainWindowPresenter::handleRefactorNoteToOutline()
{
    // IMPROVE check current view to be VIEW or EDIT NOTE
    Note* noteToRefactor = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    if(noteToRefactor) {
        if(refactorNoteToOutlineDialog->getChoice()) {
            Outline* dstOutline = (Outline*)refactorNoteToOutlineDialog->getChoice();
            mind->noteRefactor(noteToRefactor, dstOutline->getKey());

            orloj->showFacetOutline((Outline*)refactorNoteToOutlineDialog->getChoice());

            // IMPROVE make this more efficient .arg() + add Note's name
            statusBar->showInfo(QString(tr("Refactored Note to Notebook '"))+QString::fromStdString(refactorNoteToOutlineDialog->getChoice()->getName())+"'...");
        } else {
            statusBar->showInfo(QString(tr("Target Notebook not found")+": ") += refactorNoteToOutlineDialog->getSearchedString());
        }
    } else {
        QMessageBox::critical(&view, tr("Refactor Note"), tr("Note to be refactored not specified!"));
    }
}

void MainWindowPresenter::doActionFindNoteByName()
{
    // IMPROVE rebuild model ONLY if dirty i.e. an outline name was changed on save
    if(orloj->isFacetActiveOutlineOrNoteView() || orloj->isFacetActiveOutlineOrNoteEdit()) {
        findNoteByNameDialog->setWindowTitle(tr("Find Note by Name in Notebook"));
        findNoteByNameDialog->setScope(orloj->getOutlineView()->getCurrentOutline());
        vector<Note*> allNotes(findNoteByNameDialog->getScope()->getNotes());
        Outline::sortByRead(allNotes);
        findNoteByNameDialog->show(allNotes);
    } else {
        findNoteByNameDialog->setWindowTitle(tr("Find Note by Name"));
        findNoteByNameDialog->clearScope();
        vector<Note*> allNotes{};
        mind->getAllNotes(allNotes);
        Outline::sortByRead(allNotes);
        findNoteByNameDialog->show(allNotes);
    }
}

void MainWindowPresenter::handleFindNoteByName()
{
    if(findNoteByNameDialog->getChoice()) {
        Note* choice = (Note*)findNoteByNameDialog->getChoice();

        choice->incReads();
        choice->makeDirty();

        orloj->showFacetOutline(choice->getOutline());
        orloj->getNoteView()->refresh(choice);
        orloj->showFacetNoteView();
        orloj->getOutlineView()->selectRowByNote(choice);
        // IMPROVE make this more efficient
        statusBar->showInfo(QString(tr("Note "))+QString::fromStdString(choice->getName()));
    } else {
        statusBar->showInfo(QString(tr("Note not found")+": ") += findNoteByNameDialog->getSearchedString());
    }
}

#ifdef MF_NER

void MainWindowPresenter::doActionFindNerPersons()
{
    if(orloj->isFacetActiveOutlineManagement()) {
        nerChooseTagsDialog->clearCheckboxes();
        nerChooseTagsDialog->getPersonsCheckbox()->setChecked(true);
        nerChooseTagsDialog->show();
    } else {
        statusBar->showInfo(tr("Initializing NER and predicting..."));
        QMessageBox::critical(&view, tr("NER"), tr("Memory NER not implemented yet."));
    }
}
void MainWindowPresenter::doActionFindNerLocations()
{
    if(orloj->isFacetActiveOutlineManagement()) {
        nerChooseTagsDialog->clearCheckboxes();
        nerChooseTagsDialog->getLocationsCheckbox()->setChecked(true);
        nerChooseTagsDialog->show();
    } else {
        statusBar->showInfo(tr("Initializing NER and predicting..."));
        QMessageBox::critical(&view, tr("NER"), tr("Memory NER not implemented yet."));
    }
}
void MainWindowPresenter::doActionFindNerOrganizations()
{
    if(orloj->isFacetActiveOutlineManagement()) {
        nerChooseTagsDialog->clearCheckboxes();
        nerChooseTagsDialog->getOrganizationsCheckbox()->setChecked(true);
        nerChooseTagsDialog->show();
    } else {
        statusBar->showInfo(tr("Initializing NER and predicting..."));
        QMessageBox::critical(&view, tr("NER"), tr("Memory NER not implemented yet."));
    }
}
void MainWindowPresenter::doActionFindNerMisc()
{
    if(orloj->isFacetActiveOutlineManagement()) {
        nerChooseTagsDialog->clearCheckboxes();
        nerChooseTagsDialog->getMiscCheckbox()->setChecked(true);
        nerChooseTagsDialog->show();
    } else {
        statusBar->showInfo(tr("Initializing NER and predicting..."));
        QMessageBox::critical(&view, tr("NER"), tr("Memory NER not implemented yet."));
    }
}

NerMainWindowWorkerThread* MainWindowPresenter::startNerWorkerThread(
        Mind* m,
        OrlojPresenter* o,
        int f,
        std::vector<NerNamedEntity>* r,
        QDialog* d)
{
    QThread* thread = new QThread;
    NerMainWindowWorkerThread* worker
        = new NerMainWindowWorkerThread(thread, m, o, f, r, d);

    // signals
    worker->moveToThread(thread);
    // TODO implement dialog w/ error handling - QObject::connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    QObject::connect(thread, SIGNAL(started()), worker, SLOT(process()));
    // open dialog to choose from result(s)
    QObject::connect(worker, SIGNAL(finished()), this, SLOT(handleChooseNerEntityResult()));
    // worker's finished signal quits thread ~ thread CANNOT be reused
    QObject::connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    // schedule thread for automatic deletion by Qt - I delete worker myself
    //QObject::connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();

    return worker;
}

// handleFindNerPerson() -> handleChooseNerEntityResult() -> handleFtsNerEntity()
void MainWindowPresenter::handleFindNerEntities()
{
    nerChooseTagsDialog->hide();

    int entityFilter{};
    entityFilter =
          (nerChooseTagsDialog->getPersonsCheckbox()->isChecked()?NerNamedEntityType::PERSON:0) |
          (nerChooseTagsDialog->getLocationsCheckbox()->isChecked()?NerNamedEntityType::LOCATION:0) |
          (nerChooseTagsDialog->getOrganizationsCheckbox()->isChecked()?NerNamedEntityType::ORGANIZATION:0) |
          (nerChooseTagsDialog->getMiscCheckbox()->isChecked()?NerNamedEntityType::MISC:0);

    MF_DEBUG("Named-entity type filter: " << entityFilter << endl);

    vector<NerNamedEntity>* result
        = new vector<NerNamedEntity>{};
    if(mind->isNerInitilized()) {
        statusBar->showInfo(tr("Recognizing named entities..."));

        mind->recognizePersons(orloj->getOutlineView()->getCurrentOutline(), entityFilter, *result);

        chooseNerEntityResult(result);
    } else {
        statusBar->showInfo(tr("Initializing NER and recognizing named entities..."));

        // launch async worker
        QDialog* progressDialog
            = new QDialog{&view};
        nerWorker
            = startNerWorkerThread(mind, orloj, entityFilter, result, progressDialog);

        // show PROGRESS dialog - will be closed by worker
        QVBoxLayout* mainLayout = new QVBoxLayout{};
        QLabel* l = new QLabel{tr(" Initializing (the first run only) NER and predicting... ")};
        mainLayout->addWidget(l);
        progressDialog->setLayout(mainLayout);
        progressDialog->setWindowTitle(tr("Named-entity Recognition"));
        //progressDialog->resize(fontMetrics().averageCharWidth()*35, height());
        //progressDialog->setModal(true);
        progressDialog->update();
        progressDialog->activateWindow();
        progressDialog->show();
        // dialog is deleted by worker thread
    }
}

void MainWindowPresenter::chooseNerEntityResult(vector<NerNamedEntity>* nerEntities)
{
    MF_DEBUG("Showing NER results to choose one entity for FTS..." << endl);
    statusBar->showInfo(tr("NER predicition finished"));

    if(nerEntities && nerEntities->size()) {
        nerResultDialog->show(*nerEntities);
    } else {
        QMessageBox::information(&view, tr("Named-entity Recognition"), tr("No named entities recognized."));
    }
}

void MainWindowPresenter::handleChooseNerEntityResult()
{
    vector<NerNamedEntity>* nerEntities = nerWorker->getResult();
    chooseNerEntityResult(nerEntities);

    // cleanup: thread is deleted by Qt (deleteLater() signal)
    delete nerEntities;
    delete nerWorker;
}

void MainWindowPresenter::handleFtsNerEntity()
{
    if(nerResultDialog->getChoice().size()) {
        executeFts(
            nerResultDialog->getChoice(),
            false,
            orloj->getOutlineView()->getCurrentOutline());
    }
}

#endif

void MainWindowPresenter::doActionViewRecentNotes()
{
    vector<Note*> notes{};
    mind->getAllNotes(notes, true, config.isRecentIncludeOs());
    orloj->showFacetRecentNotes(notes);
}

void MainWindowPresenter::doActionViewDashboard()
{
    if(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY) {
        orloj->showFacetDashboard();
    }
}

void MainWindowPresenter::sortAndSaveOrganizersConfig()
{
    if(config.hasRepositoryConfiguration()) {
        config.getRepositoryConfiguration().sortOrganizers();
        getConfigRepresentation()->save(config);
    }
}

void MainWindowPresenter::doActionViewOrganizers()
{
    if(config.getActiveRepository()->getMode()
       == Repository::RepositoryMode::REPOSITORY
    ) {
        orloj->showFacetOrganizerList(
            config.getRepositoryConfiguration().getOrganizers()
        );
    }
}

void MainWindowPresenter::doActionViewOrganizer()
{
    if(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY) {
        orloj->slotShowSelectedOrganizer();
    }
}

void MainWindowPresenter::doActionViewKnowledgeGraphNavigator()
{
    orloj->showFacetKnowledgeGraphNavigator();
}

bool MainWindowPresenter::doActionViewHome()
{
    vector<const Tag*> tagsFilter{};
    tagsFilter.push_back(mind->remind().getOntology().findOrCreateTag(Tag::KeyMindForgerHome()));
    vector<Outline*> homeOutline{};
    mind->findOutlinesByTags(tagsFilter, homeOutline);
    if(homeOutline.size()) {
        orloj->showFacetOutline(homeOutline.at(0));
        return true;
    } else {
        statusBar->showInfo(
            tr("Home Notebook not set - use menu 'Notebooks/Make Home'")
        );
        return false;
    }
}

void MainWindowPresenter::doActionViewOutlines()
{
    if(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY) {
        view.getCli()->setBreadcrumbPath("/notebooks");
        cli->executeListOutlines();
        view.getOrloj()->getOutlinesTable()->setFocus();
    }
}

void MainWindowPresenter::doActionViewOutlinesMap()
{
    if(config.getActiveRepository()->getType()==Repository::RepositoryType::MINDFORGER
         &&
       config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY)
    {
        orloj->showFacetOutlinesMap(mind->outlinesMapGet());
    }
}

void MainWindowPresenter::doActionViewTagCloud()
{
    orloj->showFacetTagCloud();
}

void MainWindowPresenter::doActionCli()
{
    view.getCli()->setBreadcrumbPath("/");
    view.getCli()->showCli();
}

void MainWindowPresenter::doActionViewDistractionFree()
{
    if(view.statusBar()->isVisible()) {
        view.menuBar()->hide();
        view.getCli()->hide();
        view.statusBar()->hide();
    } else {
        view.menuBar()->show();
        view.getCli()->show();
        view.statusBar()->show();
    }
}

void MainWindowPresenter::doActionViewFullscreen()
{
    if(view.isFullScreen()) {
        view.showMaximized();
    } else {
        view.showFullScreen();
    }
}

void MainWindowPresenter::doActionNavigatorShuffle()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_NAVIGATOR)) {
        orloj->getNavigator()->shuffle();
    }
}

void MainWindowPresenter::doActionFormatBold()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->wrapSelectedText("**");
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->wrapSelectedText("**");
    }
}

void MainWindowPresenter::doActionFormatItalic()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->wrapSelectedText("_");
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->wrapSelectedText("_");
    }
}

void MainWindowPresenter::doActionFormatCode()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->wrapSelectedText("`");
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->wrapSelectedText("`");
    }
}

void MainWindowPresenter::doActionFormatComment()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        // note 3 x - which ensures that comment will work also in Pandoc, for comprehensive MD comments discussion check:
        //   https://stackoverflow.com/questions/4823468/comments-in-markdown
        orloj->getNoteEdit()->getView()->getNoteEditor()->wrapSelectedText("<!--- ", " -->");
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->wrapSelectedText("<!--- ", " -->");
    }
}

void MainWindowPresenter::doActionFormatMath()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->wrapSelectedText("$");
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->wrapSelectedText("$");
    }
}

void MainWindowPresenter::doActionFormatMathFrac()
{
    injectMarkdownText("\\frac{}{}", false, 6);
}
void MainWindowPresenter::doActionFormatMathSum()
{
    injectMarkdownText("\\sum_{i=0}^n", false, 12);
}
void MainWindowPresenter::doActionFormatMathInt()
{
    injectMarkdownText("\\int_{x}^{y}", false, 12);
}
void MainWindowPresenter::doActionFormatMathIiint()
{
    injectMarkdownText("\\iiint", false, 3);
}
void MainWindowPresenter::doActionFormatMathAlpha()
{
    injectMarkdownText("\\alpha", false, 6);
}
void MainWindowPresenter::doActionFormatMathBeta()
{
    injectMarkdownText("\\beta", false, 5);
}
void MainWindowPresenter::doActionFormatMathDelta()
{
    injectMarkdownText("\\Delta", false, 6);
}
void MainWindowPresenter::doActionFormatMathGama()
{
    injectMarkdownText("\\Gama", false, 5);
}
void MainWindowPresenter::doActionFormatMathText()
{
    injectMarkdownText("\\text{}", false, 6);
}
void MainWindowPresenter::doActionFormatMathBar()
{
    injectMarkdownText("\\bar", false, 4);
}
void MainWindowPresenter::doActionFormatMathHat()
{
    injectMarkdownText("\\hat", false, 4);
}
void MainWindowPresenter::doActionFormatMathDot()
{
    injectMarkdownText("\\dot", false, 4);
}
void MainWindowPresenter::doActionFormatMathOverrightarrow()
{
    injectMarkdownText("\\overrightarrow", false, 15);
}
void MainWindowPresenter::doActionFormatMathCup()
{
    injectMarkdownText("\\cup", false, 4);
}
void MainWindowPresenter::doActionFormatMathCap()
{
    injectMarkdownText("\\cap", false, 4);
}
void MainWindowPresenter::doActionFormatMathEmptyset()
{
    injectMarkdownText("\\emptyset", false, 9);
}
void MainWindowPresenter::doActionFormatMathIn()
{
    injectMarkdownText("\\in", false, 3);
}
void MainWindowPresenter::doActionFormatMathNotin()
{
    injectMarkdownText("\\notin", false, 6);
}
void MainWindowPresenter::doActionFormatMathSqrt()
{
    injectMarkdownText("\\sqrt{}", false, 6);
}

void MainWindowPresenter::doActionFormatStrikethrough()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->wrapSelectedText("~~");
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->wrapSelectedText("~~");
    }
}

void MainWindowPresenter::doActionFormatKeyboard()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->wrapSelectedText("<kbd>", "</kbd>");
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->wrapSelectedText("<kbd>", "</kbd>");
    }
}

void MainWindowPresenter::handleRowsAndDepth()
{
    QString text{};

    if(rowsAndDepthDialog->getPurpose()==RowsAndDepthDialog::Purpose::BULLETS) {
        text += "\n";
        for(int r=0; r<rowsAndDepthDialog->getRows(); r++) {
            for(int d=0; d<rowsAndDepthDialog->getDepth(); d++) {
                for(int t=0; t<d; t++) {
                    text += "    ";
                }
                text += "* ...\n";
            }
        }
    } else if(rowsAndDepthDialog->getPurpose()==RowsAndDepthDialog::Purpose::NUMBERS) {
        text += "\n";
        for(int r=0; r<rowsAndDepthDialog->getRows(); r++) {
            for(int d=0; d<rowsAndDepthDialog->getDepth(); d++) {
                for(int t=0; t<d; t++) {
                    text += "    ";
                }
                text += QString::number(r+1);
                text += ". ...\n";
            }
        }
    } else if(rowsAndDepthDialog->getPurpose()==RowsAndDepthDialog::Purpose::TASKS) {
        text += "\n";
        for(int r=0; r<rowsAndDepthDialog->getRows(); r++) {
            for(int d=0; d<rowsAndDepthDialog->getDepth(); d++) {
                for(int t=0; t<d; t++) {
                    text += "    ";
                }
                text += QString::number(r+1);
                text += ". [";
                if(d%2) text+="x"; else text+=" ";
                text += "] ...\n";
            }
        }
    } else if(rowsAndDepthDialog->getPurpose()==RowsAndDepthDialog::Purpose::BLOCKQUOTE) {
        for(int r=0; r<rowsAndDepthDialog->getRows(); r++) {
            text += "\n";
            for(int d=0; d<rowsAndDepthDialog->getDepth(); d++) {
                for(int t=0; t<d; t++) {
                    text += ">";
                }
                text += "> ...\n";
            }
        }
    }

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text, false);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text, false);
    }
}

void MainWindowPresenter::doActionFormatListBullet()
{
    rowsAndDepthDialog->setPurpose(RowsAndDepthDialog::Purpose::BULLETS);
    rowsAndDepthDialog->show();
}

void MainWindowPresenter::doActionFormatListNumber()
{
    rowsAndDepthDialog->setPurpose(RowsAndDepthDialog::Purpose::NUMBERS);
    rowsAndDepthDialog->show();
}

void MainWindowPresenter::doActionFormatListTask()
{
    rowsAndDepthDialog->setPurpose(RowsAndDepthDialog::Purpose::TASKS);
    rowsAndDepthDialog->show();
}

void MainWindowPresenter::doActionFormatListTaskItem()
{
    QString text{"[ ] "};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text, false, text.length());
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text, false, text.length());
    }
}

void MainWindowPresenter::doActionFormatToc(bool withTags)
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
            || orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER))
    {
        string* text = mdRepresentation->toc(orloj->getOutlineView()->getCurrentOutline(), withTags);

        if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
            orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(QString::fromStdString(*text));
        } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
            orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(QString::fromStdString(*text));
        }

        delete text;
    }
}

void MainWindowPresenter::doActionFormatTocWithTags()
{
    this->doActionFormatToc(true);
}

void MainWindowPresenter::doActionFormatTocWithoutTags()
{
    this->doActionFormatToc(false);
}

// IMPROVE: consolidate methods which just insert a (semi)static string
void MainWindowPresenter::doActionFormatTimestamp()
{
    QString text{QString::fromStdString(datetimeToString(datetimeNow()))};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text, false, text.size());
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text, false, text.size());
    }
}

void MainWindowPresenter::doActionFormatCodeBlock()
{
    // IMPROVE ask for dialect
    QString text{
        "\n"
        "```\n"
        "...\n"
        "```\n"
    };

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text);
    }
}

void MainWindowPresenter::doActionFormatMathBlock()
{
    QString text{"\n$$\n...\n$$\n"};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text);
    }
}


void MainWindowPresenter::injectDiagramBlock(const QString& diagramText)
{
    // QString text{"\n```mermaid\n...\n```\n"};
    QString text{"\n<div class=\"mermaid\">\n"};
    text += diagramText;
    text += QString{"\n</div>\n"};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text, false, 1);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text, false, 1);
    }
}


void MainWindowPresenter::doActionFormatDiagramBlock()
{
    injectDiagramBlock(QString{"..."});
}

void MainWindowPresenter::doActionFormatDiagramPie()
{
    injectDiagramBlock(
        QString{
            "pie title Pets\n"
            "    \"Dogs\" : 386\n"
            "    \"Cats\" : 85\n"
            "    \"Rats\" : 15"
        }
    );
}

void MainWindowPresenter::doActionFormatDiagramFlow()
{
    injectDiagramBlock(
        QString{
            "graph TD\n"
            "a --> b\n"
            "a --> c"
        }
    );
}

void MainWindowPresenter::doActionFormatDiagramClass()
{
    injectDiagramBlock(
        QString{
            "classDiagram\n"
            "     class Animal\n"
            "     Animal : +int age\n"
            "     Animal : -String gender\n"
            "     Animal: +isMammal()\n"
            "     Animal: *mate()"
        }
    );
}

void MainWindowPresenter::doActionFormatDiagramGantt()
{
    injectDiagramBlock(
        QString{
            "gantt\n"
            "        dateFormat  YYYY-MM-DD\n"
            "        title GANTT diagram\n"
            "        section A section\n"
            "        Completed task            :done,    des1, 2014-01-06,2014-01-08\n"
            "        Active task               :active,  des2, 2014-01-09, 3d\n"
            "        Future task               :         des3, after des2, 5d\n"
            "        section Critical tasks\n"
            "        Completed task in the critical line :crit, done, 2014-01-06,24h\n"
            "        Create tests for parser             :crit, active, 3d\n"
            "        Future task in critical line        :crit, 5d\n"
            "        Add to mermaid                      :1d"
        }
    );
}

void MainWindowPresenter::doActionFormatDiagramState()
{
    injectDiagramBlock(QString{"stateDiagram    \ns1"});
}

void MainWindowPresenter::doActionFormatDiagramSequence()
{
    injectDiagramBlock(
         QString{
            "sequenceDiagram\n"
            "    participant John\n"
            "    participant Alice\n"
            "    Alice->>John: Hello John, how are you?\n"
            "    John-->>Alice: Great!"
         }
    );
}

void MainWindowPresenter::doActionFormatBlockquote()
{
    rowsAndDepthDialog->setPurpose(RowsAndDepthDialog::Purpose::BLOCKQUOTE);
    rowsAndDepthDialog->show();
}

void MainWindowPresenter::doActionFormatTable()
{
    // IMPROVE ask for number of items using dialog
    // IMPROVE left/right alignment options
    int count=3;
    QString text{"\n . | . | .\n --- | --- | ---\n"};
    for(int i=1; i<=count; i++) {
        text += " . | . | .\n";
    }

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text);
    }
}

void MainWindowPresenter::doActionFormatLinkOrImage(QString link)
{
    // IMPROVE rebuild model ONLY if dirty i.e. an outline name was changed on save
    vector<Outline*> oss{mind->getOutlines()};
    Outline::sortByName(oss);
    vector<Thing*> os{oss.begin(), oss.end()};

    vector<Note*> ns{};
    mind->getAllNotes(ns);

    QString selectedText;
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        selectedText = orloj->getNoteEdit()->getView()->getNoteEditor()->getSelectedText();
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        selectedText = orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->getSelectedText();
    }

    if(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY) {
        insertLinkDialog->getCopyCheckBox()->setEnabled(true);
    } else {
        insertLinkDialog->getCopyCheckBox()->setChecked(false);
        insertLinkDialog->getCopyCheckBox()->setEnabled(false);
    }

    if(link.size() && (
         link.endsWith(".png") ||
         link.endsWith(".gif") ||
         link.endsWith(".jpg") ||
         link.endsWith(".jpeg") ||
         link.endsWith(".PNG") ||
         link.endsWith(".GIF") ||
         link.endsWith(".JPG") ||
         link.endsWith(".JPEG")))
    {
        insertImageDialog->show(
            selectedText.size()?selectedText:QString{tr("image")},
            link
        );
    } else {
        insertLinkDialog->show(
            config.getActiveRepository(),
            orloj->getOutlineView()->getCurrentOutline(),
            os,
            ns,
            selectedText,
            link);
    }
}

void MainWindowPresenter::doActionFormatLink()
{
    doActionFormatLinkOrImage(QString{});
}

void MainWindowPresenter::injectMarkdownText(const QString& text, bool newline, int offset)
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text, newline, offset);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text, newline, offset);
    }
}

void MainWindowPresenter::copyLinkOrImageToRepository(const string& srcPath, QString& path)
{
    if(isDirectoryOrFileExists(srcPath.c_str())) {
        QString pathPrefix{};
        string oPath{};
        if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
            oPath = orloj->getNoteEdit()->getCurrentNote()->getOutlineKey();
        } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
            oPath = orloj->getOutlineHeaderEdit()->getCurrentOutline()->getKey();
        }
        if(stringEndsWith(oPath, File::EXTENSION_MD_MD)) {
            pathPrefix = QString::fromStdString(oPath.substr(0, oPath.length()-3));
        } else {
            pathPrefix = QString::fromStdString(oPath);
        }
        pathPrefix.append(".");

        string d{}, f{};
#if defined(_WIN32)
        QString src{srcPath.c_str()};
        pathToDirectoryAndFile(src.replace("/", "\\").toStdString(), d, f);
        QString pathSuffix{QString::fromStdString(f)};
        path = pathPrefix.replace("/", "\\") + pathSuffix;
#else
        pathToDirectoryAndFile(srcPath, d, f);
        QString pathSuffix{QString::fromStdString(f)};

        path = pathPrefix + pathSuffix;
#endif

        while(isDirectoryOrFileExists(path.toStdString().c_str())) {
            pathSuffix.prepend("_");
            path = pathPrefix + pathSuffix;
        }
#if defined(_WIN32)
        MF_DEBUG("Copying: " << src.toStdString() << " > " << path.toStdString() << endl);
        copyFile(src.toStdString(), path.toStdString());
#else
        copyFile(srcPath, path.toStdString());
#endif

        d.clear();
        f.clear();
        pathToDirectoryAndFile(path.toStdString(), d, f);
        path = QString::fromStdString(f);

        statusBar->showInfo(
            tr("File copied to workspace path '%1'").arg(path.toStdString().c_str())
        );
    } else {
        // fallback: create link, but don't copy
        path = insertLinkDialog->getPathText();
        statusBar->showInfo(tr("Given path '%1' doesn't exist - target will not be copied, but link will be created").arg(path.toStdString().c_str()));
    }
}

// IMPROVE optimize this function (QString, string, ...)
// IMPROVE deduplicate this method and copy image/attachment code
void MainWindowPresenter::doActionEditPasteImageData(QImage image)
{
    // save image object as file
    string oPath{};
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        oPath = orloj->getNoteEdit()->getCurrentNote()->getOutlineKey();
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        oPath = orloj->getOutlineHeaderEdit()->getCurrentOutline()->getKey();
    }
    QString pathPrefix{};
    if(stringEndsWith(oPath, File::EXTENSION_MD_MD)) {
        pathPrefix = QString::fromStdString(oPath.substr(0, oPath.length()-3));
    } else {
        pathPrefix = QString::fromStdString(oPath);
    }
    pathPrefix.append(".");
    QString pathSuffix{"image.png"};
    QString path = pathPrefix + pathSuffix;
    while(isDirectoryOrFileExists(path.toStdString().c_str())) {
        pathSuffix.prepend("_");
        path = pathPrefix + pathSuffix;
    }

    statusBar->showInfo(tr("Saving pasted image data to file: '%1'").arg(path.toStdString().c_str()));
    image.save(path);

    // inject link to file to O
    injectImageLinkToEditor(path, QString{"image"});
}

void MainWindowPresenter::doActionOpenRunToolDialog(QString& phrase)
{
    MF_DEBUG("SIGNAL handled: open run tool dialog...");
    this->runToolDialog->setPhraseText(phrase);
    QString templateText = this->runToolDialog->getTemplateTextForToolName(
        this->runToolDialog->getSelectedTool().toStdString());
    if(templateText.length() == 0) {
        return;
    }
    this->runToolDialog->setTemplateText(templateText);

    this->runToolDialog->show();
}

void MainWindowPresenter::handleRunTool()
{
    this->runToolDialog->hide();

    string selectedTool{
        this->runToolDialog->getSelectedTool().toStdString()
    };

    QString phrase=this->runToolDialog->getPhraseText();
    if(phrase.length() == 0) {
        QMessageBox msgBox{
            QMessageBox::Critical,
            QObject::tr("Empty Phrase"),
            QObject::tr("Phrase to search/explain/process is empty.")
        };
        msgBox.exec();
        return;
    }

    // get & check template text validity
    QString templateText = this->runToolDialog->getTemplateText();

    // phrase replace @ template > get command, if invalid, then fallback
    QString command = templateText.replace(
        QString{TOOL_PHRASE}, phrase
    );

    // RUN tool
    QDesktopServices::openUrl(QUrl{command});
}

void MainWindowPresenter::statusInfoPreviewFlickering()
{
    statusBar->showInfo(
        QString(
            tr(
                "HTML Note preview flickering can be eliminated by disabling math "
                "and diagrams in Preferences menu")));
}

/*
 * See InsertLinkDialog for link creation hints
 */
void MainWindowPresenter::handleFormatLink()
{
    insertLinkDialog->hide();

    QString path{};
    if(insertLinkDialog->isCopyToRepo()) {
        copyLinkOrImageToRepository(insertLinkDialog->getPathText().toStdString(), path);
    } else {
        path = insertLinkDialog->getPathText();
    }

    // IMPROVE make this reusable method
    QString text{"["};
    text += insertLinkDialog->getLinkText();
    text += "](";
    text += QString{path}.replace(" ","%20");
    text += ")";

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        if(orloj->getNoteEdit()->getView()->getNoteEditor()->getSelectedText().size()) {
            orloj->getNoteEdit()->getView()->getNoteEditor()->removeSelectedText();
        }
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text, false, 1);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        if(orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->getSelectedText().size()) {
            orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->removeSelectedText();
        }
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text, false, 1);
    }
}

void MainWindowPresenter::doActionFormatImage()
{
    if(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY) {
        insertImageDialog->getCopyCheckBox()->setEnabled(true);
    } else {
        insertImageDialog->getCopyCheckBox()->setChecked(false);
        insertImageDialog->getCopyCheckBox()->setEnabled(false);
    }

    insertImageDialog->show();
}

void MainWindowPresenter::injectImageLinkToEditor(
        const QString& path,
        const QString& alternateText)
{
    QString text{"!["};
    if(alternateText.size()) {
        text += alternateText;
    } else {
        text += insertImageDialog->getAlternateText();
    }
    text += "](";
#ifdef _WIN32
    // image links are processed by HTML browser > \s must be replaced with /s
    // (attachments use \s as the path is used by OS tools)
    text +=  QString{path}.replace("\\", "/").replace(" ","%20");
#else
    text += QString{path}.replace(" ","%20");
#endif
    text += ")";

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text, false, 2);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text, false);
    }
}

void MainWindowPresenter::handleFormatImage()
{
    insertImageDialog->hide();

    QString path{};
    if(insertImageDialog->isCopyToRepo()) {
        copyLinkOrImageToRepository(insertImageDialog->getPathText().toStdString(), path);
    } else {
        path = insertImageDialog->getPathText();
    }

    injectImageLinkToEditor(path, QString{});
}

void MainWindowPresenter::doActionFormatHr()
{
    QString text{"\n---"};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text);
    }
}

void MainWindowPresenter::doActionOutlineNew()
{
    newOutlineDialog->show(
        mind->remind().getStencils(ResourceType::OUTLINE),
        Configuration::getInstance().getActiveRepository()->getType()
    );
}

void MainWindowPresenter::doActionArxivToolbar()
{
    handleLeftToolbarAction(TOOL_ARXIV);
}

void MainWindowPresenter::doActionWikipediaToolbar()
{
    handleLeftToolbarAction(TOOL_WIKIPEDIA);
}

void MainWindowPresenter::doActionStackOverflowToolbar()
{
    handleLeftToolbarAction(TOOL_STACK_OVERFLOW);
}

void MainWindowPresenter::doActionDuckDuckGoToolbar()
{
    handleLeftToolbarAction(TOOL_DUCKDUCKGO);
}

void MainWindowPresenter::doActionGitHubToolbar()
{
    handleLeftToolbarAction(TOOL_GH_REPOS);
}

void MainWindowPresenter::doActionBardToolbar()
{
    handleLeftToolbarAction(TOOL_GOOGLE_BARD);
}

// TODO remove when code reused by Wingman and @ CLI
void MainWindowPresenter::handleLeftToolbarAction(string selectedTool)
{
    // get PHRASE from the active context:
    // - N editor: get word under cursor OR selected text
    // - N tree: get N name
    // - O tree: get O name
    // - ...
    QString phrase;
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        phrase = orloj->getNoteEdit()->getView()->getNoteEditor()->getToolPhrase();
    } else if(
        orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
        || orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
    ) {
        Outline* o = orloj->getOutlineView()->getCurrentOutline();
        if(o) {
              phrase = QString::fromStdString(o->getName());
        }
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)) {
        Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
        if(note) {
            phrase = QString::fromStdString(note->getName());
        }
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        phrase = orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->getToolPhrase();
    }
    else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_LIST_OUTLINES)) {
        int row = orloj->getOutlinesTable()->getCurrentRow();
        if(row != OutlinesTablePresenter::NO_ROW) {
            QStandardItem* item
                = orloj->getOutlinesTable()->getModel()->item(row);
            if(item) {
                Outline* outline = item->data(Qt::UserRole + 1).value<Outline*>();
                phrase = QString::fromStdString(outline->getName());
            }
        }
    }

    if(phrase.length() == 0) {
        QMessageBox msgBox{
            QMessageBox::Critical,
            QObject::tr("Empty Phrase"),
            QObject::tr("Phrase to search/explain/process is empty.")
        };
        msgBox.exec();
        return;
    }

    // use phrase to RUN the tool
    QString templateText
        = this->runToolDialog->getTemplateTextForToolName(selectedTool);
    MF_DEBUG(
        "Run tool: "
        << phrase.toStdString() << " -> "
        << templateText.toStdString() << " -> "
        << selectedTool << endl
    );

    // phrase replace @ template > get command, if invalid, then fallback
    QString command = templateText.replace(QString{TOOL_PHRASE}, phrase);
    MF_DEBUG("Run tool: command '" << command.toStdString() << "'" << endl);
    QDesktopServices::openUrl(QUrl{command});
}

void MainWindowPresenter::doActionWingman()
{
    MF_DEBUG("SIGNAL handled: WINGMAN dialog...");
    // get PHRASE from the active context:
    // - N editor: get word under cursor OR selected text
    // - N tree: get N name
    // - O tree: get O name
    // - ...
    QString phrase;
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        phrase = orloj->getNoteEdit()->getView()->getNoteEditor()->getToolPhrase();
    } else if(
        orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
        || orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
    ) {
        Outline* o = orloj->getOutlineView()->getCurrentOutline();
        if(o) {
              phrase = QString::fromStdString(o->getName());
        }
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)) {
        Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
        if(note) {
            phrase = QString::fromStdString(note->getName());
        }
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        phrase = orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->getToolPhrase();
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_LIST_OUTLINES)) {
        int row = orloj->getOutlinesTable()->getCurrentRow();
        if(row != OutlinesTablePresenter::NO_ROW) {
            QStandardItem* item
                = orloj->getOutlinesTable()->getModel()->item(row);
            if(item) {
                Outline* outline = item->data(Qt::UserRole + 1).value<Outline*>();
                phrase = QString::fromStdString(outline->getName());
            }
        }
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
        int row = orloj->getOutlinesMap()->getCurrentRow();
        if(row != OutlinesTablePresenter::NO_ROW) {
            QStandardItem* item
                = orloj->getOutlinesMap()->getModel()->item(row);
            if(item) {
                Note* note = item->data(Qt::UserRole + 1).value<Note*>();
                phrase = QString::fromStdString(note->getName());
            }
        }
    }

    if(phrase.length() == 0) {
        QMessageBox msgBox{
            QMessageBox::Critical,
            QObject::tr("Empty Phrase"),
            QObject::tr("Phrase to search/explain/process is empty.")
        };
        msgBox.exec();
        return;
    }

    // TODO set type determined ^
    this->wingmanDialog->initForMode(WingmanDialogModes::WINGMAN_DIALOG_MODE_OUTLINE);
    // TODO set context name e.g. N name
    // TODO set context (actual text to be used in prompt) e.g. N description


    // TODO rename content to context
    this->wingmanDialog->setContextNameText(phrase);
    this->wingmanDialog->show();
}

void MainWindowPresenter::handleActionWingman()
{
    MF_DEBUG("SIGNAL handled: WINGMAN dialog..." << endl);

    string wingmanAnswer{};

    // TODO get and resolve prompt

    // TODO this->wingmanDialog->getPrompt();
    mind->wingmanSummarize(
        "FOO text",
        wingmanAnswer
    );

    // TODO TODO TODO continue here
}

void MainWindowPresenter::doActionOutlineOrNoteNew()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE))
    {
        doActionNoteNew();
    } else {
        doActionOutlineNew();
    }
}

void MainWindowPresenter::handleOutlineNew()
{
    string name = newOutlineDialog->getOutlineName().toStdString();

    // preamble
    vector<string*>* preamble = nullptr;
    if(newOutlineDialog->getPreamble().size()) {
        string* preambleText = new string{newOutlineDialog->getPreamble().toStdString()};
        preamble = new vector<string*>{};
        stringToLines(preambleText, *preamble);
        delete preambleText;
    }

    string outlineKey = mind->outlineNew(
        &name,
        newOutlineDialog->getOutlineType(),
        newOutlineDialog->getImportance(),
        newOutlineDialog->getUrgency(),
        newOutlineDialog->getProgress(),
        &newOutlineDialog->getTags(),
        preamble,
        newOutlineDialog->getStencil());

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_LIST_OUTLINES)) {
        // IMPROVE PERF add only 1 new outline + sort table (don't load all outlines)
        orloj->getOutlinesTable()->refresh(mind->getOutlines());
    }
    // else Outlines are refreshed on facet change

    // open newly create outline
    Outline* o = mind->remind().getOutline(outlineKey);
    if(o) {
        orloj->showFacetOutline(o);
        statusBar->showInfo(QString(tr("Notebook "))+QString::fromStdString(o->getName()));
    }
}

void MainWindowPresenter::doActionOutlineEdit()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
    ) {
        Outline* o = orloj->getOutlineView()->getCurrentOutline();
        if(o) {
            if(withWriteableOutline(o->getKey())) {
                orloj->showFacetOutlineHeaderEdit(o);
            }
            return;
        }
    }
    QMessageBox::critical(&view, tr("Edit Notebook"), tr("Please open an Notebook to edit."));
}

/**
 * @brief Show error dialog if Outline is not writeable.
 * @param outlineKey    key of the Outline whose writability should be checked
 * @return `true` if Outline is writeabl, else show dialog and return `false`
 */
bool MainWindowPresenter::withWriteableOutline(const std::string& outlineKey)
{
    if(!mind->remind().canRemember(outlineKey)) {
        QMessageBox::critical(
            &view,
            tr("🔒 Notebook Write Error"),
            QString{tr("Notebook file is read-only and cannot be written:\n'%1' ")}.arg(outlineKey.c_str())
        );
        return false;
    }
    return true;
}

void MainWindowPresenter::handleNoteNew()
{
    int offset
        = orloj->getOutlineView()->getOutlineTree()->getCurrentRow();
    if(offset == OutlineTreePresenter::NO_ROW) {
        offset = NO_PARENT;
    } else {
        if(newNoteDialog->isPositionBelow()) {
            offset++;
        }
        // else position is ABOVE
    }

    MF_DEBUG("New N: current N offset: " << offset << endl);

    u_int16_t depth;
    Note* n = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    if(n) {
        depth = n->getDepth();
    } else {
        depth = 0;
    }

    string name = newNoteDialog->getNoteName().toStdString();
    Note* note = mind->noteNew(
        orloj->getOutlineView()->getCurrentOutline()->getKey(),
        // IMPROVE get parent note number from selection (if selected)
        offset,
        &name,
        newNoteDialog->getNoteType(),
        depth,
        &newNoteDialog->getTags(),
        newNoteDialog->getProgress(),
        newNoteDialog->getStencil());
    if(note) {
        auto oKey = orloj->getOutlineView()->getCurrentOutline()->getKey();
        if(withWriteableOutline(oKey)) {
            mind->remember(oKey);

            // insert new N and select it in the tree
            orloj->getOutlineView()->insertAndSelect(note);

            // IMPROVE smarter refresh of outline tree (do less than overall load)
            //orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());

            if(newNoteDialog->isOpenInEditor()) {
                orloj->showFacetNoteEdit(note);
            } else {
                orloj->showFacetNoteView(note);
            }
        }
    } else {
        QMessageBox::critical(&view, tr("New Note"), tr("Failed to create new Note!"));
    }
}

void MainWindowPresenter::doActionOutlineClone()
{
    Outline* o = orloj->getOutlineView()->getCurrentOutline();
    if(o) {
        Outline* clonedOutline = mind->outlineClone(o->getKey());
        if(clonedOutline) {
            orloj->getOutlineView()->refresh(clonedOutline);
            orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());
        } else {
            QMessageBox::critical(&view, tr("Clone Notebook"), tr("Failed to clone Notebook!"));
        }
    } else {
        QMessageBox::critical(&view, tr("Clone Notebook"), tr("Please open and Notebook to be cloned."));
    }
}

void MainWindowPresenter::doActionOutlineHome()
{
    if(orloj->isFacetActiveOutlineOrNoteView()) {
        const Tag* t = mind->remind().getOntology().findOrCreateTag(Tag::KeyMindForgerHome());
        Outline* o = orloj->getOutlineView()->getCurrentOutline();
        if(withWriteableOutline(o->getKey())) {
            // if O has tag, then toggle (remove) it, else set the tag
            if(o->hasTag(t)) {
                o->removeTag(t);
                mind->remind().remember(o->getKey());
                statusBar->showInfo(tr("Home tag toggled/removed - Notebook '%1' is no longer home").arg(o->getName().c_str()));
            } else {
                if(mind->setOutlineUniqueTag(t, o->getKey())) {
                    statusBar->showInfo(tr("Notebook '%1' successfully marked as home").arg(o->getName().c_str()));
                }
            }
        }
    } else {
        QMessageBox::critical(&view, tr("Make Notebook home"), tr("Notebook can be marked as home only when viewed."));
    }
}

void MainWindowPresenter::doActionOutlineForget()
{
    if(orloj->isFacetActiveOutlineOrNoteView()
       && withWriteableOutline(orloj->getOutlineView()->getCurrentOutline()->getKey())
    ) {
        QMessageBox msgBox{
            QMessageBox::Question,
            tr("Forget Notebook"),
            tr("Do you really want to deprecate '") +
            QString::fromStdString(orloj->getOutlineView()->getCurrentOutline()->getName()) +
            tr("' Notebook?")
        };
        QPushButton* yes = msgBox.addButton("&Yes", QMessageBox::YesRole);
#ifdef __APPLE__
        yes->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Y));
        yes->setToolTip("⌘Y");

        QPushButton* no =
#endif
        msgBox.addButton("&No", QMessageBox::NoRole);
#ifdef __APPLE__
        no->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
        no->setToolTip("⌘N");
#endif
        msgBox.exec();

        QAbstractButton* choosen = msgBox.clickedButton();
        if(yes == choosen) {
            mind->outlineForget(orloj->getOutlineView()->getCurrentOutline()->getKey());
            orloj->slotShowOutlines();
        } // else do nothing
    } else {
        QMessageBox::critical(&view, tr("Forget Notebook"), tr("Notebook can be forgotten only when viewed."));
    }
}

void MainWindowPresenter::doActionOutlineHtmlExport()
{
    exportOutlineToHtmlDialog->show();
}

void MainWindowPresenter::handleOutlineHtmlExport()
{
    if(isDirectoryOrFileExists(newFileDialog->getFilePath().toStdString().c_str())) {
        QMessageBox::critical(&view, tr("Export Error"), tr("Specified file path already exists!"));
    } else {
        if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
             ||
           orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
             ||
           orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
             ||
           orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
        ) {
            Outline* o = orloj->getOutlineView()->getCurrentOutline();
            if(o) {
                mind->remind().exportToHtml(o, exportOutlineToHtmlDialog->getFilePath().toStdString());
                return;
            }
        }

        QMessageBox::critical(&view, tr("Export Error"), tr("Unable to find Notebook to export!"));
    }
}

void MainWindowPresenter::doActionMindCsvExport()
{
    exportMemoryToCsvDialog->show();
}

void MainWindowPresenter::handleMindCsvExport()
{
    if(isDirectoryOrFileExists(newFileDialog->getFilePath().toStdString().c_str())) {
        QMessageBox::critical(
            &view,
            tr("Export Error"),
            tr("Specified file path already exists!")
        );
    } else {
        StatusBarProgressCallbackCtx callbackCtx{statusBar};
        map<const Tag*,int> tagsCardinality{};
        mind->getTagsCardinality(tagsCardinality);
        mind->remind().exportToCsv(
            exportMemoryToCsvDialog->getFilePath().toStdString(),
            tagsCardinality,
            exportMemoryToCsvDialog->isOheTags()
            ?exportMemoryToCsvDialog->getOheTagsCardinality()
            :-1,
            &callbackCtx
            //[](float progress){ cout << "Export progress: " << progress << endl; }
        );
        statusBar->showInfo(
            "Export to CSV file '"
            + exportMemoryToCsvDialog->getFilePath().toStdString()
            + "' successfully finished"
        );
    }
}

void MainWindowPresenter::doActionOutlineTWikiImport()
{
    QString homeDirectory
        = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);

    QFileDialog importDialog{&view};
    importDialog.setWindowTitle(tr("Import TWiki File"));
    importDialog.setFileMode(QFileDialog::ExistingFile);
    importDialog.setDirectory(homeDirectory);
    importDialog.setViewMode(QFileDialog::Detail);

    QStringList directoryNames{};
    if(importDialog.exec()) {
        directoryNames = importDialog.selectedFiles();
        if(directoryNames.size()==1) {
            mind->learnOutlineTWiki(directoryNames[0].toStdString());

            // refresh O view
            if(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY) {
                orloj->showFacetOutlineList(mind->getOutlines());
            } else {
                if(mind->getOutlines().size()>0) {
                    orloj->showFacetOutline(*mind->getOutlines().begin());
                }
            }
            statusBar->showMindStatistics();
        } // else too many files
    } // else directory closed / nothing choosen
}

void MainWindowPresenter::doActionNoteNew()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE))
    {
        Outline* o = orloj->getOutlineView()->getCurrentOutline();
        if(withWriteableOutline(o->getKey())) {
            newNoteDialog->show(
                QString::fromStdString(o->getKey()),
                mind->remind().getStencils(ResourceType::NOTE),
                config.getActiveRepository()->getType()
            );
        }
    } else {
        QMessageBox::critical(&view, tr("New Note"), tr("Open and view a Notebook to create new Note."));
    }
}

void MainWindowPresenter::doActionOutlineOrNoteEdit()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
    ) {
        Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
        if(note) {
            if(withWriteableOutline(note->getOutline()->getKey())) {
                orloj->showFacetNoteEdit(note);
            }
            return;
        }
    }

    doActionOutlineEdit();
}

void MainWindowPresenter::doActionNoteEdit()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
    ) {
        Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
        if(note) {
            if(withWriteableOutline(note->getOutline()->getKey())) {
                orloj->showFacetNoteEdit(note);
            }
            return;
        }
    }

#ifdef __APPLE__
    doActionOutlineEdit();
#else
    QMessageBox::critical(&view, tr("Edit Note"), tr("Please select a Note to edit in the Notebook."));
#endif
}

void MainWindowPresenter::doActionNoteExternalEdit()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
    ) {
        Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
        if(note) {
            // check whether external editor is configured & open preferences if not
            if(!config.isExternalEditorCmd()) {
                QMessageBox::critical(
                    &view,
                    tr("Edit Note with External Editor Error"),
                    tr("External editor command is not configured in preferences (Editor tab).")
                );

                // let user to configure external editor
                configDialog->show();

                return;
            }

            // export Note to file in temp directory
            string tempFilePath{getNewTempFilePath(File::EXTENSION_MD_MD)};
            string noteStrMd{
                "# " + note->getName() + "\n"
                + note->getDescriptionAsString()
            };
            stringToFile(tempFilePath, noteStrMd);

            // prepare dialog message with command which will be run (file path)
            string cmd{config.getExternalEditorCmd() + " " + tempFilePath};

            // open modal dialog to block MindForger usage
#ifdef MF_MODAL_DIALOG_DOES_NOT_WORK
            // PROBLEM: blocking
            QMessageBox::information(
                &view,
                tr("Edit Note with External Editor"),
                tr("Running command: '%1'").arg(cmd.c_str()),
                QMessageBox::NoButton
            );

            // PROBLEM: black background ~ dialog not rendered
            QDialog* dialog = new AddLibraryDialog(&view);
            Qt::WindowFlags flags = dialog->windowFlags();
            dialog->setWindowFlags(flags | Qt::Tool);
            dialog->show();
            // force processing of all events and refresh
            QCoreApplication::processEvents();
#endif
            this->statusBar->showInfo(
                tr(
                    "Running command: '%1'. Close external editor to return "
                    "control back to MindForger."
                ).arg(cmd.c_str())
            );

            // run external editor
            if(!system(NULL)) {
                string errorMessage{
                    "Error: unable to run external editor as C++ command processor "
                    "is not available"
                };
                MF_DEBUG(errorMessage);
                statusBar->showError(errorMessage);
                QMessageBox::critical(
                    &view,
                    tr("Edit Note with External Editor Error"),
                    tr(errorMessage.c_str())
                );
                return;
            }

            MF_DEBUG("Running external editor: '" << cmd << "'" << endl);
            int statusCode = system(cmd.c_str());
            MF_DEBUG("External editor finished with status: " << statusCode << endl);
            if(statusCode) {
                cerr << "External editor failed with status: " << statusCode << endl;
            }

            // paste text BACK to Note
            if(isFile(tempFilePath.c_str())) {
                vector<string*> description{};
                size_t fileSize{};
                fileToLines(&tempFilePath, description, fileSize);

                // kill the first line if title
                if(description.size()
                   && description[0]
                   && description[0]->size() > 2
                   && description[0]->at(0) == '#'
                   && description[0]->at(1) == ' '
                ) {
                    delete description[0];
                    description.erase(description.begin());
                }

                // update note
                if(description.size()) {
                    note->setDescription(description);
                } else {
                    note->clearDescription();
                }
            }

            // update view
            this->orloj->showFacetNoteView(note);

            // close modal dialog
            statusBar->clear();

            // save updated N
            mind->remember(orloj->getOutlineView()->getCurrentOutline()->getKey());

            return;
        }
    }

    QMessageBox::critical(
        &view,
        tr("Edit Note with External Editor"),
        tr("Please select a Note to edit in the Notebook.")
    );
}

void MainWindowPresenter::doActionNoteHoist()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE))
    {
        config.setUiHoistedMode(getMainMenu()->getView()->actionViewHoist->isChecked());
        orloj->applyFacetHoisting();
    }
}

void MainWindowPresenter::doActionNoteLeave()
{
    orloj->slotShowOutlines();
}

void MainWindowPresenter::doActionNoteForget()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
    ) {
        Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
        if(note) {
            QMessageBox msgBox{
                QMessageBox::Question,
                tr("Delete Note"),
                tr("Do you really want to delete note '") +
                QString::fromStdString(note->getName()) +
                tr("' along with its child notes?")
            };
            QPushButton* yes = msgBox.addButton("&Yes", QMessageBox::YesRole);
#ifdef __APPLE__
            yes->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Y));
            yes->setToolTip("⌘Y");

            QPushButton* no =
#endif
            msgBox.addButton("&No", QMessageBox::NoRole);
#ifdef __APPLE__
            no->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
            no->setToolTip("⌘N");
#endif
            msgBox.exec();

            QAbstractButton* choosen = msgBox.clickedButton();
            if(yes == choosen) {
                // delete selected row & SELECT and adjacent row:
                //   - row is always deleted INCLUDING children
                //   - adjacent row must be identified BEFORE deleting the row
                // consider the following scenarios:
                //   - N is the first row in O
                //     > row 0 w/ chidlren will be deleted and new row 0
                //       will be selected AFTER the delete (if any N remains in O)
                //   - there are Ns above row to be deleted
                //     > simplest scenario - N above N to be deleted will be selected
                //   - N is the last N in O, it has children and it is deleted including children
                //     > no row is selected in this case

                Note* adjacentNote = nullptr;
                if(orloj->getOutlineView()->getOutlineTree()->getModel()->rowCount() > 1) {
                    adjacentNote = orloj->getOutlineView()->getOutlineTree()->getAdjacentNote();
                }

                Outline* outline = mind->noteForget(note);
                mind->remember(outline);
                orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());

                if(orloj->getOutlineView()->getOutlineTree()->getModel()->rowCount()) {
                    if(adjacentNote) {
                        orloj->getOutlineView()->selectRowByNote(adjacentNote);
                    } else {
                        orloj->getOutlineView()->getOutlineTree()->selectRow(0);
                    }
                }
            }
            return;
        }
    }
    QMessageBox::critical(&view, tr("Forget Note"), tr("Please select a Note to forget."));
}

void MainWindowPresenter::doActionNoteExtract()
{
    // TODO distinquish HEADER and NOTE - different places from where to get text
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
    ) {
        // try to get N (might be null if O's header is being edited)
        Note* n = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
        QString selectedText;
        if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
            selectedText = orloj->getOutlineHeaderEdit()->getSelectedText();
        } else {
            selectedText = orloj->getNoteEdit()->getSelectedText();
        }

        if(selectedText.isEmpty()) {
            QMessageBox::critical(&view, tr("Extract Note"), tr("Please select a text to extract."));
        } else {
            int offset = orloj->getOutlineView()->getOutlineTree()->getCurrentRow();
            if(offset == OutlineTreePresenter::NO_ROW) {
                offset = NO_PARENT;
            } else {
                offset++; // extracted N to be sibling below w/ same depth
            }

            static string defaultExtractedNoteName{"Extracted Note"};
            Note* extractedNote = mind->noteNew(
                        orloj->getOutlineView()->getCurrentOutline()->getKey(),
                        offset,
                        &defaultExtractedNoteName,
                        n?n->getType():mind->remind().getOntology().findOrCreateNoteType(NoteType::KeyNote()),
                        n?n->getDepth():0);
            if(extractedNote) {
                // parse selected text to description
                std::vector<std::string*> description{};
                string t{selectedText.toStdString()};
                mdRepresentation->description(&t, description);
                extractedNote->setDescription(description);

                mind->remember(orloj->getOutlineView()->getCurrentOutline()->getKey());
                // IMPROVE smarter refresh of outline tree (do less then overall load)
                orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());
                orloj->showFacetNoteEdit(extractedNote);
            } else {
                QMessageBox::critical(&view, tr("Extract Note"), tr("Failed to extract new Note!"));
            }
        }
    } else {
        QMessageBox::critical(&view, tr("Extract Note"), tr("Please select a Note, edit it and select a text to extract."));
    }
}

void MainWindowPresenter::doActionSpellCheck()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
    ) {
        if(config.isUiEditorLiveSpellCheck()) {
            NoteEditorView* editor = orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)
                ? orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()
                : orloj->getNoteEdit()->getView()->getNoteEditor();
            editor->checkDocumentSpelling();
        }
    }
}

void MainWindowPresenter::doActionNoteClone()
{
    Note* n = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    if(n && withWriteableOutline(n->getOutline()->getKey())) {
        QMessageBox::StandardButton choice;
        choice = QMessageBox::question(
            &view,
            tr("Clone Note"),
            tr("Do you want to clone Note '") + QString::fromStdString(n->getName()) + tr("' including its child notes?'?"),
            QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No | QMessageBox::StandardButton::Cancel
        );

        if(QMessageBox::StandardButton::Cancel != choice) {
            bool deep = choice == QMessageBox::Yes;
            Note* clonedNote = mind->noteClone(orloj->getOutlineView()->getCurrentOutline()->getKey(), n, deep);
            if(clonedNote) {
                mind->remind().remember(orloj->getOutlineView()->getCurrentOutline()->getKey());
                // IMPROVE smarter refresh of outline tree (do less then overall load)
                orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());
                // select Note in the tree
                QModelIndex idx
                    = orloj->getOutlineView()->getOutlineTree()->getView()->model()->index(n->getOutline()->getNoteOffset(clonedNote), 0);
                orloj->getOutlineView()->getOutlineTree()->getView()->setCurrentIndex(idx);
            } else {
                QMessageBox::critical(&view, tr("Clone Note"), tr("Failed to clone Note!"));
            }
        }
    } else {
        QMessageBox::critical(&view, tr("Clone Note"), tr("Please select a Note to be cloned."));
    }
}

void MainWindowPresenter::doActionOutlineShow()
{
    orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());
}

void MainWindowPresenter::selectNoteInOutlineTree(Note* note, Outline::Patch& patch, bool onUp)
{
    QModelIndex idx;
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
        QModelIndex idx;
        if(onUp) {
            idx = orloj->getOutlinesMap()->getModel()->index(patch.start, 0);
        } else {
            idx = orloj->getOutlinesMap()->getModel()->index(note->getOutline()->getNoteOffset(note), 0);
        }

        orloj->getOutlinesMap()->getView()->setCurrentIndex(idx);
    } else {
        QModelIndex idx;
        if(onUp) {
            idx = orloj->getOutlineView()->getOutlineTree()->getView()->model()->index(patch.start, 0);
        } else {
            idx = orloj->getOutlineView()->getOutlineTree()->getView()->model()->index(note->getOutline()->getNoteOffset(note), 0);
        }
        orloj->getOutlineView()->getOutlineTree()->getView()->setCurrentIndex(idx);
    }
}

void MainWindowPresenter::doActionNoteFirst()
{
    Note* note{};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
        note = orloj->getOutlinesMap()->getCurrentNote();
    } else {
        note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    }

    if(note && withWriteableOutline(note->getOutline()->getKey())) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->noteFirst(note, &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                mind->outlinesMapRemember();
            } else {
                mind->remind().remember(note->getOutline());
            }

            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                orloj->getOutlinesMap()->refresh(note->getOutline(), &patch);
            } else {
                orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
            }

            // select Note in the tree
            this->selectNoteInOutlineTree(note, patch, true);
            statusBar->showInfo(QString(tr("Moved Note '%1' to be the first child")).arg(note->getName().c_str()));
        }
    } else {
        QMessageBox::critical(&view, tr("Move Note"), tr("Please select a Note to be moved."));
    }
}

void MainWindowPresenter::doActionNoteUp()
{
    Note* note{};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
        note = orloj->getOutlinesMap()->getCurrentNote();
    } else {
        note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    }

    if(note && withWriteableOutline(note->getOutline()->getKey())) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->noteUp(note, &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                mind->outlinesMapRemember();
            } else {
                mind->remind().remember(note->getOutline());
            }

            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                orloj->getOutlinesMap()->refresh(note->getOutline(), &patch);
            } else {
                orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
            }

            // select Note in the tree
            this->selectNoteInOutlineTree(note, patch, true);
            statusBar->showInfo(QString(tr("Moved up Note '%1'")).arg(note->getName().c_str()));
        }
    } else {
        QMessageBox::critical(&view, tr("Move Note"), tr("Please select a Note to be moved."));
    }
}

void MainWindowPresenter::doActionNoteDown()
{
    Note* note{};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
        note = orloj->getOutlinesMap()->getCurrentNote();
    } else {
        note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    }

    if(note && withWriteableOutline(note->getOutline()->getKey())) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->noteDown(note, &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                mind->outlinesMapRemember();
            } else {
                mind->remind().remember(note->getOutline());
            }

            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                orloj->getOutlinesMap()->refresh(note->getOutline(), &patch);
            } else {
                orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
            }

            // select Note in the tree
            this->selectNoteInOutlineTree(note, patch, false);
            statusBar->showInfo(QString(tr("Moved down Note '%1'").arg(note->getName().c_str())));
        }
    } else {
        QMessageBox::critical(&view, tr("Move Note"), tr("Please select a Note to be moved."));
    }
}

void MainWindowPresenter::doActionNoteLast()
{
    Note* note{};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
        note = orloj->getOutlinesMap()->getCurrentNote();
    } else {
        note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    }

    if(note && withWriteableOutline(note->getOutline()->getKey())) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->noteLast(note, &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                mind->outlinesMapRemember();
            } else {
                mind->remind().remember(note->getOutline());
            }

            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                orloj->getOutlinesMap()->refresh(note->getOutline(), &patch);
            } else {
                orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
            }

            // select Note in the tree
            this->selectNoteInOutlineTree(note, patch, false);
            statusBar->showInfo(QString(tr("Moved Note '%1' to be the last child")).arg(note->getName().c_str()));
        }
    } else {
        QMessageBox::critical(&view, tr("Move Note"), tr("Please select a Note to be moved."));
    }
}

void MainWindowPresenter::doActionNotePromote()
{
    Note* note{};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
        note = orloj->getOutlinesMap()->getCurrentNote();
    } else {
        note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    }

    if(note && withWriteableOutline(note->getOutline()->getKey())) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->notePromote(note, &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                mind->outlinesMapRemember();
            } else {
                mind->remind().remember(note->getOutline());
            }

            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                orloj->getOutlinesMap()->refresh(note->getOutline(), &patch);
            } else {
                orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
            }

            statusBar->showInfo(QString(tr("Promoted Note '%1'")).arg(note->getName().c_str()));
        }
    } else {
        QMessageBox::critical(&view, tr("Promote Note"), tr("Please select a Note to be promoted."));
    }
}

void MainWindowPresenter::doActionNoteDemote()
{
    Note* note{};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
        note = orloj->getOutlinesMap()->getCurrentNote();
    } else {
        note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    }

    if(note && withWriteableOutline(note->getOutline()->getKey())) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->noteDemote(note, &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                mind->outlinesMapRemember();
            } else {
                mind->remind().remember(note->getOutline());
            }

            if(orloj->isFacetActive(OrlojPresenterFacets::FACET_MAP_OUTLINES)) {
                orloj->getOutlinesMap()->refresh(note->getOutline(), &patch);
            } else {
                orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
            }

            statusBar->showInfo(QString(tr("Demoted Note '%1'")).arg(note->getName().c_str()));
        }
    } else {
        QMessageBox::critical(&view, tr("Demote Note"), tr("Please select a Note to be demoted."));
    }
}

void MainWindowPresenter::doActionEditFind()
{
    doActionFts();
}

void MainWindowPresenter::doActionEditFindAgain()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->findStringAgain();
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->findStringAgain();
    }
}

void MainWindowPresenter::doActionEditWordWrapToggle()
{
    NoteEditorView* editor{};
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        editor = orloj->getNoteEdit()->getView()->getNoteEditor();
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        editor = orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor();
    } else {
        return;
    }

    if(editor->wordWrapMode() == QTextOption::NoWrap) {
        editor->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    } else {
        editor->setWordWrapMode(QTextOption::NoWrap);
    }
}

void MainWindowPresenter::doActionMindRemember()
{
    mdConfigRepresentation->save(config);
}

void MainWindowPresenter::doActionMindSnapshot()
{
}

void MainWindowPresenter::doActionMindTimeTagScope()
{
    TimeScopeAspect& time = mind->getTimeScopeAspect();
    scopeDialog->show(
        mind->getTagsScopeAspect().getTags(),
        time.isEnabled(),
        time.getTimeScope().years,
        time.getTimeScope().months,
        time.getTimeScope().days,
        time.getTimeScope().hours,
        time.getTimeScope().minutes);
}

void MainWindowPresenter::handleMindScope()
{
    // time scope
    TimeScope& ts=mind->getTimeScopeAspect().getTimeScope();
    if(scopeDialog->isTimeScopeSet()) {
        ts.years=scopeDialog->getYears();
        ts.months=scopeDialog->getMonths();
        ts.days=scopeDialog->getDays();
        ts.hours=scopeDialog->getHours();
        ts.minutes=scopeDialog->getMinutes();

        ts.recalculateRelativeSecs();
    } else {
        ts.reset();
    }
    // update components
    mind->getTimeScopeAspect().setTimeScope(ts);
    config.setTimeScope(mind->getTimeScopeAspect().getTimeScope());

    // tags scope
    if(scopeDialog->isTagsScopeSet() && scopeDialog->getTags().size()) {
        mind->getTagsScopeAspect().setTags(scopeDialog->getTags());
    } else {
        mind->getTagsScopeAspect().reset();
    }
    config.setTagsScope(mind->getTagsScopeAspect().getTags());

    // save configuration
    mdConfigRepresentation->save(config);

    // IMPROVE don't change view to Os, but refresh current one
    doActionViewOutlines();
}

void MainWindowPresenter::doActionMindPreferences()
{
    configDialog->show();
}

void MainWindowPresenter::handleMindPreferences()
{
    mdConfigRepresentation->save(config);

    view.getToolBar()->setVisible(config.isUiShowToolbar());
    view.getOrloj()->getNoteView()->setZoomFactor(config.getUiHtmlZoomFactor());
    view.getOrloj()->getOutlineHeaderView()->setZoomFactor(config.getUiHtmlZoomFactor());

    view.getOrloj()->getNoteView()->getButtonsPanel()->setExpertMode(config.isUiExpertMode());
    view.getOrloj()->getNoteView()->getButtonsPanel()->setVisible(!config.isUiExpertMode());
    view.getOrloj()->getOutlineHeaderView()->getEditPanel()->setExpertMode(config.isUiExpertMode());
    view.getOrloj()->getOutlineHeaderView()->getEditPanel()->setVisible(!config.isUiExpertMode());

    view.getOrloj()->getNoteEdit()->getButtonsPanel()->setVisible(!config.isUiExpertMode());
    view.getOrloj()->getOutlineHeaderEdit()->getButtonsPanel()->setVisible(!config.isUiExpertMode());

    // IMPROVE: highlighter should NOT reference lib configuration to honor MVP, spell check
    // setting to be pushed to highlighter from here
}

void MainWindowPresenter::doActionViewTerminal()
{
    terminalDialog->show();
}

void MainWindowPresenter::doActionLibraryNew()
{
    newLibraryDialog->show();
}

void MainWindowPresenter::handleNewLibrary()
{
    // check that filesystem path exists
    string uri{newLibraryDialog->getLibraryUriText().toStdString()};
    if(!isDirectory(uri.c_str())) {
        QMessageBox::critical(
            &view,
            tr("Add Library Error"),
            tr("Library directory doesn't exist!")
        );
        return;
    }

    newLibraryDialog->hide();

    // index library documents
    FilesystemInformationSource informationSource{
        uri,
        *orloj->getMind(),
        *mdDocumentRepresentation,
    };

    FilesystemInformationSource::ErrorCode code = informationSource.indexToMemory(
        *config.getActiveRepository()
    );
    if(FilesystemInformationSource::ErrorCode::LIBRARY_ALREADY_EXISTS == code) {
        QMessageBox::critical(
            &view,
            tr("Add Library Error"),
            tr("Library already indexed - use 'Update library' action to synchronize documents.")
        );
        return;
    }
    if(FilesystemInformationSource::ErrorCode::SUCCESS != code) {
        QMessageBox::critical(
            &view,
            tr("Add Library Error"),
            tr("Unable to index documents on library path - either memory directory "
               "doesn't exist or not in MindForger workspace mode."
            )
        );
        return;
    }

    // TODO Library menu enabled in case of MF/repository only

    MF_DEBUG("New Library: " << uri << " has been added to MindForger" << endl);

    // show Os view
    orloj->showFacetOutlineList(mind->getOutlines());
}

void MainWindowPresenter::doActionLibrarySync()
{
    syncLibraryDialog->reset();

    // determine existing library dirs
    vector<FilesystemInformationSource*> srcs
        = FilesystemInformationSource::findInformationSources(
            config,
            *orloj->getMind(),
            *mdDocumentRepresentation);

    if(srcs.size()) {
        for(FilesystemInformationSource* src:srcs) {
            syncLibraryDialog->addLibraryToSync(src->getPath());

            delete src;
        }

        syncLibraryDialog->show();
    } else {
        QMessageBox::information(
            &view,
            tr("Library synchronization"),
            tr("There are no libraries - nothing to synchronize.")
        );
    }
}

void MainWindowPresenter::handleSyncLibrary()
{
    syncLibraryDialog->hide();

    string librarySrcDir
        = syncLibraryDialog->getLibraryPathsCombo()->currentText().toStdString();

    FilesystemInformationSource informationSource{
        librarySrcDir,
        *orloj->getMind(),
        *mdDocumentRepresentation,
    };

    informationSource.indexToMemory(*config.getActiveRepository(), true);

    rmLibraryDialog->reset();
}


void MainWindowPresenter::doActionLibraryRm()
{
    rmLibraryDialog->reset();

    // determine existing library dirs
    vector<FilesystemInformationSource*> srcs
        = FilesystemInformationSource::findInformationSources(
            config,
            *orloj->getMind(),
            *mdDocumentRepresentation);

    if(srcs.size()) {
        for(FilesystemInformationSource* src:srcs) {
            rmLibraryDialog->addLibraryToRemove(src);
        }

        rmLibraryDialog->show();
    } else {
        QMessageBox::information(
            &view,
            tr("Library deletion"),
            tr("There are no libraries - nothing to delete.")
        );
    }
}

void MainWindowPresenter::handleRmLibrary()
{
    rmLibraryDialog->hide();

    string librarySrcDir
        = rmLibraryDialog->getLibraryPathsCombo()->currentText().toStdString();

    // confirm removal of MF directory
    QMessageBox msgBox{
        QMessageBox::Question,
        tr("Delete Library"),
        tr("Do you really want to delete Notebooks which represent the library documents?")
    };
    QPushButton* yes = msgBox.addButton("&Yes", QMessageBox::YesRole);
#ifdef __APPLE__
    yes->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Y));
    yes->setToolTip("⌘Y");

    QPushButton* no =
#endif
    msgBox.addButton("&No", QMessageBox::NoRole);
#ifdef __APPLE__
    no->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    no->setToolTip("⌘N");
#endif
    msgBox.exec();

    QAbstractButton* choosen = msgBox.clickedButton();
    if(yes == choosen) {
        // purge library directory
        string mfLibraryPath{rmLibraryDialog->getLibraryMfPathForLocator(librarySrcDir)};
        removeDirectoryRecursively(mfLibraryPath.c_str());

        // reload the whole workspace
        doActionMindRelearn(
            QString::fromStdString(config.getActiveRepository()->getPath())
        );
    }

    rmLibraryDialog->reset();
}

void MainWindowPresenter::doActionOrganizerNew()
{
    newOrganizerDialog->show(mind->getOutlines());
}

void MainWindowPresenter::handleCreateOrganizer()
{
    static vector<Note*> organizerOutlinesAndNotes{};
    organizerOutlinesAndNotes.clear();
    static vector<Note*> organizerNotes{};
    organizerNotes.clear();

    Organizer* o{nullptr};
    if(newOrganizerDialog->getOrganizerToEdit()) {
        MF_DEBUG("Updating organizer...");
        o = newOrganizerDialog->getOrganizerToEdit();
        o->setName(newOrganizerDialog->getOrganizerName().toStdString());
    } else {
        MF_DEBUG("Creating organizer...");
        if(Organizer::OrganizerType::EISENHOWER_MATRIX == newOrganizerDialog->getOrganizerType()) {
            o = new EisenhowerMatrix(
                newOrganizerDialog->getOrganizerName().toStdString()
            );
            o->setKey(EisenhowerMatrix::createEisenhowerMatrixKey());
        } else {
            o = new Kanban(newOrganizerDialog->getOrganizerName().toStdString());
            o->setKey(Kanban::createKanbanKey());
        }
    }

    // tags
    o->tagsUlQuadrant.clear();
    o->tagsLlQuadrant.clear();
    o->tagsUrQuadrant.clear();
    o->tagsLrQuadrant.clear();

    o->setUpperLeftTags(
        newOrganizerDialog->getUpperLeftChosenTags(
            o->tagsUlQuadrant
        )
    );
    o->setUpperRightTags(
        newOrganizerDialog->getUpperRightChosenTags(
            o->tagsUrQuadrant
        )
    );
    o->setLowerLeftTags(
        newOrganizerDialog->getLowerLeftChosenTags(
            o->tagsLlQuadrant
        )
    );
    o->setLowerRightTags(
        newOrganizerDialog->getLowerRightChosenTags(
            o->tagsLrQuadrant
        )
    );
    // filter
    o->setFilterBy(newOrganizerDialog->getFilterBy());
    // sort
    if(Organizer::OrganizerType::EISENHOWER_MATRIX == newOrganizerDialog->getOrganizerType()) {
        dynamic_cast<EisenhowerMatrix*>(o)->setSortBy(newOrganizerDialog->getSortBy());
    }
    // scope
    o->setOutlineScope(newOrganizerDialog->getOutlineScope());

    // add organizer & save configuration
    if(!newOrganizerDialog->getOrganizerToEdit()) {
        config.getRepositoryConfiguration().addOrganizer(o);
    }
    sortAndSaveOrganizersConfig();

    newOrganizerDialog->hide();

    // refresh organizer table view
    if(!newOrganizerDialog->getOrganizerToEdit()) {
        orloj->showFacetOrganizerList(config.getRepositoryConfiguration().getOrganizers());
    } else {
        if(Organizer::OrganizerType::KANBAN == newOrganizerDialog->getOrganizerToEdit()->getOrganizerType()) {
            orloj->showFacetKanban(
                static_cast<Kanban*>(o),
                mind->getAllNotes(organizerOutlinesAndNotes, true, true),
                mind->getOutlines(),
                mind->getAllNotes(organizerNotes, true, false)
            );
        } else {
            orloj->showFacetEisenhowerMatrix(
                o,
                mind->getAllNotes(organizerOutlinesAndNotes, true, true),
                mind->getOutlines(),
                mind->getAllNotes(organizerNotes, true, false)
            );
        }
    }
}

void MainWindowPresenter::doActionOrganizerEdit()
{
    // no need to check view - this action is available only when organizer is opened
    Organizer* o{nullptr};
    if(OrlojPresenterFacets::FACET_KANBAN == orloj->getFacet()) {
        o = orloj->getKanban()->getKanban();
    } else {
        o = orloj->getOrganizer()->getOrganizer();
    }

    // Eisenhower matrix organizer cannot be edited
    if(o->getKey() == EisenhowerMatrix::KEY_EISENHOWER_MATRIX) {
        QMessageBox::critical(
            orloj->getOrganizer()->getView(),
            tr("Organizer Update Error"),
            tr("Eisenhower Matrix organizer is built-in and cannot be edited - please create or update a custom organizer.")
        );
        return;
    }

    // lazy lookup of O scope
    Outline* oScopeOutline{nullptr};
    if(o->getOutlineScope().size()) {
        oScopeOutline=mind->remind().getOutline(o->getOutlineScope());
        // ensures that if O is deleted, it will be detected
        if(!oScopeOutline) {
            o->clearOutlineScope();
            mdConfigRepresentation->save(config);
        }
    }
    newOrganizerDialog->show(mind->getOutlines(), nullptr, o, oScopeOutline);
}

void MainWindowPresenter::doActionOrganizerClone()
{
    // no need to check view - this action is available only when organizer is opened
    Organizer* o = orloj->getOrganizer()->getOrganizer();

    // Eisenhower matrix organizer cannot be cloned
    if(o->getKey() == EisenhowerMatrix::KEY_EISENHOWER_MATRIX) {
        QMessageBox::critical(
            orloj->getOrganizer()->getView(),
            tr("Organizer Clone Error"),
            tr("Eisenhower Matrix organizer is built-in and cannot be cloned - please create or update a custom organizer.")
        );
        return;
    }

    Organizer* oClone = new Organizer{*o};
    o->setName(o->getName()+" Clone");

    config.getRepositoryConfiguration().addOrganizer(oClone);
    getConfigRepresentation()->save(config);
}

void MainWindowPresenter::doActionOrganizerFocusToNextVisibleQuadrant()
{
    if(OrlojPresenterFacets::FACET_KANBAN == orloj->getFacet()) {
        orloj->getKanban()->focusToNextVisibleColumn();
    } else {
        orloj->getOrganizer()->focusToNextVisibleQuadrant();
    }
}

void MainWindowPresenter::doActionOrganizerFocusToPreviousVisibleQuadrant()
{
    if(OrlojPresenterFacets::FACET_KANBAN == orloj->getFacet()) {
        orloj->getKanban()->focusToPreviousVisibleColumn();
    } else {
        orloj->getOrganizer()->focusToPreviousVisibleQuadrant();
    }
}

void doActionOrganizerMoveNoteCommon(
    Note* note,
    OrganizerQuadrantPresenter* presenter,
    OrlojPresenter* orloj
) {
    static vector<Note*> organizerOutlinesAndNotes{};
    static vector<Note*> organizerNotes{};

    if(presenter) {
        // persist modified N
        orloj->getMind()->remember(note->getOutlineKey());

        // refresh view
        organizerOutlinesAndNotes.clear();
        organizerNotes.clear();
        orloj->getOrganizer()->refresh(
            orloj->getOrganizer()->getOrganizer(),
            orloj->getMind()->getAllNotes(organizerOutlinesAndNotes, true, true),
            orloj->getMind()->getOutlines(),
            orloj->getMind()->getAllNotes(organizerNotes, true, false),
            false
        );

        // give target N column focus
        presenter->getView()->setFocus();
        presenter->getView()->setCurrentIndex(
            presenter->getView()->model()->index(0, 0)
        );
    }
}

void doActionKanbanMoveNoteCommon(
    Note* note,
    KanbanColumnPresenter* presenter,
    OrlojPresenter* orloj
) {
    static vector<Note*> organizerOutlinesAndNotes{};
    static vector<Note*> organizerNotes{};

    if(presenter) {
        // persist modified N
        orloj->getMind()->remember(note->getOutlineKey());

        // refresh view
        organizerOutlinesAndNotes.clear();
        organizerNotes.clear();
        orloj->getKanban()->refresh(
            orloj->getKanban()->getKanban(),
            orloj->getMind()->getAllNotes(organizerOutlinesAndNotes, true, true),
            orloj->getMind()->getOutlines(),
            orloj->getMind()->getAllNotes(organizerNotes, true, false),
            false
        );

        // give target N column focus
        presenter->getView()->setFocus();
        presenter->getView()->setCurrentIndex(
            presenter->getView()->model()->index(0, 0)
        );
    }
}

void MainWindowPresenter::doActionOrganizerMoveNoteToNextVisibleQuadrant(Note* note)
{
    if(OrlojPresenterFacets::FACET_KANBAN == orloj->getFacet()) {
        doActionKanbanMoveNoteCommon(
            note,
            orloj->getKanban()->moveToNextVisibleColumn(note),
            orloj
        );
    } else {
        if(!EisenhowerMatrix::isEisenhowMatrixOrganizer(
                orloj->getOrganizer()->getOrganizer()
           )
         ) {
            doActionOrganizerMoveNoteCommon(
                note,
                orloj->getOrganizer()->moveToNextVisibleQuadrant(note),
                orloj
            );
        } else {
            statusBar->showError(
                "Notebooks/notes cannot be moved around quadrants of "
                "Eisenhower Matrix"
            );
        }
    }
}

void MainWindowPresenter::doActionOrganizerMoveNoteToPreviousVisibleQuadrant(Note* note)
{
    if(OrlojPresenterFacets::FACET_KANBAN == orloj->getFacet()) {
        doActionKanbanMoveNoteCommon(
            note,
            orloj->getKanban()->moveToPreviousVisibleColumn(note),
            orloj
        );
    } else {
        if(!EisenhowerMatrix::isEisenhowMatrixOrganizer(orloj->getOrganizer()->getOrganizer())) {
            doActionOrganizerMoveNoteCommon(
                note,
                orloj->getOrganizer()->moveToPreviousVisibleQuadrant(note),
                orloj
            );
        } else {
            statusBar->showError("Notebooks/notes cannot be moved around quadrants of Eisenhower Matrix");
        }
    }
}

void MainWindowPresenter::doActionOrganizerForget()
{
    Organizer* o{nullptr};

    // no need to check view - this action is available only when organizer is opened
    if(OrlojPresenterFacets::FACET_KANBAN == orloj->getFacet()) {
        o = orloj->getKanban()->getKanban();
    } else {
        o = orloj->getOrganizer()->getOrganizer();
    }

    if(o->getKey() != EisenhowerMatrix::KEY_EISENHOWER_MATRIX) {
        QMessageBox::StandardButton choice;
        choice = QMessageBox::question(
            &view,
            tr("Forget Organizer"),
            tr("Do you really want to forget '")
                + QString::fromStdString(o->getName()) + tr("' Organizer?")
        );
        if (choice == QMessageBox::Yes) {
            config.getRepositoryConfiguration().removeOrganizer(o);
            getConfigRepresentation()->save(config);
            orloj->showFacetOrganizerList(
                config.getRepositoryConfiguration().getOrganizers());
        } // else do nothing
    } else {
        QMessageBox::critical(
            &view,
            tr("Delete Organizer"),
            tr(
                "Eisenhower Matrix is built-in and cannot be deleted - only custom "
                "organizers can.")
        );
    }
}


void MainWindowPresenter::doActionViewLimbo()
{
    if(config.getActiveRepository()->getMode()
           ==Repository::RepositoryMode::REPOSITORY
       && config.getActiveRepository()->getType()
           ==Repository::RepositoryType::MINDFORGER
    ) {
        QString path{"file://"};
        path.append(config.getLimboPath().c_str());
        QDesktopServices::openUrl(QUrl{path});
    } else {
        QMessageBox::information(
            &view,
            tr("View Limbo"),
            tr(
                "Limbo directory with deleted Notebooks is available in "
                "the MindForger workspace, not if a Markdown is edited or a directory "
                "with markdowns is opened.")
        );
    }
}

void MainWindowPresenter::doActionHelpDocumentation()
{
    QDesktopServices::openUrl(QUrl{"https://github.com/dvorka/mindforger/wiki"});
}

void MainWindowPresenter::doActionHelpWeb()
{
    QDesktopServices::openUrl(
        QUrl{"http://www.mindforger.com"}
    );
}

void MainWindowPresenter::doActionHelpMarkdown()
{
    QDesktopServices::openUrl(
        QUrl{"https://guides.github.com/features/mastering-markdown/"}
    );
}

void MainWindowPresenter::doActionHelpDiagrams()
{
    QDesktopServices::openUrl(
        QUrl{"https://mermaid-js.github.io/mermaid/#/"}
    );
}

void MainWindowPresenter::doActionHelpMathLivePreview()
{
    QDesktopServices::openUrl(
        QUrl{"https://www.mathjax.org/#demo"}
    );
}

void MainWindowPresenter::doActionHelpMathQuickReference()
{
    QDesktopServices::openUrl(
        QUrl{
            "https://math.meta.stackexchange.com/questions/5020"
            "/mathjax-basic-tutorial-and-quick-reference"}
    );
}

void MainWindowPresenter::doActionHelpReportBug()
{
    QDesktopServices::openUrl(
        QUrl{"https://github.com/dvorka/mindforger/issues"}
    );
}

void MainWindowPresenter::doActionHelpCheckForUpdates()
{
    QDesktopServices::openUrl(
        QUrl{"https://github.com/dvorka/mindforger/releases"}
    );
}

void MainWindowPresenter::doActionEmojisDialog()
{
    // IMPROVE load emojis from the main MF configuration file
    QMessageBox::information(
        &view,
        QString{tr("Emojis")},
        QString{
            "<html>"
            "Copy character from below and paste it to the text:"
            "<br>"
            "<br>Emoji:"
            "<br>🐞 🎉 ⛑ ❗ ❌"
            "<br>🔴 🔵 🚫 🎯 ⚽"
            "<br>🙂 😃 🥶 🥰 🐻 🤖 💩 👻 ☠️"
            "<br>🦑 🐙 👾"
            "<br>💪 👍 🤞 🤙 👌 🙏 🤦"
            "<br>🛠 🔧"
            "<br>📡 🌊 🚀 🎖🍔"
            "<br>📣 📢 🏁 🚧 🚩 💣 💯 🖼️"
            "<br>"
            "<br>Greek alphabet:"
            "<br>Α α, Β β, Γ γ, Δ δ, Ε ε,"
            "<br>Ζ ζ, Η η, Θ θ, Ι ι, Κ κ,"
            "<br>Λ λ, Μ μ, Ν ν, Ξ ξ, Ο ο,"
            "<br>Π π, Ρ ρ, Σ σ/ς, Τ τ, Υ υ,"
            "<br>Φ φ, Χ χ, Ψ ψ, Ω ω"
            "<br>"
            "<br>Special:"
            "<br>✔ ♥ ⦀"
            "<br>"
            "<br>Math and statistics:"
            "<br>x̄"
            "<br>"
            "<br>Physics:"
            "<br>°"
            "<br>"
            "<br>More special unicode characters:"
            "<ul>"
            "<li><a href='https://unicode-table.com/en/'>Unicode Table</a></li>"
            "<li><a href='https://emojipedia.org/'>Emojipedia</a></li>"
            "</ul>"
        });
}


void MainWindowPresenter::doActionHelpAboutMindForger()
{
    // IMPROVE move this to view: remove this method and route signal to MainWindowView
    QMessageBox::about(
        &view,
        QString{tr("About MindForger")},
        QString{
            "<b>MindForger " MINDFORGER_VERSION "</b>"
#ifdef DO_MF_DEBUG
            "&nbsp;&nbsp;&nbsp;&nbsp; Qt " QT_VERSION_STR
#endif
            "<br>"
            "<br>Personal thinking notebook."
            "<br>"
            "<br>MindForger is licensed under the <a href='https://www.gnu.org/licenses/gpl-2.0.html'>GNU GPLv2</a> or later. "
            "See also <a href='https://github.com/dvorka/mindforger/licenses'>licenses</a> directory "
            "for 3rd party content licensing."
            "<br>"
            "<br>MindForger is developed as a free and open source project on <a href='https://github.com/dvorka/mindforger'>github.com/dvorka/mindforger</a>"
            "<br>"
            "<br>MindForger is built with passion for my personal pleasure."
            "<br>"
            "<br>Contact me at <a href='mailto:martin.dvorak@mindforger.com'>&lt;martin.dvorak@mindforger.com&gt;</a>"
            " or see <a href='https://www.mindforger.com'>www.mindforger.com</a> for more information."
            "<br>"
            "<br>Copyright (C) 2016-2024 <a href='http://me.mindforger.com'>Martin Dvorak</a> and <a href='https://github.com/dvorka/mindforger/blob/master/CREDITS.md'>contributors</a>."
        });
}

} // m8r namespace
