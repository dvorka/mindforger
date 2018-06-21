/*
 main_window_presenter.cpp     MindForger thinking notebook

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
#include "main_window_presenter.h"

using namespace std;

namespace m8r {

MainWindowPresenter::MainWindowPresenter(MainWindowView& view)
    : view(view),
      config(Configuration::getInstance())
{
    mind = new Mind{config};

    // representations
    this->mdRepresentation
        = new MarkdownOutlineRepresentation{mind->ontology()};
    this->htmlRepresentation
        = new HtmlOutlineRepresentation{mind->ontology(),LookAndFeels::getInstance()};
    this->mdConfigRepresentation
        = new MarkdownConfigurationRepresentation{};

    // assemble presenters w/ UI
    statusBar = new StatusBarPresenter{view.getStatusBar(), mind};
    mainMenu = new MainMenuPresenter{this};
    cli = new CliAndBreadcrumbsPresenter{this, view.getCli(), mind};
    orloj = new OrlojPresenter{this, view.getOrloj(), mind};

    // initialize components
    scopeDialog = new ScopeDialog{mind->ontology(), &view};
    newOutlineDialog = new OutlineNewDialog{QString::fromStdString(config.getMemoryPath()), mind->remind().getOntology(), &view};
    newNoteDialog = new NoteNewDialog{mind->remind().getOntology(), &view};
    ftsDialog = new FtsDialog{&view};
    findOutlineByNameDialog = new FindOutlineByNameDialog{&view};
    findNoteByNameDialog = new FindNoteByNameDialog{&view};
    findOutlineByTagDialog = new FindOutlineByTagDialog{mind->remind().getOntology(), &view};
    findNoteByTagDialog = new FindNoteByTagDialog{mind->remind().getOntology(), &view};
    refactorNoteToOutlineDialog = new RefactorNoteToOutlineDialog{&view};
    configDialog = new ConfigurationDialog{&view};
    insertImageDialog = new InsertImageDialog{&view};
    insertLinkDialog = new InsertLinkDialog{&view};
    rowsAndDepthDialog = new RowsAndDepthDialog(&view);
    newRepositoryDialog = new NewRepositoryDialog(&view);
    newFileDialog = new NewFileDialog(&view);
#ifdef MF_NER
    nerChooseTagsDialog = new NerChooseTagTypesDialog(&view);
    nerResultDialog = new NerResultDialog(&view);
#endif

    // wire signals
    QObject::connect(scopeDialog->getSetButton(), SIGNAL(clicked()), this, SLOT(handleMindScope()));
    QObject::connect(newOutlineDialog, SIGNAL(accepted()), this, SLOT(handleOutlineNew()));
    QObject::connect(newNoteDialog, SIGNAL(accepted()), this, SLOT(handleNoteNew()));
    QObject::connect(ftsDialog->getFindButton(), SIGNAL(clicked()), this, SLOT(handleFts()));
    QObject::connect(findOutlineByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindOutlineByName()));
    QObject::connect(findNoteByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindNoteByName()));
    QObject::connect(findOutlineByTagDialog, SIGNAL(searchFinished()), this, SLOT(handleFindOutlineByTag()));
    QObject::connect(findNoteByTagDialog, SIGNAL(searchFinished()), this, SLOT(handleFindNoteByTag()));
    QObject::connect(refactorNoteToOutlineDialog, SIGNAL(searchFinished()), this, SLOT(handleRefactorNoteToOutline()));
    QObject::connect(configDialog, SIGNAL(saveConfigSignal()), this, SLOT(handleMindPreferences()));
    QObject::connect(insertImageDialog->getInsertButton(), SIGNAL(clicked()), this, SLOT(handleFormatImage()));
    QObject::connect(insertLinkDialog->getInsertButton(), SIGNAL(clicked()), this, SLOT(handleFormatLink()));
    QObject::connect(rowsAndDepthDialog->getGenerateButton(), SIGNAL(clicked()), this, SLOT(handleRowsAndDepth()));
    QObject::connect(newRepositoryDialog->getNewButton(), SIGNAL(clicked()), this, SLOT(handleMindNewRepository()));
    QObject::connect(newFileDialog->getNewButton(), SIGNAL(clicked()), this, SLOT(handleMindNewFile()));
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
    if(ftsDialog) delete ftsDialog;
    if(findOutlineByNameDialog) delete findOutlineByNameDialog;
    if(findNoteByNameDialog) delete findNoteByNameDialog;
    if(findOutlineByTagDialog) delete findOutlineByTagDialog;
    if(configDialog) delete configDialog;
    //if(findNoteByNameDialog) delete findNoteByNameDialog;
    if(insertImageDialog) delete insertImageDialog;

    // TODO deletes
}

void MainWindowPresenter::showInitialView()
{
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
                if(!doActionViewHome()) {
                    // fallback
                    view.getCli()->setBreadcrumbPath("/outlines");
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

    // move Mind to configured state
    if(config.getDesiredMindState()==Configuration::MindState::THINKING) {
        MF_DEBUG("InitialView: asking Mind to THINK..." << endl);
        shared_future<bool> f = mind->think(); // move
        if(f.wait_for(chrono::microseconds(0)) == future_status::ready) {
            if(!f.get()) {
                mainMenu->showFacetMindSleep();
                statusBar->showError(tr("Cannot think - either Mind already dreaming or repository too big"));
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

/* Link handling hints
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
        if(url.toString().startsWith("file://")) {
            string key{url.toString().toStdString()};
            key.erase(0,7); // remove file prefix
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
                            orloj->showFacetNoteView(n);
                            return;
                        } else {
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
            // launch URL in browser
            QDesktopServices::openUrl(url);
        }
    }
}

#ifdef DO_MF_DEBUG
void MainWindowPresenter::doActionMindHack()
{
    MF_DEBUG("[MindHack] Current facet: " << orloj->getFacet() << endl);
}
#endif

void MainWindowPresenter::doActionMindNewRepository()
{
    newRepositoryDialog->show();
}

void MainWindowPresenter::handleMindNewRepository()
{
    // if directory exists, then fail
    if(isDirectoryOrFileExists(newRepositoryDialog->getRepositoryPath().toStdString().c_str())) {
        QMessageBox::critical(&view, tr("New Repository Error"), tr("Specified repository path already exists!"));
        return;
    }

    // create repository
    if(!config.getInstaller()->createEmptyMindForgerRepository(newRepositoryDialog->getRepositoryPath().toStdString())) {
        QMessageBox::critical(&view, tr("New Repository Error"), tr("Failed to create empty repository!"));
        return;
    }

    // copy doc and stencils
    if(!config.getInstaller()->initMindForgerRepository(
        newRepositoryDialog->isCopyDoc(),
        newRepositoryDialog->isCopyStencils(),
        newRepositoryDialog->getRepositoryPath().toStdString().c_str()
    )) {
        statusBar->showError(tr("ERROR: repository created, but attempt to copy documentation and/or stencils failed"));
    }

    // open new repository
    doActionMindRelearn(newRepositoryDialog->getRepositoryPath());
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
}

void MainWindowPresenter::doActionMindLearnRepository()
{
    QString homeDirectory
        = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);

    QFileDialog learnDialog{&view};
    learnDialog.setWindowTitle(tr("Learn Directory"));
    // learnDialog.setFileMode(QFileDialog::Directory|QFileDialog::ExistingFiles); not supported, therefore
    // >
    // ASK user: directory/repository or file (choice) > open dialog configured as required
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
    learnDialog.setWindowTitle(tr("Learn File"));
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
        config.setActiveRepository(config.addRepository(r));
        // remember new repository
        mdConfigRepresentation->save(config);
        // learn and show
        mind->learn();
        showInitialView();
    } else {
        QMessageBox::critical(
            &view,
            tr("Learn"),
            tr("This is neither valid MindForger/Markdown repository nor file."));
    }
}

void MainWindowPresenter::doActionExit()
{
    QApplication::quit();
}

void MainWindowPresenter::doActionFts()
{
    if(orloj->isFacetActiveOutlineManagement()) {
        ftsDialog->setWindowTitle(tr("Full-text Search in Notebook"));
        ftsDialog->setScope(orloj->getOutlineView()->getCurrentOutline());
    } else {
        ftsDialog->setWindowTitle(tr("Full-text Search"));
        ftsDialog->clearScope();
    }
    ftsDialog->show();
}

void MainWindowPresenter::handleFts()
{
    QString searchedString = ftsDialog->getSearchedString();
    ftsDialog->hide();
    executeFts(
        searchedString.toStdString(),
        ftsDialog->getCaseCheckbox()->isChecked(),
        ftsDialog->getScope());
}

void MainWindowPresenter::executeFts(const string& searchedString, const bool ignoreCase, Outline* scope) const
{
    vector<Note*>* result = mind->findNoteFts(searchedString, ignoreCase, scope);

    QString info = QString::number(result->size());
    info += QString::fromUtf8(" result(s) found for '");
    info += QString::fromStdString(searchedString);
    info += QString::fromUtf8("'");
    view.getStatusBar()->showInfo(info);

    if(result && result->size()) {
        orloj->getNoteView()->setSearchExpression(searchedString);
        orloj->getNoteView()->setSearchIgnoreCase(ignoreCase);

        orloj->showFacetFtsResult(result);
    } else {
        QMessageBox::information(&view, tr("Full-text Search Result"), tr("No matching Notebook or Note found."));
    }
}

void MainWindowPresenter::doActionFindOutlineByName()
{
    // IMPROVE rebuild model ONLY if dirty i.e. an outline name was changed on save
    vector<Outline*> os{mind->getOutlines()};
    mind->remind().sortByName(os);
    vector<Thing*> es{os.begin(),os.end()};

    findOutlineByNameDialog->show(es);
}

void MainWindowPresenter::handleFindOutlineByName()
{
    if(findOutlineByNameDialog->getChoice()) {
        orloj->showFacetOutline((Outline*)findOutlineByNameDialog->getChoice());
        // IMPROVE make this more efficient
        statusBar->showInfo(QString(tr("Notebook "))+QString::fromStdString(findOutlineByNameDialog->getChoice()->getName()));
    } else {
        statusBar->showInfo(QString(tr("Notebook not found")+": ") += findOutlineByNameDialog->getSearchedString());
    }
}

void MainWindowPresenter::doActionFindOutlineByTag()
{
    // IMPROVE rebuild model ONLY if dirty i.e. an outline name was changed on save
    vector<Outline*> os{mind->getOutlines()};
    mind->remind().sortByName(os);
    vector<Thing*> outlines{os.begin(),os.end()};

    findOutlineByTagDialog->show(outlines);
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
    if(orloj->isFacetActiveOutlineManagement()) {
        findNoteByTagDialog->setWindowTitle(tr("Find Note by Tag in Notebook"));
        findNoteByTagDialog->setScope(orloj->getOutlineView()->getCurrentOutline());
        vector<Note*> allNotes(findNoteByTagDialog->getScope()->getNotes());
        findNoteByTagDialog->show(allNotes);
    } else {
        findNoteByTagDialog->setWindowTitle(tr("Find Note by Tag"));
        findNoteByTagDialog->clearScope();
        vector<Note*> allNotes{};
        mind->getAllNotes(allNotes);
        findNoteByTagDialog->show(allNotes);
    }
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
    mind->remind().sortByName(os);
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
    if(orloj->isFacetActiveOutlineManagement()) {
        findNoteByNameDialog->setWindowTitle(tr("Find Note by Name in Notebook"));
        findNoteByNameDialog->setScope(orloj->getOutlineView()->getCurrentOutline());
        vector<Note*> allNotes(findNoteByNameDialog->getScope()->getNotes());
        findNoteByNameDialog->show(allNotes);
    } else {
        findNoteByNameDialog->setWindowTitle(tr("Find Note by Name"));
        findNoteByNameDialog->clearScope();
        vector<Note*> allNotes{};
        mind->getAllNotes(allNotes);
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

void MainWindowPresenter::doActionViewToggleRecent()
{
}

bool MainWindowPresenter::doActionViewHome()
{
    vector<const Tag*> tagsFilter{};
    tagsFilter.push_back(mind->remind().getOntology().findOrCreateTag(Tag::KeyMindForgerHome()));
    vector<Outline*> homeOutline{};
    mind->findOutlineByTags(tagsFilter, homeOutline);
    if(homeOutline.size()) {
        orloj->showFacetOutline(homeOutline.at(0));
        return true;
    } else {
        return false;
    }
}

void MainWindowPresenter::doActionViewOutlines()
{
    if(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY) {
        view.getCli()->setBreadcrumbPath("/notebooks");
        cli->executeListOutlines();
    }
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
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text);
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

void MainWindowPresenter::doActionFormatCodeblock()
{
    // IMPROVE ask for dialect
    QString text{"\n```\n...\n```\n"};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text);
    }

}

void MainWindowPresenter::doActionFormatBlockquote()
{
    rowsAndDepthDialog->setPurpose(RowsAndDepthDialog::Purpose::BLOCKQUOTE);
    rowsAndDepthDialog->show();
}

void MainWindowPresenter::doActionFormatTable()
{
    // IMPROVE ask for number of items using dialog
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

void MainWindowPresenter::doActionFormatLink()
{
    // IMPROVE rebuild model ONLY if dirty i.e. an outline name was changed on save
    vector<Outline*> oss{mind->getOutlines()};
    mind->remind().sortByName(oss);
    vector<Thing*> os{oss.begin(),oss.end()};

    vector<Note*> ns{};
    mind->getAllNotes(ns);

    QString selectedText;
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        selectedText = orloj->getNoteEdit()->getView()->getNoteEditor()->getSelectedText();
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        selectedText = orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->getSelectedText();
    }

    insertLinkDialog->show(
        config.getActiveRepository(),
        orloj->getOutlineView()->getCurrentOutline(),
        os,
        ns,
        selectedText);
}

/*
 * See InsertLinkDialog for link creation hints
 */
