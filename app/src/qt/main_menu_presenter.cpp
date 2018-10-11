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
    QObject::connect(view->actionMindNewRepository, SIGNAL(triggered()), mwp, SLOT(doActionMindNewRepository()));
    QObject::connect(view->actionMindNewFile, SIGNAL(triggered()), mwp, SLOT(doActionMindNewFile()));
    QObject::connect(view->actionMindThink, SIGNAL(triggered()), mwp, SLOT(doActionMindToggleThink()));
    QObject::connect(view->actionMindAutolink, SIGNAL(triggered()), mwp, SLOT(doActionMindToggleAutolink()));
    QObject::connect(view->actionMindLearnRepository, SIGNAL(triggered()), mwp, SLOT(doActionMindLearnRepository()));
    QObject::connect(view->actionMindLearnFile, SIGNAL(triggered()), mwp, SLOT(doActionMindLearnFile()));
    for(auto& r:config.getRepositories()) {
        view->submenuMindRelearn->addFile(QString::fromStdString(r.first));
    }
    QObject::connect(view->submenuMindRelearn, SIGNAL(recentFileTriggered(QString)), mwp, SLOT(doActionMindRelearn(QString)));
    QObject::connect(view->actionMindScope, SIGNAL(triggered()), mwp, SLOT(doActionMindTimeTagScope()));
    QObject::connect(view->actionMindRemember, SIGNAL(triggered()), mwp, SLOT(doActionMindRemember()));
    // TODO QObject::connect(view->actionMindForget, SIGNAL(triggered()), mwp, SLOT(doActionMindForgetting()));
    QObject::connect(view->actionMindPreferences, SIGNAL(triggered()), mwp, SLOT(doActionMindPreferences()));
    QObject::connect(view->actionMindSnapshot, SIGNAL(triggered()), mwp, SLOT(doActionMindSnapshot()));
    QObject::connect(view->actionMindExportCsv, SIGNAL(triggered()), mwp, SLOT(doActionMindCsvExport()));
    QObject::connect(view->actionExit, SIGNAL(triggered()), mwp, SLOT(doActionExit()));
#ifdef DO_MF_DEBUG
    QObject::connect(view->actionMindHack, SIGNAL(triggered()), mwp, SLOT(doActionMindHack()));
#endif

    // menu: recall
    QObject::connect(view->actionFts, SIGNAL(triggered()), mwp, SLOT(doActionFts()));
    QObject::connect(view->actionFindOutlineByName, SIGNAL(triggered()), mwp, SLOT(doActionFindOutlineByName()));
    QObject::connect(view->actionFindNoteByName, SIGNAL(triggered()), mwp, SLOT(doActionFindNoteByName()));
    QObject::connect(view->actionFindOutlineByTag, SIGNAL(triggered()), mwp, SLOT(doActionFindOutlineByTag()));
    QObject::connect(view->actionFindNoteByTag, SIGNAL(triggered()), mwp, SLOT(doActionFindNoteByTag()));
#ifdef MF_NER
    QObject::connect(view->actionFindNerPersons, SIGNAL(triggered()), mwp, SLOT(doActionFindNerPersons()));
    QObject::connect(view->actionFindNerLocations, SIGNAL(triggered()), mwp, SLOT(doActionFindNerLocations()));
    QObject::connect(view->actionFindNerOrganizations, SIGNAL(triggered()), mwp, SLOT(doActionFindNerOrganizations()));
    QObject::connect(view->actionFindNerMisc, SIGNAL(triggered()), mwp, SLOT(doActionFindNerMisc()));
#endif

    // menu: view
    QObject::connect(view->actionViewHome, SIGNAL(triggered()), mwp, SLOT(doActionViewHome()));
    QObject::connect(view->actionViewOrganizer, SIGNAL(triggered()), mwp, SLOT(doActionViewOrganizer()));
    QObject::connect(view->actionViewOutlines, SIGNAL(triggered()), mwp, SLOT(doActionViewOutlines()));
    QObject::connect(view->actionViewTags, SIGNAL(triggered()), mwp, SLOT(doActionViewTagCloud()));
    QObject::connect(view->actionViewNavigator, SIGNAL(triggered()), mwp, SLOT(doActionViewKnowledgeGraphNavigator()));
