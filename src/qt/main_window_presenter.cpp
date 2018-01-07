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
    mind = new Mind(configuration);
    mind->think();

    // assemble presenters w/ UI
    mainMenu = new MainMenuPresenter(this);
    cli = new CliAndBreadcrumbsPresenter(this, view.getCli(), mind);
    orloj = new OrlojPresenter(this, view.getOrloj(), mind);
    statusBar = new StatusBarPresenter(view.getStatusBar(), mind);

    // initialize components
    newOutlineDialog = new OutlineNewDialog(
                QString::fromStdString(configuration.getMemoryPath()),
                mind->remind().getOntology(),
                mind->remind().getStencils(ResourceType::OUTLINE),
                &view);
    newNoteDialog = new NoteNewDialog(
                mind->remind().getOntology(),
                mind->remind().getStencils(ResourceType::NOTE),
                &view);

    // wire signals
    QObject::connect(newOutlineDialog, SIGNAL(accepted()), this, SLOT(handleOutlineNew()));
    QObject::connect(newNoteDialog, SIGNAL(accepted()), this, SLOT(handleNoteNew()));
}

MainWindowPresenter::~MainWindowPresenter()
{
    if(mind) delete mind;
    if(mainMenu) delete mainMenu;
    if(statusBar) delete statusBar;
    if(newOutlineDialog) delete newOutlineDialog;
    // TODO deletes
}

void MainWindowPresenter::initView(void)
{
}

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
    view.getCli()->setBreadcrumbPath("/fts");
    // TODO constant
    view.getCli()->setCommand("fts ");
    view.getCli()->showCli(false);
}

void MainWindowPresenter::doActionFindOutlineByName()
{
    cli->executeListOutlines();
    view.getCli()->setBreadcrumbPath("/");
    // TODO constant
    view.getCli()->setCommand("find outline by name ");
    view.getCli()->showCli(false);
}

void MainWindowPresenter::doActionFindNoteByName()
{
    cli->executeListNotes();
    // TODO gear function that trims string to some length + adds ... to the end > use it here
    view.getCli()->setBreadcrumbPath("/outlines/[outline name].../notes");
    // TODO constant
    view.getCli()->setCommand("find note by name ");
    view.getCli()->showCli(false);
}

void MainWindowPresenter::doActionFindNoteByTag()
{
}

void MainWindowPresenter::doActionFindPreviousNote()
{
}

void MainWindowPresenter::doActionViewToggleRecent()
{
}

void MainWindowPresenter::doActionViewOutlines(void)
{
    view.getCli()->setBreadcrumbPath("/outlines");
    cli->executeListOutlines();
}

void MainWindowPresenter::doActionCli(void)
{
    view.getCli()->setBreadcrumbPath("/");
    view.getCli()->showCli();
}

void MainWindowPresenter::doActionViewDistractionFree(void)
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

void MainWindowPresenter::handleOutlineNew(void)
{
    string title = newOutlineDialog->getOutlineName().toStdString();
    mind->outlineNew(
        &title,
        newOutlineDialog->getOutlineType(),
        newOutlineDialog->getImportance(),
        newOutlineDialog->getUrgency(),
        newOutlineDialog->getTag(),
        newOutlineDialog->getStencil());

    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_LIST_OUTLINES)) {
        // IMPROVE PERF add only 1 new outline + sort table (don't load all outlines)
        orloj->getOutlinesTable()->refresh(mind->remind().getOutlines());
    }
    // else Outlines are refreshed on facet change
}

void MainWindowPresenter::handleNoteNew(void)
{    
    string title = newNoteDialog->getNoteName().toStdString();
    bool created = mind->noteNew(
                orloj->getOutlineView()->getCurrentOutline()->getKey(),
                // IMPROVE get parent note number from selection (if selected)
                NO_PARENT,
                &title,
                newNoteDialog->getNoteType(),
                newNoteDialog->getTag(),
                newNoteDialog->getStencil());
    if(created) {
        // TODO remember modified outline
        mind->remind().remember(orloj->getOutlineView()->getCurrentOutline()->getKey());

        // TODO refresh outline note tree
        orloj->showFacetOutline(orloj->getOutlineView()->getCurrentOutline());

        // TODO open new note for edit
    } else {
        QMessageBox::critical(&view, tr("New Note"), tr("Failed to create new note!"));
    }
}

void MainWindowPresenter::doActionOutlineOpen()
{
    doActionFindOutlineByName();
}

void MainWindowPresenter::doActionOutlineClose()
{
    if(orloj->isFacetActiveOutlineManagement()) {
        orloj->slotShowOutlines();
    } else {
        QMessageBox::critical(&view, tr("Close Outline"), tr("Cannot close Outline as it's not open."));
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
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)) {
        newNoteDialog->show();
    } else {
        QMessageBox::critical(&view, tr("New Note"), tr("Open and view an Outline to create new Note."));
    }
}

void MainWindowPresenter::doActionNoteOpen()
{
}

void MainWindowPresenter::doActionNoteArchive()
{
}

void MainWindowPresenter::doActionNoteAttach()
{
}

void MainWindowPresenter::doActionNoteSave()
{
}

void MainWindowPresenter::doActionNoteFirst()
{
}

void MainWindowPresenter::doActionNoteUp()
{
}

void MainWindowPresenter::doActionNotePromote()
{
}

void MainWindowPresenter::doActionNoteDemote()
{
}

void MainWindowPresenter::doActionNoteDown()
{
}

void MainWindowPresenter::doActionNoteLast()
{
}

void MainWindowPresenter::doActionToolsBackup()
{
}

void MainWindowPresenter::doActionHelpDocumentation()
{
}

void MainWindowPresenter::doActionHelpWeb()
{
    QDesktopServices::openUrl(QUrl("http://www.mindforger.com"));
}

void MainWindowPresenter::doActionHelpReportBug()
{
    QDesktopServices::openUrl(QUrl("https://github.com/dvorka/mindforger/issues"));
}

void MainWindowPresenter::doActionHelpCheckForUpdates()
{
    QDesktopServices::openUrl(QUrl("https://github.com/dvorka/mindforger/releases"));
}

void MainWindowPresenter::doActionHelpAboutMindForger()
{
    // IMPROVE move this to view: remove this method and route signal to MainWindowView
    QMessageBox::about(
        &view,
        QString("About MindForger"),
        QString(
            "<b>MindForger " MINDFORGER_VERSION "</b>"
            "<br>"
            "<br>Personal thinking notebook."
            "<br>"
            "<br>MindForger is licensed under the <a href='https://www.gnu.org/licenses/gpl-2.0.html'>GNU GPLv2</a> or later. "
            "See also <a href='https://github.com/dvorka/mindforger/licenses'>licenses</a> directory "
            "for 3rd party content licensing."
            "<br>"
            "<br>MindForger is developed as a free and open source project on <a href='https://github.com/dvorka/mindforger'>github.com/dvorka/mindforger</a>."
            "<br>"
            "<br>Contact me at <a href='mailto:martin.dvorak@mindforger.com'>&lt;martin.dvorak@mindforger.com&gt;</a>"
            " or see <a href='https://www.mindforger.com'>www.mindforger.com</a> for more information."
            "<br>"
            "<br>Copyright (C) 2018 <a href='http://me.mindforger.com'>Martin Dvorak</a> and <a href='https://github.com/dvorka/mindforger/CREDITS.md'>contributors</a>."
        ));
}

}