void MainWindowPresenter::handleFormatLink()
{
    insertLinkDialog->hide();

    QString text{"["};
    text += insertLinkDialog->getLinkText();
    text += "](";
    text += QString::fromStdString(insertLinkDialog->getPathText().toStdString());
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
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text, false);
    }
}

void MainWindowPresenter::doActionFormatImage()
{
    insertImageDialog->show();
}

void MainWindowPresenter::handleFormatImage()
{
    insertImageDialog->hide();

    QString text{"!["};
    text += insertImageDialog->getAlternateText();
    text += "](";
    text += insertImageDialog->getPathText();
    text += ")";

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text, false, 2);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text, false);
    }
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
    newOutlineDialog->show(mind->remind().getStencils(ResourceType::OUTLINE));
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

    mind->outlineNew(
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
            orloj->showFacetOutlineHeaderEdit(o);
            return;
        }
    }
    QMessageBox::critical(&view, tr("Edit Notebook"), tr("Please open an Notebook to edit."));
}

void MainWindowPresenter::handleNoteNew()
{
    int offset
        = orloj->getOutlineView()->getOutlineTree()->getCurrentRow();
    if(offset == OutlineTreePresenter::NO_ROW) {
        offset = NO_PARENT;
    } else {
        // new note to be sibling below the current line (if offset>o.notes.size(), then it's appended)
        offset++;
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
        mind->remind().remember(orloj->getOutlineView()->getCurrentOutline()->getKey());

        // insert new N and select it in the tree
        orloj->getOutlineView()->insertAndSelect(note);


        // IMPROVE smarter refresh of outline tree (do less than overall load)
        //orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());

        orloj->showFacetNoteEdit(note);
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
    if(orloj->isFacetActiveOutlineManagement()) {
        const Tag* t = mind->remind().getOntology().findOrCreateTag(Tag::KeyMindForgerHome());
        Outline* o = orloj->getOutlineView()->getCurrentOutline();
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
    } else {
        QMessageBox::critical(&view, tr("Make Notebook home"), tr("Notebook can be marked as home only when viewed."));
    }
}

void MainWindowPresenter::doActionOutlineForget()
{
    if(orloj->isFacetActiveOutlineManagement()) {
        QMessageBox::StandardButton choice;
        choice = QMessageBox::question(&view, tr("Forget Notebook"), tr("Do you really want to forget current Notebook?"));
        if (choice == QMessageBox::Yes) {
            mind->outlineForget(orloj->getOutlineView()->getCurrentOutline()->getKey());
            orloj->slotShowOutlines();
        } // else do nothing
    } else {
        QMessageBox::critical(&view, tr("Forget Notebook"), tr("Notebook can be forgotten only when viewed."));
    }
}

void MainWindowPresenter::doActionOutlineExport()
{
}

void MainWindowPresenter::doActionOutlineImport()
{
}

void MainWindowPresenter::doActionNoteNew()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE))
       // IMPROVE if note is edited, show warning that note must be saved
    {
        newNoteDialog->show(
                    QString::fromStdString(orloj->getOutlineView()->getCurrentOutline()->getKey()),
                    mind->remind().getStencils(ResourceType::NOTE));
    } else {
        QMessageBox::critical(&view, tr("New Note"), tr("Open and view a Notebook to create new Note."));
    }
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
            orloj->showFacetNoteEdit(note);
            return;
        }
    }

