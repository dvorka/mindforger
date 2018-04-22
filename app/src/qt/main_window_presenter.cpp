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
    timeScopeDialog = new TimeScopeDialog{&view};
    forgetDialog = new ForgetDialog{&view};
    newOutlineDialog = new OutlineNewDialog{
                QString::fromStdString(config.getMemoryPath()),
                mind->remind().getOntology(),
                &view};
    newNoteDialog = new NoteNewDialog{mind->remind().getOntology(), &view};
    ftsDialog = new FtsDialog{&view};
    findOutlineByNameDialog = new FindOutlineByNameDialog{&view};
    findNoteByNameDialog = new FindNoteByNameDialog{&view};
    findOutlineByTagDialog = new FindOutlineByTagDialog{mind->remind().getOntology(), &view};
    findNoteByTagDialog = new FindNoteByTagDialog{mind->remind().getOntology(), &view};
    refactorNoteToOutlineDialog = new RefactorNoteToOutlineDialog{&view};
    configDialog = new ConfigurationDialog{&view};

    // wire signals
    QObject::connect(timeScopeDialog->getSetButton(), SIGNAL(clicked()), this, SLOT(handleMindTimeScope()));
    QObject::connect(forgetDialog->getSetButton(), SIGNAL(clicked()), this, SLOT(handleMindForgetting()));
    QObject::connect(newOutlineDialog, SIGNAL(accepted()), this, SLOT(handleOutlineNew()));
    QObject::connect(newNoteDialog, SIGNAL(accepted()), this, SLOT(handleNoteNew()));
    QObject::connect(ftsDialog->getFindButton(), SIGNAL(clicked()), this, SLOT(handleFts()));
    QObject::connect(findOutlineByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindOutlineByName()));
    QObject::connect(findNoteByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindNoteByName()));
    QObject::connect(findOutlineByTagDialog, SIGNAL(searchFinished()), this, SLOT(handleFindOutlineByTag()));
    QObject::connect(findNoteByTagDialog, SIGNAL(searchFinished()), this, SLOT(handleFindNoteByTag()));
    QObject::connect(refactorNoteToOutlineDialog, SIGNAL(searchFinished()), this, SLOT(handleRefactorNoteToOutline()));
    QObject::connect(configDialog->getAppTab(), SIGNAL(saveConfigSignal()), this, SLOT(handleMindPreferences()));

    // async task 2 GUI events distributor
    distributor = new AsyncTaskNotificationsDistributor(this);
    // setup callback for cleanup when it finishes
    QObject::connect(distributor, SIGNAL(finished()), distributor, SLOT(deleteLater()));
    distributor->start();

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

#ifdef DO_M8F_DEBUG
void MainWindowPresenter::doActionMindHack()
{
    qDebug() << "[MindHack] Current facet: " << orloj->getFacet();
}
#endif

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

void MainWindowPresenter::doActionMindLearn()
{
    QString homeDirectory
        = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);

    // IMPROVE extend Qt's file dialog to support both directory and file section (message box goes away)
    QMessageBox msgBox{&view};
    msgBox.setIcon(QMessageBox::Icon::Question);
    msgBox.setText(tr("Do you want to learn content of directory or file?"));
    QPushButton* repositoryButton = msgBox.addButton(tr("&Directory"), QMessageBox::ActionRole);
    QPushButton* fileButton = msgBox.addButton(tr("&File"), QMessageBox::ActionRole);
    QPushButton* cancelButton = msgBox.addButton(QMessageBox::Cancel);
    msgBox.exec();

    QFileDialog learnDialog{&view};
    if (msgBox.clickedButton() == repositoryButton) {
        learnDialog.setWindowTitle(tr("Learn Directory"));
        // learnDialog.setFileMode(QFileDialog::Directory|QFileDialog::ExistingFiles); not supported, therefore
        // >
        // ASK user: directory/repository or file (choice) > open dialog configured as required
        learnDialog.setFileMode(QFileDialog::Directory);
        learnDialog.setDirectory(homeDirectory);
        learnDialog.setViewMode(QFileDialog::Detail);
    } else if (msgBox.clickedButton() == fileButton) {
        learnDialog.setWindowTitle(tr("Learn File"));
        learnDialog.setFileMode(QFileDialog::ExistingFile);
        learnDialog.setDirectory(homeDirectory);
        learnDialog.setViewMode(QFileDialog::Detail);
    } else if (msgBox.clickedButton() == cancelButton) {
        return;
    }

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
        ftsDialog->setWindowTitle(tr("Full-text Search in Outline"));
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
        QMessageBox::information(&view, tr("Full-text Search Result"), tr("No matching Outline or Note found."));
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
        statusBar->showInfo(QString(tr("Outline "))+QString::fromStdString(findOutlineByNameDialog->getChoice()->getName()));
    } else {
        statusBar->showInfo(QString(tr("Outline not found")+": ") += findOutlineByNameDialog->getSearchedString());
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
        statusBar->showInfo(QString(tr("Outline "))+QString::fromStdString(findOutlineByTagDialog->getChoice()->getName()));
    } else {
        statusBar->showInfo(QString(tr("Outline not found")));
    }
}

