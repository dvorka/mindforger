/*
 main_menu_presenter.cpp     MindForger thinking notebook

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
#include "main_menu_presenter.h"

namespace m8r {

MainMenuPresenter::MainMenuPresenter(MainWindowPresenter* mwp)
    : config(Configuration::getInstance())
{
    this->view = new MainMenuView(mwp->getView());
    this->mainWindowPresenter = mwp;

    // menu: mind
    QObject::connect(
        view->actionMindNewRepository, SIGNAL(triggered()),
        mwp, SLOT(doActionMindNewRepository()));
    QObject::connect(
        view->actionMindNewFile, SIGNAL(triggered()),
        mwp, SLOT(doActionMindNewFile()));
    QObject::connect(
        view->actionMindWingman, SIGNAL(triggered()),
        mwp, SLOT(handleActionWingman()));
    QObject::connect(
        view->actionMindTool, SIGNAL(triggered()),
        mwp, SLOT(doActionRunToolDialogAnywhere()));
    QObject::connect(
        view->actionMindThink, SIGNAL(triggered()),
        mwp, SLOT(doActionMindToggleThink()));
    QObject::connect(
        view->actionMindAutolink, SIGNAL(triggered()),
        mwp, SLOT(doActionMindToggleAutolink()));
    QObject::connect(
        view->actionMindLearnDirectory, SIGNAL(triggered()),
        mwp, SLOT(doActionMindLearnRepository()));
    QObject::connect(
        view->actionMindLearnRepository, SIGNAL(triggered()),
        mwp, SLOT(doActionMindLearnRepository()));
    QObject::connect(
        view->actionMindLearnFile, SIGNAL(triggered()),
        mwp, SLOT(doActionMindLearnFile()));
    for(auto& r:config.getRepositories()) {
        view->submenuMindRelearn->addFile(QString::fromStdString(r.first));
    }
    QObject::connect(
        view->submenuMindRelearn, SIGNAL(recentFileTriggered(QString)),
        mwp, SLOT(doActionMindRelearn(QString)));
    QObject::connect(
        view->actionMindScope, SIGNAL(triggered()),
        mwp, SLOT(doActionMindTimeTagScope()));
    QObject::connect(
        view->actionMindRemember, SIGNAL(triggered()),
        mwp, SLOT(doActionMindRemember()));
    QObject::connect(
        view->actionMindPreferences, SIGNAL(triggered()),
        mwp, SLOT(doActionMindPreferences()));
    QObject::connect(
        view->actionMindSnapshot, SIGNAL(triggered()),
        mwp, SLOT(doActionMindSnapshot()));
    QObject::connect(
        view->actionMindExportCsv, SIGNAL(triggered()),
        mwp, SLOT(doActionMindCsvExport()));
    QObject::connect(
        view->actionExit, SIGNAL(triggered()),
        mwp, SLOT(doActionExit()));
#ifdef DO_MF_DEBUG
    QObject::connect(view->actionMindHack, SIGNAL(triggered()), mwp, SLOT(doActionMindHack()));
#endif

    // menu: recall
    QObject::connect(view->actionFindFts, SIGNAL(triggered()), mwp, SLOT(doActionFts()));
    QObject::connect(view->actionFindOutlineByName, SIGNAL(triggered()), mwp, SLOT(doActionFindOutlineByName()));
    QObject::connect(view->actionFindNoteByName, SIGNAL(triggered()), mwp, SLOT(doActionFindNoteByName()));
    QObject::connect(view->actionFindOutlineByTag, SIGNAL(triggered()), mwp, SLOT(doActionFindOutlineByTag()));
    QObject::connect(view->actionFindNoteByTag, SIGNAL(triggered()), mwp, SLOT(doActionFindNoteByTag()));

    // menu: view
    QObject::connect(
        view->actionViewTerminal, SIGNAL(triggered()),
        mwp, SLOT(doActionViewTerminal()));
    QObject::connect(
        view->actionViewLimbo, SIGNAL(triggered()),
        mwp, SLOT(doActionViewLimbo()));
    QObject::connect(view->actionViewHome, SIGNAL(triggered()), mwp, SLOT(doActionViewHome()));
    QObject::connect(view->actionViewOrganizers, SIGNAL(triggered()), mwp, SLOT(doActionViewOrganizers()));
    QObject::connect(view->actionViewOutlines, SIGNAL(triggered()), mwp, SLOT(doActionViewOutlines()));
    QObject::connect(view->actionViewOutlinesMap, SIGNAL(triggered()), mwp, SLOT(doActionViewOutlinesMap()));
    QObject::connect(view->actionViewTags, SIGNAL(triggered()), mwp, SLOT(doActionViewTagCloud()));
    QObject::connect(view->actionViewNavigator, SIGNAL(triggered()), mwp, SLOT(doActionViewKnowledgeGraphNavigator()));
    QObject::connect(view->actionViewCli, SIGNAL(triggered()), mwp, SLOT(doActionCli()));
    QObject::connect(view->actionViewRecentNotes, SIGNAL(triggered()), mwp, SLOT(doActionViewRecentNotes()));
    QObject::connect(view->actionViewHoist, SIGNAL(triggered()), mwp, SLOT(doActionNoteHoist()));
    QObject::connect(view->actionViewDistractionFree, SIGNAL(triggered()), mwp, SLOT(doActionViewDistractionFree()));
    QObject::connect(view->actionViewFullscreen, SIGNAL(triggered()), mwp, SLOT(doActionViewFullscreen()));

    // menu: navigator
    // IMPROVE complete edge shrinking & zooming (skipped as shortcut/mouse wheel is the much more reasonable)
    QObject::connect(view->actionNavigatorShuffle, SIGNAL(triggered()), mwp, SLOT(doActionNavigatorShuffle()));

    // menu: Library
    QObject::connect(view->actionLibraryAdd, SIGNAL(triggered()), mwp, SLOT(doActionLibraryNew()));
    QObject::connect(view->actionLibrarySync, SIGNAL(triggered()), mwp, SLOT(doActionLibrarySync()));
    QObject::connect(view->actionLibraryOrphans, SIGNAL(triggered()), mwp, SLOT(doActionLibraryOrphans()));
    QObject::connect(view->actionLibraryDeprecate, SIGNAL(triggered()), mwp, SLOT(doActionLibraryRm()));

    // menu: Organizer
    QObject::connect(
        view->actionOrganizerNew, SIGNAL(triggered()),
        mwp, SLOT(doActionOrganizerNew()));
    QObject::connect(
        view->actionOrganizerEdit, SIGNAL(triggered()),
        mwp, SLOT(doActionOrganizerEdit()));
    QObject::connect(
        view->actionOrganizerClone, SIGNAL(triggered()),
        mwp, SLOT(doActionOrganizerClone()));
    QObject::connect(
        view->actionOrganizerForget, SIGNAL(triggered()),
        mwp, SLOT(doActionOrganizerForget()));
    QObject::connect(
        view->actionOrganizerFocusPrevious, SIGNAL(triggered()),
        mwp, SLOT(doActionOrganizerFocusToPreviousVisibleQuadrant()));
    QObject::connect(
        view->actionOrganizerFocusNext, SIGNAL(triggered()),
        mwp, SLOT(doActionOrganizerFocusToNextVisibleQuadrant()));

    // menu: Outline
    QObject::connect(
        view->actionOutlineNew, SIGNAL(triggered()),
        mwp, SLOT(doActionOutlineNew()));
    QObject::connect(
        view->actionOutlineEdit, SIGNAL(triggered()),
        mwp, SLOT(doActionOutlineEdit()));
    QObject::connect(
        view->actionOutlineForget, SIGNAL(triggered()),
        mwp, SLOT(doActionOutlineForget()));
    QObject::connect(
        view->actionOutlineHome, SIGNAL(triggered()),
        mwp, SLOT(doActionOutlineHome()));
    QObject::connect(
        view->actionOutlineClone, SIGNAL(triggered()),
        mwp, SLOT(doActionOutlineClone()));
    QObject::connect(
        view->actionOutlinePromote, SIGNAL(triggered()),
        mwp, SLOT(doActionNotePromote()));
    QObject::connect(
        view->actionOutlineDemote, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteDemote()));
    QObject::connect(
        view->actionOutlineFirst, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteFirst()));
    QObject::connect(
        view->actionOutlineUp, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteUp()));
    QObject::connect(
        view->actionOutlineDown, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteDown()));
    QObject::connect(
        view->actionOutlineLast, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteLast()));
    QObject::connect(
        view->actionOutlineHtmlExport, SIGNAL(triggered()),
        mwp, SLOT(doActionOutlineHtmlExport()));
    QObject::connect(
        view->actionOutlineTWikiImport, SIGNAL(triggered()),
        mwp, SLOT(doActionOutlineTWikiImport()));
    // wingman: Outline
    QObject::connect(
        view->actionOutlineWingmanSummarize, SIGNAL(triggered()),
        mwp, SLOT(doActionWingmanOSummarize()));
    QObject::connect(
        view->actionOutlineWingmanExplain, SIGNAL(triggered()),
        mwp, SLOT(doActionWingmanOExplain()));
    QObject::connect(
        view->actionOutlineWingmanFind, SIGNAL(triggered()),
        mwp, SLOT(doActionWingmanOFind()));
    QObject::connect(
        view->actionOutlineWingmanMore, SIGNAL(triggered()),
        mwp, SLOT(handleActionWingman()));
    // wingman: Note
    QObject::connect(
        view->actionNoteWingmanSummarize, SIGNAL(triggered()),
        mwp, SLOT(doActionWingmanNSummarize()));
    QObject::connect(
        view->actionNoteWingmanFixGrammar, SIGNAL(triggered()),
        mwp, SLOT(doActionWingmanNFixGrammar()));
    QObject::connect(
        view->actionNoteWingmanRewrite, SIGNAL(triggered()),
        mwp, SLOT(doActionWingmanNRewrite()));
    QObject::connect(
        view->actionNoteWingmanMore, SIGNAL(triggered()),
        mwp, SLOT(handleActionWingman()));
    // wingman: Text (Edit)
    QObject::connect(
        view->actionEditWingmanFixGrammar, SIGNAL(triggered()),
        mwp, SLOT(doActionWingmanEFixGrammar()));
    QObject::connect(
        view->actionEditWingmanExplain, SIGNAL(triggered()),
        mwp, SLOT(doActionWingmanEExplain()));
    QObject::connect(
        view->actionEditWingmanFinishText, SIGNAL(triggered()),
        mwp, SLOT(doActionWingmanEFinishText()));
    QObject::connect(
        view->actionEditWingmanRewriteText, SIGNAL(triggered()),
        mwp, SLOT(doActionWingmanERewriteText()));
    QObject::connect(
        view->actionEditWingmanMore, SIGNAL(triggered()),
        mwp, SLOT(handleActionWingman()));

    // menu: Note
    QObject::connect(
        view->actionNoteNew, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteNew()));
    QObject::connect(
        view->actionNoteEdit, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteEdit()));
    QObject::connect(
        view->actionNoteExternalEdit, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteExternalEdit()));
    QObject::connect(
        view->actionNoteClose, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteLeave()));
    QObject::connect(
        view->actionNoteForget, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteForget()));
    QObject::connect(
        view->actionNotePromote, SIGNAL(triggered()),
        mwp, SLOT(doActionNotePromote()));
    QObject::connect(
        view->actionNoteDemote, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteDemote()));
    QObject::connect(
        view->actionNoteFirst, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteFirst()));
    QObject::connect(
        view->actionNoteUp, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteUp()));
    QObject::connect(
        view->actionNoteDown, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteDown()));
    QObject::connect(
        view->actionNoteLast, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteLast()));
    QObject::connect(
        view->actionNoteRefactor, SIGNAL(triggered()),
        mwp, SLOT(doActionRefactorNoteToOutline()));
    QObject::connect(
        view->actionNoteClone, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteClone()));

    // menu: edit
    QObject::connect(
        view->actionEditFind, SIGNAL(triggered()),
        mwp, SLOT(doActionEditFind()));
    QObject::connect(
        view->actionEditFindNext, SIGNAL(triggered()),
        mwp, SLOT(doActionEditFindAgain()));
    // no other bindings needed - it's already bound in the editor ~ menu is rather a documentation
    QObject::connect(
        view->actionEditWordWrap, SIGNAL(triggered()),
        mwp, SLOT(doActionEditWordWrapToggle()));
    QObject::connect(
        view->actionEditNameDescFocusSwap, SIGNAL(triggered()),
        mwp, SLOT(doActionNameDescFocusSwap()));
    QObject::connect(
        view->actionEditLiveNotePreview, SIGNAL(triggered()),
        mwp, SLOT(doActionToggleLiveNotePreview()));
    QObject::connect(
        view->actionEditExtract, SIGNAL(triggered()),
        mwp, SLOT(doActionNoteExtract()));
    QObject::connect(
        view->actionEditSpellCheck, SIGNAL(triggered()),
        mwp, SLOT(doActionSpellCheck()));

    // menu: format
    QObject::connect(
        view->actionFormatEmojis, SIGNAL(triggered()),
        mwp, SLOT(doActionEmojisDialog()));
    QObject::connect(view->actionFormatBold, SIGNAL(triggered()), mwp, SLOT(doActionFormatBold()));
    QObject::connect(view->actionFormatItalic, SIGNAL(triggered()), mwp, SLOT(doActionFormatItalic()));
    QObject::connect(view->actionFormatCode, SIGNAL(triggered()), mwp, SLOT(doActionFormatCode()));
    QObject::connect(view->actionFormatComment, SIGNAL(triggered()), mwp, SLOT(doActionFormatComment()));
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
    QObject::connect(view->actionFormatListTaskItem, SIGNAL(triggered()), mwp, SLOT(doActionFormatListTaskItem()));
    QObject::connect(view->actionFormatTocWithoutTags, SIGNAL(triggered()), mwp, SLOT(doActionFormatTocWithoutTags()));
    QObject::connect(view->actionFormatTocWithTags, SIGNAL(triggered()), mwp, SLOT(doActionFormatTocWithTags()));
    QObject::connect(view->actionFormatTimestamp, SIGNAL(triggered()), mwp, SLOT(doActionFormatTimestamp()));
    QObject::connect(view->actionFormatCodeBlock, SIGNAL(triggered()), mwp, SLOT(doActionFormatCodeBlock()));
    QObject::connect(view->actionFormatMathBlock, SIGNAL(triggered()), mwp, SLOT(doActionFormatMathBlock()));
    QObject::connect(view->actionFormatDiagramBlock, SIGNAL(triggered()), mwp, SLOT(doActionFormatDiagramBlock()));
    QObject::connect(view->actionFormatDiagramsPie, SIGNAL(triggered()), mwp, SLOT(doActionFormatDiagramPie()));
    QObject::connect(view->actionFormatDiagramsFlow, SIGNAL(triggered()), mwp, SLOT(doActionFormatDiagramFlow()));
    QObject::connect(view->actionFormatDiagramsClass, SIGNAL(triggered()), mwp, SLOT(doActionFormatDiagramClass()));
    QObject::connect(view->actionFormatDiagramsGantt, SIGNAL(triggered()), mwp, SLOT(doActionFormatDiagramGantt()));
    QObject::connect(view->actionFormatDiagramsState, SIGNAL(triggered()), mwp, SLOT(doActionFormatDiagramState()));
    QObject::connect(view->actionFormatDiagramsSequence, SIGNAL(triggered()), mwp, SLOT(doActionFormatDiagramSequence()));
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
    QObject::connect(view->actionHelpMathQuickReference, SIGNAL(triggered()), mwp, SLOT(doActionHelpMathQuickReference()));
    QObject::connect(view->actionHelpMathLivePreview, SIGNAL(triggered()), mwp, SLOT(doActionHelpMathLivePreview()));
    QObject::connect(view->actionHelpDiagrams, SIGNAL(triggered()), mwp, SLOT(doActionHelpDiagrams()));
    QObject::connect(
        view->actionHelpAboutQt, SIGNAL(triggered()),
        LookAndFeels::getInstance().getQApplication(), SLOT(aboutQt())
    );
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

void MainMenuPresenter::showFacetOrganizerList()
{
    view->showFacetOrganizerList(
        config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY,
        config.getActiveRepository()->getType()==Repository::RepositoryType::MINDFORGER
    );
}

void MainMenuPresenter::showFacetOrganizer()
{
    view->showFacetOrganizerView(
        config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY,
        config.getActiveRepository()->getType()==Repository::RepositoryType::MINDFORGER
    );
}

void MainMenuPresenter::showFacetNavigator()
{
    view->showFacetNavigator();
}

void MainMenuPresenter::showFacetOutlinesMap()
{
    view->showFacetOutlinesMap(
        config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY,
        config.getActiveRepository()->getType()==Repository::RepositoryType::MINDFORGER
    );
}

void MainMenuPresenter::showFacetOutlineList()
{
    view->showFacetOutlineList(
        config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY,
        config.getActiveRepository()->getType()==Repository::RepositoryType::MINDFORGER
    );
}

void MainMenuPresenter::showFacetOutlineView()
{
    view->showFacetOutlineView(
        config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY,
        config.getActiveRepository()->getType()==Repository::RepositoryType::MINDFORGER
    );
}

void MainMenuPresenter::showFacetNoteEdit()
{
    view->showFacetNoteEdit(
        config.getActiveRepository()->getMode()==Repository::RepositoryMode::REPOSITORY,
        config.getActiveRepository()->getType()==Repository::RepositoryType::MINDFORGER
    );
}

void MainMenuPresenter::showFacetMindThink()
{
    view->showFacetMindThink();
}
void MainMenuPresenter::showFacetMindSleep()
{
    view->showFacetMindSleep();
}

void MainMenuPresenter::showFacetMindAutolink(bool enabled)
{
    view->showFacetMindAutolink(enabled);
}

void MainMenuPresenter::showFacetLiveNotePreview(bool enabled)
{
    view->showFacetLiveNotePreview(enabled);
}

void MainMenuPresenter::addRecentDirectoryOrFile(const QString& path)
{
    view->addRepositoryOrFileToRelearn(path);
}

} // namespace