#ifdef __APPLE__
    doActionOutlineEdit();
#else
    QMessageBox::critical(&view, tr("Edit Note"), tr("Please select a Note to edit in the outline."));
#endif
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
        orloj->toggleCurrentFacetHoisting();
    }
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
            Outline* outline = mind->noteForget(note);
            mind->remind().remember(outline);
            orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());
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

                mind->remind().remember(orloj->getOutlineView()->getCurrentOutline()->getKey());
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

void MainWindowPresenter::doActionNoteClone()
{
    Note* n = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    if(n) {
        Note* clonedNote = mind->noteClone(orloj->getOutlineView()->getCurrentOutline()->getKey(), n);
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
    } else {
        QMessageBox::critical(&view, tr("Clone Note"), tr("Please select a Note to be cloned."));
    }
}

void MainWindowPresenter::doActionNoteFirst()
{
    Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    if(note) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->noteFirst(note, &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            mind->remind().remember(note->getOutline());
            orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
            // select Note in the tree
            QModelIndex idx
                = orloj->getOutlineView()->getOutlineTree()->getView()->model()->index(patch.start, 0);
            orloj->getOutlineView()->getOutlineTree()->getView()->setCurrentIndex(idx);
            statusBar->showInfo(QString(tr("Moved Note '%1' to be the first child")).arg(note->getName().c_str()));
        }
    } else {
        QMessageBox::critical(&view, tr("Move Note"), tr("Please select a Note to be moved."));
    }
}

