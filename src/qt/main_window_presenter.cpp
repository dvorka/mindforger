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

MainWindowPresenter::MainWindowPresenter(MainWindowView& view, Configuration& configuration)
    : view(view),
      configuration(configuration)
{    
    // think
    mind = new Mind{configuration};
    mind->think();

    // representations
    this->mdRepresentation
        = new MarkdownOutlineRepresentation{mind->ontology()};

    // assemble presenters w/ UI
    mainMenu = new MainMenuPresenter{this};
    cli = new CliAndBreadcrumbsPresenter{this, view.getCli(), mind};
    orloj = new OrlojPresenter{this, view.getOrloj(), mind};
    statusBar = new StatusBarPresenter{view.getStatusBar(), mind};

    // initialize components
    newOutlineDialog = new OutlineNewDialog{
                QString::fromStdString(configuration.getMemoryPath()),
                mind->remind().getOntology(),
                mind->remind().getStencils(ResourceType::OUTLINE),
                &view};
    newNoteDialog = new NoteNewDialog{
                mind->remind().getOntology(),
                mind->remind().getStencils(ResourceType::NOTE),
                &view};
    ftsDialog = new FtsDialog{&view};
    findOutlineByNameDialog = new FindOutlineByNameDialog{&view};
    findNoteByNameDialog = new FindNoteByNameDialog{&view};

    // wire signals
    QObject::connect(newOutlineDialog, SIGNAL(accepted()), this, SLOT(handleOutlineNew()));
    QObject::connect(newNoteDialog, SIGNAL(accepted()), this, SLOT(handleNoteNew()));
    QObject::connect(ftsDialog->getFindButton(), SIGNAL(clicked()), this, SLOT(handleFts()));
    QObject::connect(findOutlineByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindOutlineByName()));
    QObject::connect(findNoteByNameDialog, SIGNAL(searchFinished()), this, SLOT(handleFindNoteByName()));
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

    // TODO deletes
}

void MainWindowPresenter::initView()
{
}

#ifdef DO_MF_DEBUG
void MainWindowPresenter::doActionMindHack()
{
    qDebug() << "[MindHack] Current facet: " << orloj->getFacet();
}
#endif

void MainWindowPresenter::doActionRepositoryOpen()
{
}

void MainWindowPresenter::doActionRepositoryRecent()
{
}

void MainWindowPresenter::doActionPreferences()
{
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
    vector<Outline*> os{mind->remind().getOutlines()};
    mind->remind().sortByTitle(os);
    vector<MindEntity*> es{os.begin(),os.end()};

    findOutlineByNameDialog->show(es);
}

void MainWindowPresenter::handleFindOutlineByName()
{
    if(findOutlineByNameDialog->getChoice()) {
        orloj->showFacetOutline((Outline*)findOutlineByNameDialog->getChoice());
        // IMPROVE make this more efficient
        statusBar->showInfo(QString(tr("Outline "))+QString::fromStdString(findOutlineByNameDialog->getChoice()->getTitle()));
    } else {
        statusBar->showInfo(QString(tr("Outline not found")+": ").append(findOutlineByNameDialog->getSearchedString()));
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
        orloj->showFacetOutline(choice->getOutline());
        orloj->getNoteView()->refresh(choice);
        orloj->showFacetNoteView();
        orloj->getOutlineView()->selectRowByNote(choice);
        // IMPROVE make this more efficient
        statusBar->showInfo(QString(tr("Note "))+QString::fromStdString(choice->getTitle()));
    } else {
        statusBar->showInfo(QString(tr("Note not found")+": ").append(findNoteByNameDialog->getSearchedString()));
    }
}


void MainWindowPresenter::doActionFindNoteByTag()
{
}

void MainWindowPresenter::doActionViewToggleRecent()
{
}

void MainWindowPresenter::doActionViewOutlines()
{
    view.getCli()->setBreadcrumbPath("/outlines");
    cli->executeListOutlines();
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

void MainWindowPresenter::doActionOutlineNew()
{
    newOutlineDialog->show();
}

void MainWindowPresenter::handleOutlineNew()
{
    string title = newOutlineDialog->getOutlineName().toStdString();
    mind->outlineNew(
        &title,
        newOutlineDialog->getOutlineType(),
        newOutlineDialog->getImportance(),
        newOutlineDialog->getUrgency(),
        newOutlineDialog->getProgress(),
        newOutlineDialog->getTags(),
        newOutlineDialog->getStencil());

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_LIST_OUTLINES)) {
        // IMPROVE PERF add only 1 new outline + sort table (don't load all outlines)
        orloj->getOutlinesTable()->refresh(mind->remind().getOutlines());
    }
    // else Outlines are refreshed on facet change
}

void MainWindowPresenter::handleNoteNew()
{    
    string title = newNoteDialog->getNoteName().toStdString();
    Note* note = mind->noteNew(
                orloj->getOutlineView()->getCurrentOutline()->getKey(),
                // IMPROVE get parent note number from selection (if selected)
                NO_PARENT,
                &title,
                newNoteDialog->getNoteType(),
                newNoteDialog->getTags(),
                newNoteDialog->getProgress(),
                newNoteDialog->getStencil());
    if(note) {
        mind->remind().remember(orloj->getOutlineView()->getCurrentOutline()->getKey());
        // IMPROVE smarter refresh of outline tree (do less then overall load)
        orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());
        orloj->showFacetNoteEdit(note);
    } else {
        QMessageBox::critical(&view, tr("New Note"), tr("Failed to create new note!"));
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
    {
        newNoteDialog->show(QString::fromStdString(orloj->getOutlineView()->getCurrentOutline()->getKey()));
    } else {
        QMessageBox::critical(&view, tr("New Note"), tr("Open and view an Outline to create new Note."));
    }
}

void MainWindowPresenter::doActionNoteForget()
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE))
    {
        Note* note = orloj->getOutlineView()->getOutlineTree()->getCurrentNote();
        if(note) {
            Outline* outline = mind->noteForget(note);
            mind->remind().remember(outline);
            orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());
            return;
        }
    }
    QMessageBox::critical(&view, tr("Forget Note"), tr("Please select a note to forget."));
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
            statusBar->showInfo(QString(tr("Moved Note '%1' to be the first child")).arg(note->getTitle().c_str()));
        }
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
            statusBar->showInfo(QString(tr("Moved up Note '%1'")).arg(note->getTitle().c_str()));
        }
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
            statusBar->showInfo(QString(tr("Moved down Note '%1'").arg(note->getTitle().c_str())));
        }
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
            statusBar->showInfo(QString(tr("Moved Note '%1' to be the last child")).arg(note->getTitle().c_str()));
        }
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
            statusBar->showInfo(QString(tr("Promoted Note '%1'")).arg(note->getTitle().c_str()));
        }
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
            statusBar->showInfo(QString(tr("Demoted Note '%1'")).arg(note->getTitle().c_str()));
        }
    }
}

void MainWindowPresenter::doActionMindSnapshot()
{
}

void MainWindowPresenter::doActionHelpDocumentation()
{
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

}