void MainWindowPresenter::doActionFindNoteByTag()
{
    // IMPROVE rebuild model ONLY if dirty i.e. an outline name was changed on save
    if(orloj->isFacetActiveOutlineManagement()) {
        findNoteByTagDialog->setWindowTitle(tr("Find Note by Tag in Outline"));
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
            statusBar->showInfo(QString(tr("Refactored Note to Outline '"))+QString::fromStdString(refactorNoteToOutlineDialog->getChoice()->getName())+"'...");
        } else {
            statusBar->showInfo(QString(tr("Target Outline not found")+": ") += refactorNoteToOutlineDialog->getSearchedString());
        }
    } else {
        QMessageBox::critical(&view, tr("Refactor Note"), tr("Note to be refactored not specified!"));
    }
}

void MainWindowPresenter::doActionFindNoteByName()
{
    // IMPROVE rebuild model ONLY if dirty i.e. an outline name was changed on save
    if(orloj->isFacetActiveOutlineManagement()) {
        findNoteByNameDialog->setWindowTitle(tr("Find Note by Name in Outline"));
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
        view.getCli()->setBreadcrumbPath("/outlines");
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

void MainWindowPresenter::doActionFormatListBullet()
{
    // IMPROVE ask for number of items using dialog
    int count=3;
    QString text{"\n"};
    for(int i=0; i<count; i++) {
        text += "* ...\n";
    }

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text);
    }
}

void MainWindowPresenter::doActionFormatListNumber()
{
    // IMPROVE ask for number of items using dialog
    int count=3;
    QString text{"\n"};
    for(int i=1; i<=count; i++) {
        text += QString::number(i);
        text += ". ...\n";
    }

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text);
    }
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
    // IMPROVE ask for number of items using dialog
    QString text{"\n> .\n> .\n> .\n"};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text);
    }
}

void MainWindowPresenter::doActionFormatLink()
{
    // IMPROVE dialog to specify link params
    QString text{"[]()"};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text, false);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text, false);
    }
}

void MainWindowPresenter::doActionFormatImage()
{
    // IMPROVE dialog to specify image params
    QString text{"![]()"};

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
        orloj->getNoteEdit()->getView()->getNoteEditor()->insertMarkdownText(text, false);
    } else if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineHeaderEdit()->getView()->getHeaderEditor()->insertMarkdownText(text, false);
    }
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
        newOutlineDialog->getTags(),
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
    QMessageBox::critical(&view, tr("Edit Outline"), tr("Please open an Outline to edit."));
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
                newNoteDialog->getTags(),
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
            QMessageBox::critical(&view, tr("Clone Outline"), tr("Failed to clone Outline!"));
        }
    } else {
        QMessageBox::critical(&view, tr("Clone Outline"), tr("Please open and Outline to be cloned."));
    }
}

void MainWindowPresenter::doActionOutlineHome()
{
    if(orloj->isFacetActiveOutlineManagement()) {
        const Tag* t = mind->remind().getOntology().findOrCreateTag(Tag::KeyMindForgerHome());
        Outline* o = orloj->getOutlineView()->getCurrentOutline();
        if(mind->setOutlineUniqueTag(t, o->getKey())) {
            statusBar->showInfo(tr("Outline '%1' successfully marked as home").arg(o->getName().c_str()));
        }
    } else {
        QMessageBox::critical(&view, tr("Make Outline home"), tr("Outline can be marked as home only when viewed."));
    }
}

void MainWindowPresenter::doActionOutlineForget()
{
    if(orloj->isFacetActiveOutlineManagement()) {
        QMessageBox::StandardButton choice;
        choice = QMessageBox::question(&view, tr("Delete Outline"), tr("Do you really want to forget current Outline?"));
        if (choice == QMessageBox::Yes) {
            mind->outlineForget(orloj->getOutlineView()->getCurrentOutline()->getKey());
            orloj->slotShowOutlines();
        } // else do nothing
    } else {
        QMessageBox::critical(&view, tr("Delete Outline"), tr("Outline can be forgotten only when viewed."));
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
        QMessageBox::critical(&view, tr("New Note"), tr("Open and view an Outline to create new Note."));
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
    QMessageBox::critical(&view, tr("Edit Note"), tr("Please select a Note to edit."));
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

void MainWindowPresenter::doActionNoteAttach()
{
}

void MainWindowPresenter::doActionNoteSave()
{
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

void MainWindowPresenter::doActionMindSnapshot()
{
}

void MainWindowPresenter::doActionMindTimeScope()
{
    TimeScopeAspect& a=mind->getTimeScopeAspect();
    timeScopeDialog->show(
        a.isEnabled(),
        a.getTimeScope().years,
        a.getTimeScope().months,
        a.getTimeScope().days,
        a.getTimeScope().hours,
        a.getTimeScope().minutes);
}

void MainWindowPresenter::handleMindTimeScope()
{
    TimeScope& ts=mind->getTimeScopeAspect().getTimeScope();
    if(timeScopeDialog->isThreasholdSet()) {
        ts.years=timeScopeDialog->getYears();
        ts.years=timeScopeDialog->getMonths();
        ts.months=timeScopeDialog->getMonths();
        ts.days=timeScopeDialog->getDays();
        ts.hours=timeScopeDialog->getHours();
        ts.minutes=timeScopeDialog->getMinutes();

        ts.recalculateRelativeSecs();
    } else {
        ts.reset();
    }

    // notify and persist changes
    mind->getTimeScopeAspect().setTimeScope(ts);
    config.setTimeScope(mind->getTimeScopeAspect().getTimeScope());
    mdConfigRepresentation->save(config);

    // IMPROVE don't change view to Os, but refresh current one
    doActionViewOutlines();
}

void MainWindowPresenter::doActionMindForgetting()
{
}

void MainWindowPresenter::handleMindForgetting()
{
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
#ifdef DO_M8F_DEBUG
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