void MainWindowPresenter::doActionNoteUp()
{
    Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    if(note) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->noteUp(note, &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            mind->remind().remember(note->getOutline());
            orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
            // select Note in the tree
            QModelIndex idx
                = orloj->getOutlineView()->getOutlineTree()->getView()->model()->index(patch.start, 0);
            orloj->getOutlineView()->getOutlineTree()->getView()->setCurrentIndex(idx);
            statusBar->showInfo(QString(tr("Moved up Note '%1'")).arg(note->getName().c_str()));
        }
    } else {
        QMessageBox::critical(&view, tr("Move Note"), tr("Please select a Note to be moved."));
    }
}

void MainWindowPresenter::doActionNoteDown()
{
    Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    if(note) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->noteDown(note, &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            mind->remind().remember(note->getOutline());
            orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
            // select Note in the tree
            QModelIndex idx
                = orloj->getOutlineView()->getOutlineTree()->getView()->model()->index(note->getOutline()->getNoteOffset(note), 0);
            orloj->getOutlineView()->getOutlineTree()->getView()->setCurrentIndex(idx);
            statusBar->showInfo(QString(tr("Moved down Note '%1'").arg(note->getName().c_str())));
        }
    } else {
        QMessageBox::critical(&view, tr("Move Note"), tr("Please select a Note to be moved."));
    }
}