#ifdef DO_MF_DEBUG
    QObject::connect(view->actionViewCli, SIGNAL(triggered()), mwp, SLOT(doActionCli()));
#else
    QObject::connect(view, SIGNAL(showCli()), mwp, SLOT(doActionCli()));
#endif
    QObject::connect(view->actionViewRecentNotes, SIGNAL(triggered()), mwp, SLOT(doActionViewRecentNotes()));
    QObject::connect(view->actionViewDistractionFree, SIGNAL(triggered()), mwp, SLOT(doActionViewDistractionFree()));
    QObject::connect(view->actionViewFullscreen, SIGNAL(triggered()), mwp, SLOT(doActionViewFullscreen()));

    // menu: navigator
    // IMPROVE complete edge shrinking & zooming (skipped as shortcut/mouse wheel is the much more reasonable)
    QObject::connect(view->actionNavigatorShuffle, SIGNAL(triggered()), mwp, SLOT(doActionNavigatorShuffle()));

    // menu: Outline
    QObject::connect(view->actionOutlineNew, SIGNAL(triggered()), mwp, SLOT(doActionOutlineNew()));
    QObject::connect(view->actionOutlineEdit, SIGNAL(triggered()), mwp, SLOT(doActionOutlineEdit()));
    QObject::connect(view->actionOutlineForget, SIGNAL(triggered()), mwp, SLOT(doActionOutlineForget()));
    QObject::connect(view->actionOutlineHome, SIGNAL(triggered()), mwp, SLOT(doActionOutlineHome()));
    QObject::connect(view->actionOutlineClone, SIGNAL(triggered()), mwp, SLOT(doActionOutlineClone()));
    QObject::connect(view->actionOutlineHtmlExport, SIGNAL(triggered()), mwp, SLOT(doActionOutlineHtmlExport()));
    QObject::connect(view->actionOutlineTWikiImport, SIGNAL(triggered()), mwp, SLOT(doActionOutlineTWikiImport()));

    // menu: Note
    QObject::connect(view->actionNoteNew, SIGNAL(triggered()), mwp, SLOT(doActionNoteNew()));
    QObject::connect(view->actionNoteHoist, SIGNAL(triggered()), mwp, SLOT(doActionNoteHoist()));
    QObject::connect(view->actionNoteEdit, SIGNAL(triggered()), mwp, SLOT(doActionNoteEdit()));
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

    // menu: edit
    // no binding needed - it's already bound in the editor ~ menu is rather a documentation

    // menu: format
    QObject::connect(view->actionFormatBold, SIGNAL(triggered()), mwp, SLOT(doActionFormatBold()));
    QObject::connect(view->actionFormatItalic, SIGNAL(triggered()), mwp, SLOT(doActionFormatItalic()));
    QObject::connect(view->actionFormatCode, SIGNAL(triggered()), mwp, SLOT(doActionFormatCode()));
    QObject::connect(view->actionFormatMath, SIGNAL(triggered()), mwp, SLOT(doActionFormatMath()));
    QObject::connect(view->actionFormatMathFraction, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathFrac()));
    QObject::connect(view->actionFormatMathSum, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathSum()));
    QObject::connect(view->actionFormatMathInt, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathInt()));
    QObject::connect(view->actionFormatMathIiint, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathIiint()));
    QObject::connect(view->actionFormatMathAlpha, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathAlpha()));
    QObject::connect(view->actionFormatMathBeta, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathBeta()));
    QObject::connect(view->actionFormatMathDelta, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathDelta()));
    QObject::connect(view->actionFormatMathGama, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathGama()));
    QObject::connect(view->actionFormatMathText, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathText()));
    QObject::connect(view->actionFormatMathBar, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathBar()));
    QObject::connect(view->actionFormatMathHat, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathHat()));
    QObject::connect(view->actionFormatMathDot, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathDot()));
    QObject::connect(view->actionFormatMathOverrightarrow, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathOverrightarrow()));
    QObject::connect(view->actionFormatMathCup, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathCup()));
    QObject::connect(view->actionFormatMathCap, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathCap()));
    QObject::connect(view->actionFormatMathEmptyset, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathEmptyset()));
    QObject::connect(view->actionFormatMathIn, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathIn()));
    QObject::connect(view->actionFormatMathNotin, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathNotin()));
    QObject::connect(view->actionFormatMathSqrt, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathSqrt()));
    QObject::connect(view->actionFormatStrikethrough, SIGNAL(triggered()), mwp, SLOT(doActionFormatStrikethrough()));
    QObject::connect(view->actionFormatKeyboard, SIGNAL(triggered()), mwp, SLOT(doActionFormatKeyboard()));
    QObject::connect(view->actionFormatListBullet, SIGNAL(triggered()), mwp, SLOT(doActionFormatListBullet()));
    QObject::connect(view->actionFormatListNumber, SIGNAL(triggered()), mwp, SLOT(doActionFormatListNumber()));
    QObject::connect(view->actionFormatListTask, SIGNAL(triggered()), mwp, SLOT(doActionFormatListTask()));
    QObject::connect(view->actionFormatToc, SIGNAL(triggered()), mwp, SLOT(doActionFormatToc()));
    QObject::connect(view->actionFormatTimestamp, SIGNAL(triggered()), mwp, SLOT(doActionFormatTimestamp()));
    QObject::connect(view->actionFormatCodeBlock, SIGNAL(triggered()), mwp, SLOT(doActionFormatCodeBlock()));
    QObject::connect(view->actionFormatMathBlock, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathBlock()));
    QObject::connect(view->actionFormatBlockQuote, SIGNAL(triggered()), mwp, SLOT(doActionFormatBlockquote()));
    QObject::connect(view->actionFormatLink, SIGNAL(triggered()), mwp, SLOT(doActionFormatLink()));
    QObject::connect(view->actionFormatImage, SIGNAL(triggered()), mwp, SLOT(doActionFormatImage()));
    QObject::connect(view->actionFormatTable, SIGNAL(triggered()), mwp, SLOT(doActionFormatTable()));
    QObject::connect(view->actionFormatHr, SIGNAL(triggered()), mwp, SLOT(doActionFormatHr()));

    // menu: help
    QObject::connect(view->actionHelpDocumentation, SIGNAL(triggered()), mwp, SLOT(doActionHelpDocumentation()));
    QObject::connect(view->actionHelpWeb, SIGNAL(triggered()), mwp, SLOT(doActionHelpWeb()));
    QObject::connect(view->actionHelpReportBug, SIGNAL(triggered()), mwp, SLOT(doActionHelpReportBug()));
    QObject::connect(view->actionHelpCheckForUpdates, SIGNAL(triggered()), mwp, SLOT(doActionHelpCheckForUpdates()));
    QObject::connect(view->actionHelpMarkdown, SIGNAL(triggered()), mwp, SLOT(doActionHelpMarkdown()));
    QObject::connect(view->actionHelpMathJaxQuickReference, SIGNAL(triggered()), mwp, SLOT(doActionHelpMathJaxQuickReference()));
    QObject::connect(view->actionHelpMathJaxLivePreview, SIGNAL(triggered()), mwp, SLOT(doActionHelpMathJaxLivePreview()));
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

void MainMenuPresenter::showFacetOrganizer()
{
    view->showFacetOutlineList(config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY);
}

void MainMenuPresenter::showFacetNavigator()
{
    view->showFacetNavigator();
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

void MainMenuPresenter::showFacetMindAutolinkEnable()
{
    view->showFacetMindAutolinkEnable();
}
void MainMenuPresenter::showFacetMindAutolinkDisable()
{
    view->showFacetMindAutolinkDisable();
}

void MainMenuPresenter::addRecentDirectoryOrFile(const QString& path)
{
    view->addRepositoryOrFileToRelearn(path);
}

} // namespace
