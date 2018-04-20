/*
 main_menu_presenter.cpp     MindForger thinking notebook

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
#include "main_menu_presenter.h"

namespace m8r {

MainMenuPresenter::MainMenuPresenter(MainWindowPresenter* mwp)
    : config(Configuration::getInstance())
{
    this->view = new MainMenuView(mwp->getView());
    this->mainWindowPresenter = mwp;

    // menu: mind
#ifdef DO_M8F_DEBUG
    QObject::connect(view->actionMindHack, SIGNAL(triggered()), mwp, SLOT(doActionMindHack()));
#endif
    QObject::connect(view->actionMindThink, SIGNAL(triggered()), mwp, SLOT(doActionMindThink()));
    QObject::connect(view->actionMindSleep, SIGNAL(triggered()), mwp, SLOT(doActionMindSleep()));
    QObject::connect(view->actionMindLearn, SIGNAL(triggered()), mwp, SLOT(doActionMindLearn()));
    for(auto& r:config.getRepositories()) {
        view->submenuMindRelearn->addFile(QString::fromStdString(r.first));
    }
    QObject::connect(view->submenuMindRelearn, SIGNAL(recentFileTriggered(QString)), mwp, SLOT(doActionMindRelearn(QString)));
    QObject::connect(view->actionMindScope, SIGNAL(triggered()), mwp, SLOT(doActionMindTimeScope()));
    QObject::connect(view->actionMindForget, SIGNAL(triggered()), mwp, SLOT(doActionMindForgetting()));
    QObject::connect(view->actionMindPreferences, SIGNAL(triggered()), mwp, SLOT(doActionMindPreferences()));
    QObject::connect(view->actionMindSnapshot, SIGNAL(triggered()), mwp, SLOT(doActionMindSnapshot()));
    QObject::connect(view->actionExit, SIGNAL(triggered()), mwp, SLOT(doActionExit()));

    // menu: find
    QObject::connect(view->actionFts, SIGNAL(triggered()), mwp, SLOT(doActionFts()));
    QObject::connect(view->actionFindOutlineByName, SIGNAL(triggered()), mwp, SLOT(doActionFindOutlineByName()));
    QObject::connect(view->actionFindNoteByName, SIGNAL(triggered()), mwp, SLOT(doActionFindNoteByName()));
    QObject::connect(view->actionFindOutlineByTag, SIGNAL(triggered()), mwp, SLOT(doActionFindOutlineByTag()));
    QObject::connect(view->actionFindNoteByTag, SIGNAL(triggered()), mwp, SLOT(doActionFindNoteByTag()));

    // menu: view
    QObject::connect(view->actionViewHome, SIGNAL(triggered()), mwp, SLOT(doActionViewHome()));
    QObject::connect(view->actionViewOutlines, SIGNAL(triggered()), mwp, SLOT(doActionViewOutlines()));
#ifdef DO_M8F_DEBUG
    QObject::connect(view->actionViewCli, SIGNAL(triggered()), mwp, SLOT(doActionCli()));
#else
    QObject::connect(view, SIGNAL(showCli()), mwp, SLOT(doActionCli()));
#endif
    QObject::connect(view->actionViewToggleRecent, SIGNAL(triggered()), mwp, SLOT(doActionViewToggleRecent()));
    QObject::connect(view->actionViewDistractionFree, SIGNAL(triggered()), mwp, SLOT(doActionViewDistractionFree()));
    QObject::connect(view->actionViewFullscreen, SIGNAL(triggered()), mwp, SLOT(doActionViewFullscreen()));

    // menu: outline
    QObject::connect(view->actionOutlineNew, SIGNAL(triggered()), mwp, SLOT(doActionOutlineNew()));
    QObject::connect(view->actionOutlineForget, SIGNAL(triggered()), mwp, SLOT(doActionOutlineForget()));
    QObject::connect(view->actionOutlineHome, SIGNAL(triggered()), mwp, SLOT(doActionOutlineHome()));
    QObject::connect(view->actionOutlineClone, SIGNAL(triggered()), mwp, SLOT(doActionOutlineClone()));
    QObject::connect(view->actionOutlineExport, SIGNAL(triggered()), mwp, SLOT(doActionOutlineExport()));
    QObject::connect(view->actionOutlineImport, SIGNAL(triggered()), mwp, SLOT(doActionOutlineImport()));

    // menu: note
    QObject::connect(view->actionNoteNew, SIGNAL(triggered()), mwp, SLOT(doActionNoteNew()));
    QObject::connect(view->actionNoteSave, SIGNAL(triggered()), mwp, SLOT(doActionNoteSave()));
    QObject::connect(view->actionNoteForget, SIGNAL(triggered()), mwp, SLOT(doActionNoteForget()));
    QObject::connect(view->actionNotePromote, SIGNAL(triggered()), mwp, SLOT(doActionNotePromote()));
    QObject::connect(view->actionNoteDemote, SIGNAL(triggered()), mwp, SLOT(doActionNoteDemote()));
    QObject::connect(view->actionNoteFirst, SIGNAL(triggered()), mwp, SLOT(doActionNoteFirst()));
    QObject::connect(view->actionNoteUp, SIGNAL(triggered()), mwp, SLOT(doActionNoteUp()));
    QObject::connect(view->actionNoteDown, SIGNAL(triggered()), mwp, SLOT(doActionNoteDown()));
    QObject::connect(view->actionNoteLast, SIGNAL(triggered()), mwp, SLOT(doActionNoteLast()));
    QObject::connect(view->actionNoteRefactor, SIGNAL(triggered()), mwp, SLOT(doActionRefactorNoteToOutline()));
    QObject::connect(view->actionNoteExtract, SIGNAL(triggered()), mwp, SLOT(doActionNoteExtract()));
    QObject::connect(view->actionNoteClone, SIGNAL(triggered()), mwp, SLOT(doActionNoteClone()));
    QObject::connect(view->actionNoteAttach, SIGNAL(triggered()), mwp, SLOT(doActionNoteAttach()));

    // menu: help
    QObject::connect(view->actionHelpDocumentation, SIGNAL(triggered()), mwp, SLOT(doActionHelpDocumentation()));
    QObject::connect(view->actionHelpWeb, SIGNAL(triggered()), mwp, SLOT(doActionHelpWeb()));
    QObject::connect(view->actionHelpReportBug, SIGNAL(triggered()), mwp, SLOT(doActionHelpReportBug()));
    QObject::connect(view->actionHelpCheckForUpdates, SIGNAL(triggered()), mwp, SLOT(doActionHelpCheckForUpdates()));
    QObject::connect(view->actionHelpAbout, SIGNAL(triggered()), mwp, SLOT(doActionHelpAboutMindForger()));

    switch(config.getDesiredMindState()) {
    case Configuration::MindState::THINKING:
        showFacetMindThink();
        break;
    case Configuration::MindState::SLEEPING:
        showFacetMindSleep();
        break;
    default:
        ; // do nothing for dreaming
    }
}

MainMenuPresenter::~MainMenuPresenter()
{
    // TODO deletes: actions
}

void MainMenuPresenter::showFacetOutlineList()
{
    view->showFacetOutlineList(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY);
}

void MainMenuPresenter::showFacetOutlineView()
{
    view->showFacetOutlineView(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY);
}

void MainMenuPresenter::showFacetNoteEdit()
{
    view->showFacetNoteEdit(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY);
}

void MainMenuPresenter::showFacetMindThink()
{
    view->showFacetMindThink();
}

void MainMenuPresenter::showFacetMindSleep()
{
    view->showFacetMindSleep();
}

void MainMenuPresenter::addRecentDirectoryOrFile(const QString& path)
{
    view->addRepositoryOrFileToRelearn(path);
}

} // namespace