void MainWindowPresenter::doActionNoteLast()
{
    Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    if(note) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->noteLast(note, &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            mind->remind().remember(note->getOutline());
            orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
            // select Note in the tree
            QModelIndex idx
                = orloj->getOutlineView()->getOutlineTree()->getView()->model()->index(note->getOutline()->getNoteOffset(note), 0);
            orloj->getOutlineView()->getOutlineTree()->getView()->setCurrentIndex(idx);
            statusBar->showInfo(QString(tr("Moved Note '%1' to be the last child")).arg(note->getName().c_str()));
        }
    } else {
        QMessageBox::critical(&view, tr("Move Note"), tr("Please select a Note to be moved."));
    }
}

void MainWindowPresenter::doActionNotePromote()
{
    Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    if(note) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->notePromote(note, &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            mind->remind().remember(note->getOutline());
            orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
            statusBar->showInfo(QString(tr("Promoted Note '%1'")).arg(note->getName().c_str()));
        }
    } else {
        QMessageBox::critical(&view, tr("Promote Note"), tr("Please select a Note to be promoted."));
    }
}

void MainWindowPresenter::doActionNoteDemote()
{
    Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
    if(note) {
        // IMPROVE consider patch once in class (cross functions)
        Outline::Patch patch{Outline::Patch::Diff::NO,0,0}; // explicit initialization required by older GCC versions
        mind->noteDemote(note, &patch);
        mind->remind().remember(note->getOutline());
        orloj->getOutlineView()->getOutlineTree()->refresh(note->getOutline(), &patch);
        if(patch.diff != Outline::Patch::Diff::NO) {
            statusBar->showInfo(QString(tr("Demoted Note '%1'")).arg(note->getName().c_str()));
        }
    } else {
        QMessageBox::critical(&view, tr("Demote Note"), tr("Please select a Note to be demoted."));
    }
}

