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
{
    this->view = new MainMenuView(mwp->getView());
    this->maindWindowPresenter = mwp;

    // menu: mind
#ifdef MFDEBUG
    QObject::connect(view->actionMindHack, SIGNAL(triggered()),mwp, SLOT(doActionMindHack()));
#endif
    QObject::connect(view->actionMindSnapshot, SIGNAL(triggered()), mwp, SLOT(doActionMindSnapshot()));
    QObject::connect(view->actionExit, SIGNAL(triggered()), mwp, SLOT(doActionExit()));

    // menu: find
    QObject::connect(
        view->actionFts, SIGNAL(triggered()),
        mwp, SLOT(doActionFts()));
    QObject::connect(
        view->actionFindOutlineByName, SIGNAL(triggered()),
        mwp, SLOT(doActionFindOutlineByName()));
    QObject::connect(view->actionFindNoteByName, SIGNAL(triggered()),
        mwp, SLOT(doActionFindNoteByName()));
    QObject::connect(view->actionFindPreviousNote, SIGNAL(triggered()), mwp, SLOT(doActionFindPreviousNote()));

    // menu: view
    QObject::connect(view->actionViewOutlines, SIGNAL(triggered()), mwp, SLOT(doActionViewOutlines()));
    QObject::connect(view->actionViewCli, SIGNAL(triggered()), mwp, SLOT(doActionCli()));
    QObject::connect(view->actionViewCli, SIGNAL(triggered()), mwp, SLOT(doActionCli()));
    QObject::connect(view->actionViewToggleRecent, SIGNAL(triggered()), mwp, SLOT(doActionViewToggleRecent()));
    QObject::connect(view->actionViewDistractionFree, SIGNAL(triggered()), mwp, SLOT(doActionViewDistractionFree()));
    QObject::connect(view->actionViewFullscreen, SIGNAL(triggered()), mwp, SLOT(doActionViewFullscreen()));

    // menu: outline
    QObject::connect(view->actionOutlineNew, SIGNAL(triggered()), mwp, SLOT(doActionOutlineNew()));
    QObject::connect(view->actionOutlineOpen, SIGNAL(triggered()), mwp, SLOT(doActionOutlineOpen()));
    QObject::connect(view->actionOutlineClose, SIGNAL(triggered()), mwp, SLOT(doActionOutlineClose()));
    QObject::connect(view->actionOutlineForget, SIGNAL(triggered()), mwp, SLOT(doActionOutlineForget()));
    QObject::connect(view->actionOutlineExport, SIGNAL(triggered()), mwp, SLOT(doActionOutlineExport()));
    QObject::connect(view->actionOutlineImport, SIGNAL(triggered()), mwp, SLOT(doActionOutlineImport()));

    // menu: note
    QObject::connect(view->actionNoteNew, SIGNAL(triggered()), mwp, SLOT(doActionNoteNew()));
    QObject::connect(view->actionNoteOpen, SIGNAL(triggered()), mwp, SLOT(doActionNoteOpen()));
    QObject::connect(view->actionNoteFirst, SIGNAL(triggered()), mwp, SLOT(doActionNoteFirst()));
    QObject::connect(view->actionNoteUp, SIGNAL(triggered()), mwp, SLOT(doActionNoteUp()));
    QObject::connect(view->actionNotePromote, SIGNAL(triggered()), mwp, SLOT(doActionNotePromote()));
    QObject::connect(view->actionNoteDemote, SIGNAL(triggered()), mwp, SLOT(doActionNoteDemote()));
    QObject::connect(view->actionNoteDown, SIGNAL(triggered()), mwp, SLOT(doActionNoteDown()));
    QObject::connect(view->actionNoteLast, SIGNAL(triggered()), mwp, SLOT(doActionNoteLast()));
    QObject::connect(view->actionNoteAttach, SIGNAL(triggered()), mwp, SLOT(doActionNoteAttach()));
    QObject::connect(view->actionNoteSave, SIGNAL(triggered()), mwp, SLOT(doActionNoteSave()));
    QObject::connect(view->actionNoteForget, SIGNAL(triggered()), mwp, SLOT(doActionNoteForget()));

    // menu: help
    QObject::connect(view->actionHelpDocumentation, SIGNAL(triggered()), mwp, SLOT(doActionHelpDocumentation()));
    QObject::connect(view->actionHelpWeb, SIGNAL(triggered()), mwp, SLOT(doActionHelpWeb()));
    QObject::connect(view->actionHelpReportBug, SIGNAL(triggered()), mwp, SLOT(doActionHelpReportBug()));
    QObject::connect(view->actionHelpCheckForUpdates, SIGNAL(triggered()), mwp, SLOT(doActionHelpCheckForUpdates()));
    QObject::connect(view->actionHelpAbout, SIGNAL(triggered()), mwp, SLOT(doActionHelpAboutMindForger()));
}

MainMenuPresenter::~MainMenuPresenter()
{
    // TODO deletes: actions
}

}