void MainWindowPresenter::doActionMindRemember()
{
    mdConfigRepresentation->save(config);
}

void MainWindowPresenter::doActionMindSnapshot()
{
}

void MainWindowPresenter::doActionMindTimeScope()
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
}

void MainWindowPresenter::doActionHelpDocumentation()
{
    // Open MD w/ documentation

    /* Keyboard shortcuts:
     *
     * Editor shortcuts:
     *
     * Ctrl+g ... exit editor without saving
     * Ctrl+s
     * Alt+LeftArrow ... save and exit editor
     */

    QDesktopServices::openUrl(QUrl{"https://github.com/dvorka/mindforger-repository"});
}

void MainWindowPresenter::doActionHelpWeb()
{
    QDesktopServices::openUrl(QUrl{"http://www.mindforger.com"});
}

void MainWindowPresenter::doActionHelpReportBug()
{
    QDesktopServices::openUrl(QUrl{"https://github.com/dvorka/mindforger/issues"});
}

void MainWindowPresenter::doActionHelpCheckForUpdates()
{
    QDesktopServices::openUrl(QUrl{"https://github.com/dvorka/mindforger/releases"});
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
            "&nbsp;&nbsp;&nbsp;&nbsp;" __DATE__ " " __TIME__
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
            "<br>Copyright (C) 2018 <a href='http://me.mindforger.com'>Martin Dvorak</a> and <a href='https://github.com/dvorka/mindforger/blob/master/CREDITS.md'>contributors</a>."
        });
}

} // m8r namespace
