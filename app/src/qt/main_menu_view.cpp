/*
 main_menu_view.cpp     MindForger thinking notebook

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
#include "main_menu_view.h"

namespace m8r {

MainMenuView::MainMenuView(MainWindowView& mainWindowView)
    : mainWindow(&mainWindowView)
{
    qMenuBar = mainWindow->getMenuBar();
    qMenuBar->setStyleSheet(LookAndFeels::getInstance().getMenuStylesheet());

    // menu: mind

#ifdef DO_M8F_DEBUG
    actionMindHack = new QAction(tr("Mind Hack"), mainWindow);
    actionMindHack->setStatusTip(tr("Mind hacking and debugging hook"));
    actionMindHack->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_D));
#endif

    // devise... new MD repository
    actionMindDevise = new QAction(tr("&New"), mainWindow);
    actionMindDevise->setStatusTip(tr("Create a brand new MindForger repository..."));
    actionMindDevise->setEnabled(false);

    // learn... from a repository, Markdown or TXT file
    actionMindLearn = new QAction(tr("&Learn"), mainWindow);
    actionMindLearn->setStatusTip(tr("Learn knowledge by loading a MindForger repository, Markdown repository or file"));

    // relearn ... recent repositories and files
    submenuMindRelearn = new RecentFilesMenu(tr("Relearn"), mainWindow);
    submenuMindRelearn->setStatusTip(tr("Relearn recently opened MindForger repositories, Markdown repositories or files"));
    submenuMindRelearn->setEnabled(false);

    // remember... by flushing caches, saving unsaved Ns, saving Os/Ns with changed read metadata, ...
    actionMindRemember = new QAction(tr("Re&member"), mainWindow);
    actionMindRemember->setStatusTip(tr("Remember Things by ensuring Memory persistence"));
    actionMindRemember->setEnabled(false);

    // remind ... smart *combined* (semantic) mind search using FTS/associations/... to find O or N
    actionMindRecall = new QAction(tr("&Recall"), mainWindow);
    // IMPROVE show memory dwell as a base for reminding a note
    actionMindRecall->setStatusTip(tr("Recall a Note by searching memory dwell, associations and similar Notes"));
    actionMindRecall->setEnabled(false);

    // think ... toggle mental processes ~ enable associations/similarity/search based suggestions on searching/reading/writing notes
    actionMindThink = new QAction(tr("&Think"), mainWindow);
    actionMindThink->setStatusTip(tr("Start to suggest matching, similar and associated Notes while searching, reading and writing"));

    // sleep ... opposite to think ~ stop mental processes and clear Mind
    actionMindSleep = new QAction(tr("&Sleep"), mainWindow);
    actionMindSleep->setStatusTip(tr("Stop sugggestions and clear Mind"));
    actionMindSleep->setEnabled(false);

    // scope ... don't show any N/O older than 1Y/3M/...
    actionMindScope = new QAction(tr("Sco&pe"), mainWindow);
    actionMindScope->setStatusTip(tr("Don't show Outlines and Notes older than..."));

    // TODO remove this menu item > make it Configuration option in Mind menu (Limbo or erase)
    // forget ... move to Limbo or erase
    actionMindForget = new QAction(tr("&Forget"), mainWindow);
    actionMindForget->setStatusTip(tr("Limbo vs erase memory..."));
    actionMindForget->setEnabled(false);

    // dream ... sanity, integrity, detox, inference, assoc discovery, ...
    //actionMindDream = new QAction(tr("&Dream"), mainWindow);
    //actionMindDream->setStatusTip(tr("Tidy up, clean, re-infer, check and optimize Memory which is otherwise done on your inactivity"));

    actionMindSnapshot = new QAction(tr("D&ump"), mainWindow);
    actionMindSnapshot->setStatusTip(tr("Create backup archive of the current repository and store it in home directory"));
    actionMindSnapshot->setEnabled(false);

    // TODO submenu: printer, HTML, PDF

    actionMindPreferences = new QAction(tr("&Adapt"), mainWindow);
    actionMindPreferences->setShortcuts(QKeySequence::Preferences);
    actionMindPreferences->setStatusTip(tr("Adapt Mind by setting your preferences..."));

    actionExit = new QAction(tr("E&xit"), mainWindow);
    actionExit->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
    actionExit->setStatusTip(tr("Leave application"));

    menuMind = qMenuBar->addMenu(tr("&Mind"));
    menuMind->addAction(actionMindDevise);
    menuMind->addAction(actionMindLearn);
    menuMind->addMenu(submenuMindRelearn);
    menuMind->addAction(actionMindRecall);
    menuMind->addAction(actionMindRemember);
    menuMind->addAction(actionMindScope);
    menuMind->addAction(actionMindForget);
    menuMind->addSeparator();
    menuMind->addAction(actionMindThink);
    menuMind->addAction(actionMindSleep);
    menuMind->addSeparator();
    menuMind->addAction(actionMindSnapshot);
    menuMind->addAction(actionMindPreferences);
    menuMind->addSeparator();
    menuMind->addAction(actionExit);
#ifdef DO_M8F_DEBUG
    menuMind->addSeparator();
    menuMind->addAction(actionMindHack);
#endif

    // menu: recall

    actionFts = new QAction(tr("&Full-text Search"), mainWindow);
    actionFts->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_F));
    actionFts->setStatusTip(tr("Note full-text search"));

    actionFindOutlineByName = new QAction(tr("Recall &Outline by Name"), mainWindow);
    actionFindOutlineByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_O));
    actionFindOutlineByName->setStatusTip(tr("Find Outline by name"));

    actionFindNoteByName = new QAction(tr("Recall &Note by Name"), mainWindow);
    actionFindNoteByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_N));
    actionFindNoteByName->setStatusTip(tr("Find Note by name"));

    actionFindOutlineByTag = new QAction(tr("Recall O&utline by Tags"), mainWindow);
    actionFindOutlineByTag->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_U));
    actionFindOutlineByTag->setStatusTip(tr("Find Outline by tags"));

    actionFindNoteByTag = new QAction(tr("Recall Note by &Tags"), mainWindow);
    actionFindNoteByTag->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_T));
    actionFindNoteByTag->setStatusTip(tr("Find Note by tags"));

    menuFind = qMenuBar->addMenu(tr("&Recall"));
    menuFind->addAction(actionFts);
    menuFind->addAction(actionFindOutlineByName);
    menuFind->addAction(actionFindNoteByName);
    menuFind->addAction(actionFindOutlineByTag);
    menuFind->addAction(actionFindNoteByTag);

    // menu: view
    actionViewHome = new QAction(tr("&Home"), mainWindow);
    actionViewHome->setStatusTip(tr("Open Home Outline"));
    actionViewHome->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_H));

    actionViewOrganizer = new QAction(tr("O&rganizer"), mainWindow);
    actionViewOrganizer->setStatusTip(tr("Open Organizer"));
    actionViewOrganizer->setEnabled(false);

    actionViewOutlines = new QAction(tr("&Outlines"), mainWindow);
    actionViewOutlines->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_O));
    actionViewOutlines->setStatusTip(tr("Show list of Outlines"));

    actionViewNavigator = new QAction(tr("&Navigator"), mainWindow);
    actionViewNavigator->setStatusTip(tr("Open Navigator"));
    actionViewNavigator->setEnabled(false);

    actionViewDwell = new QAction(tr("&Memory Dwell"), mainWindow);
    actionViewDwell->setStatusTip(tr("Open memory dwell"));
    actionViewDwell->setEnabled(false);

#ifdef DO_M8F_DEBUG
    actionViewCli = new QAction(tr("&CLI"), mainWindow);
    actionViewCli->setShortcut(QKeySequence(Qt::ALT+Qt::Key_X));
    actionViewCli->setStatusTip(tr("Toggle command line"));
#endif

    actionViewToggleRecent = new QAction(tr("&Recent Notes"), mainWindow);
    actionViewToggleRecent->setStatusTip(tr("View recently modified Notes"));
    actionViewToggleRecent->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
    actionViewToggleRecent->setEnabled(false);

    actionViewStencils = new QAction(tr("&Stencils"), mainWindow);
    // Outline/Note marked w/ tag stencil is MOVED among stencils (NOT indexed/searched/...)
    //  + Stencil view allows making a stencil outline again
    //  + Note stencils are notes in a given Outline
    actionViewStencils->setStatusTip(tr("List Outline and Note stencils"));
    actionViewStencils->setEnabled(false);

    actionViewLimbo = new QAction(tr("&Limbo"), mainWindow);
    actionViewLimbo->setStatusTip(tr("List forgotten Outlines and Notes"));
    actionViewLimbo->setEnabled(false);

    actionViewDistractionFree = new QAction(tr("&Distraction Free"), mainWindow);
    actionViewDistractionFree->setShortcut(QKeySequence(Qt::Key_F5));
    actionViewDistractionFree->setStatusTip(tr("Toggle distraction free mode"));
    actionViewDistractionFree->setEnabled(false);

    actionViewFullscreen = new QAction(tr("&Fullscreen"), mainWindow);
    actionViewFullscreen->setShortcut(tr("F11"));
    actionViewFullscreen->setStatusTip(tr("Fullscreen"));
    actionViewFullscreen->setEnabled(false);

    menuView = qMenuBar->addMenu(tr("&View"));
    menuView->addAction(actionViewHome);
    menuView->addAction(actionViewOrganizer);
    menuView->addAction(actionViewOutlines);
    menuView->addAction(actionViewStencils);
    menuView->addAction(actionViewNavigator);
    menuView->addAction(actionViewDwell);
#ifdef DO_M8F_DEBUG
    // OBSOLETE feature: menuView->addAction(actionViewCli);
#endif
    menuView->addAction(actionViewLimbo);
    menuView->addSeparator();
    menuView->addAction(actionViewToggleRecent);
    menuView->addSeparator();
    menuView->addAction(actionViewDistractionFree);
    menuView->addAction(actionViewFullscreen);

    // menu: format

    actionFormatBold = new QAction(tr("&Bold"), mainWindow);
    actionFormatBold->setStatusTip(tr("Format text as bold"));

    actionFormatItalic = new QAction(tr("&Italic"), mainWindow);
    actionFormatItalic->setStatusTip(tr("Format text as italic"));

    actionFormatCode = new QAction(tr("&Code"), mainWindow);
    actionFormatCode->setStatusTip(tr("Format text as inlined source code"));

    actionFormatStrikethrough = new QAction(tr("&Strikethrough"), mainWindow);
    actionFormatStrikethrough->setStatusTip(tr("Format text as strikethrough"));

    actionFormatHighlight = new QAction(tr("&Highlight"), mainWindow);
    actionFormatHighlight->setStatusTip(tr("Format text as highlighted"));

    actionFormatSubscript = new QAction(tr("Subscript"), mainWindow);
    actionFormatSubscript->setStatusTip(tr("Format text as subscript"));

    actionFormatSuperscript = new QAction(tr("Superscript"), mainWindow);
    actionFormatSuperscript->setStatusTip(tr("Format text as superscript"));

    // ---

    actionFormatListBullet = new QAction(tr("Bulleted List"), mainWindow);
    actionFormatListBullet->setStatusTip(tr("Format block as bulleted list"));

    actionFormatListNumber = new QAction(tr("Numbered List"), mainWindow);
    actionFormatListNumber->setStatusTip(tr("Format block as numbered list"));

    // ---

    actionFormatCodeBlock = new QAction(tr("C&ode Block"), mainWindow);
    actionFormatCodeBlock->setStatusTip(tr("Format text block as source code"));

    actionFormatBlockQuote = new QAction(tr("Block &Quote"), mainWindow);
    actionFormatBlockQuote->setStatusTip(tr("Format text block as blockquote"));

    // ---

    actionFormatLink = new QAction(tr("&Link"), mainWindow);
    actionFormatLink->setStatusTip(tr("Insert link to a document, image or file"));

    actionFormatImage = new QAction(tr("I&mage"), mainWindow);
    actionFormatImage->setStatusTip(tr("Insert image"));

    actionFormatTable = new QAction(tr("&Table"), mainWindow);
    actionFormatTable->setStatusTip(tr("Insert table"));

    menuFormat = qMenuBar->addMenu(tr("&Format"));
    menuFormat->addAction(actionFormatBold);
    menuFormat->addAction(actionFormatItalic);
    menuFormat->addAction(actionFormatCode);
    menuFormat->addAction(actionFormatStrikethrough);
    menuFormat->addAction(actionFormatHighlight);
    menuFormat->addAction(actionFormatSubscript);
    menuFormat->addAction(actionFormatSuperscript);
    menuFormat->addSeparator();
    menuFormat->addAction(actionFormatListBullet);
    menuFormat->addAction(actionFormatListNumber);
    menuFormat->addSeparator();
    menuFormat->addAction(actionFormatCodeBlock);
    menuFormat->addAction(actionFormatBlockQuote);
    menuFormat->addSeparator();
    menuFormat->addAction(actionFormatLink);
    menuFormat->addAction(actionFormatImage);
    menuFormat->addAction(actionFormatTable);
    menuFormat->setEnabled(false);

    // menu: Outline

    actionOutlineNew = new QAction(tr("&New"), mainWindow);
    actionOutlineNew->setStatusTip(tr("Create new Outline to form new ideas, principles, combinations or applications"));

    actionOutlineEdit = new QAction(tr("&Edit"), mainWindow);
    actionOutlineEdit ->setStatusTip(tr("Edit current Outline - you can also double click view to open the editor"));

    actionOutlineHome = new QAction(tr("Make &Home"), mainWindow);
    actionOutlineHome->setStatusTip(tr("Use the current Outline as home"));

    actionOutlineStencil = new QAction(tr("Make &Stencil"), mainWindow);
    actionOutlineStencil->setStatusTip(tr("Copy the current Outline as to Stencil"));
    actionOutlineStencil->setEnabled(false);

    actionOutlineClone = new QAction(tr("C&lone"), mainWindow);
    actionOutlineClone->setStatusTip(tr("Make copy of the current Outline"));

    actionOutlineForget = new QAction(tr("&Forget"), mainWindow);
    actionOutlineForget->setStatusTip(tr("Forget Outline and move it to Limbo"));

    actionOutlineExport = new QAction(tr("E&xport"), mainWindow);
    actionOutlineExport->setStatusTip(tr("Export Outline to a file in supported format"));
    actionOutlineExport->setEnabled(false);

    actionOutlineImport = new QAction(tr("&Learn"), mainWindow);
    actionOutlineImport->setStatusTip(tr("Import Outline from an external file in a supported format"));
    actionOutlineImport->setEnabled(false);

    menuOutline = qMenuBar->addMenu(tr("&Outline"));
    menuOutline->addAction(actionOutlineNew);
    menuOutline->addAction(actionOutlineEdit);
    menuOutline->addAction(actionOutlineForget);
    menuOutline->addSeparator();
    menuOutline->addAction(actionOutlineHome);
    menuOutline->addAction(actionOutlineStencil);
    menuOutline->addAction(actionOutlineClone);
    menuOutline->addSeparator();
    menuOutline->addAction(actionOutlineExport);
    menuOutline->addAction(actionOutlineImport);

    // menu: Note

    actionNoteNew = new QAction(tr("&New"), mainWindow);
    actionNoteNew->setStatusTip(tr("Create new Note to form new ideas, principles, combinations and applications"));

    actionNoteEdit = new QAction(tr("&Edit"), mainWindow);
    actionNoteEdit ->setStatusTip(tr("Edit current Note - you can also double click view to open the editor"));

    actionNoteSave = new QAction(tr("Remember\tCtrl+S"), mainWindow); // Ctrl+S is handled elsewhere and I don't want menu to handle it
    actionNoteSave->setStatusTip(tr("Save Note being edited"));

    actionNoteForget = new QAction(tr("&Forget\tDelete"), mainWindow); // Delete is handled elsewhere and I don't want menu to handle it
    actionNoteForget->setStatusTip(tr("Forget note"));

    actionNoteClose = new QAction(tr("Leave\tAlt+Left"), mainWindow); // Alt+Left is handled elsewhere and I don't want menu to handle it
    actionNoteClose->setStatusTip(tr("Save leave editor of Note being changed"));

    actionNotePromote = new QAction(tr("&Promote\tCtrl+Left"), mainWindow); // handled from Outline tree
    actionNotePromote->setStatusTip(tr("Promote Note"));

    actionNoteDemote = new QAction(tr("&Demote\tCtrl+Right"), mainWindow);
    actionNoteDemote->setStatusTip(tr("Demote Note"));

    actionNoteFirst = new QAction(tr("F&irst\tCtrl+Shift+Up"), mainWindow);
    actionNoteFirst->setStatusTip(tr("Move Note to be the first child of its parent"));

    actionNoteUp = new QAction(tr("&Up\tCtrl+Up"), mainWindow);
    actionNoteUp->setStatusTip(tr("Move Note up"));

    actionNoteDown = new QAction(tr("Do&wn\tCtrl+Down"), mainWindow);
    actionNoteDown->setStatusTip(tr("Move Note down"));

    actionNoteLast = new QAction(tr("&Last\tCtrl+Shift+Down"), mainWindow);
    actionNoteLast->setStatusTip(tr("Move Note to be the last child of its parent"));

    // TODO Attach vs. COPY to repository
    actionNoteAttach = new QAction(tr("&Attach"), mainWindow);
    actionNoteAttach->setStatusTip(tr("Attach a document (by value) to Note and create link to it..."));
    actionNoteAttach->setEnabled(false);

    actionNoteRefactor = new QAction(tr("&Refactor"), mainWindow);
    actionNoteRefactor->setStatusTip(tr("Refactor Note to another Outline..."));

    actionNoteExtract = new QAction(tr("E&xtract"), mainWindow);
    actionNoteExtract->setStatusTip(tr("Create new Note from the text selected in the current Note..."));

    actionNoteStencil = new QAction(tr("Make &Stencil"), mainWindow);
    actionNoteStencil->setStatusTip(tr("Copy the current Outline as to Stencil"));
    actionNoteStencil->setEnabled(false);

    actionNoteClone = new QAction(tr("&Clone"), mainWindow);
    actionNoteClone->setStatusTip(tr("Make a copy of the Note to this or other Outline..."));

    actionNoteExport = new QAction(tr("E&xport"), mainWindow);
    actionNoteExport->setStatusTip(tr("Export Note to an external file in a supported format"));
    actionNoteExport->setEnabled(false);

    actionNoteImport = new QAction(tr("Learn"), mainWindow);
    actionNoteImport->setStatusTip(tr("Import Note from an external file in a supported format"));
    actionNoteImport->setEnabled(false);

    menuNote = qMenuBar->addMenu(tr("&Note"));
    menuNote->addAction(actionNoteNew);
    menuNote->addAction(actionNoteEdit);
    menuNote->addAction(actionNoteSave);
    menuNote->addAction(actionNoteClose);
    menuNote->addAction(actionNoteForget);
    menuNote->addSeparator();
    menuNote->addAction(actionNotePromote);
    menuNote->addAction(actionNoteDemote);
    menuNote->addSeparator();
    menuNote->addAction(actionNoteFirst);
    menuNote->addAction(actionNoteUp);
    menuNote->addAction(actionNoteDown);
    menuNote->addAction(actionNoteLast);
    menuNote->addSeparator();
    menuNote->addAction(actionNoteAttach);
    menuNote->addAction(actionNoteRefactor);
    menuNote->addAction(actionNoteExtract);
    menuNote->addAction(actionNoteStencil);
    menuNote->addAction(actionNoteClone);
    menuNote->addSeparator();
    menuNote->addAction(actionNoteExport);
    menuNote->addAction(actionNoteImport);
    menuNote->setEnabled(false);

    // menu: help

    actionHelpDocumentation = new QAction(tr("&Documentation"), mainWindow);
    actionHelpDocumentation->setShortcut(tr("F1"));
    actionHelpDocumentation->setStatusTip(tr("Open MindForger documentation"));

    actionHelpWeb = new QAction(tr("&Web"), mainWindow);
    actionHelpWeb->setStatusTip(tr("Open MindForger web"));

    actionHelpReportBug = new QAction(tr("Report &Bug or Request Feature"), mainWindow);
    actionHelpReportBug->setStatusTip(tr("Report bug or suggest an enhancement"));

    actionHelpCheckForUpdates = new QAction(tr("&Check for Updates"), mainWindow);
    actionHelpCheckForUpdates->setStatusTip(tr("Check for MindForger updates"));

    actionHelpAbout = new QAction(tr("&About MindForger"), mainWindow);
    actionHelpAbout->setStatusTip(tr("About MindForger..."));

    menuHelp = qMenuBar->addMenu(tr("&Help"));
    menuHelp->addAction(actionHelpDocumentation);
    menuHelp->addAction(actionHelpWeb);
    menuHelp->addSeparator();
    menuHelp->addAction(actionHelpReportBug);
    menuHelp->addAction(actionHelpCheckForUpdates);
    menuHelp->addSeparator();
    menuHelp->addAction(actionHelpAbout);
}

MainMenuView::~MainMenuView()
{
    // TODO deletes (menus DO not delete > menuBar creates them)
}

void MainMenuView::showAllMenuItems()
{
    menuMind->setEnabled(true);    
    menuFind->setEnabled(true);    
    menuView->setEnabled(true);
    menuOutline->setEnabled(true);
    actionOutlineEdit->setEnabled(true);
    actionOutlineClone->setEnabled(true);
    actionOutlineHome->setEnabled(true);
    actionOutlineForget->setEnabled(true);
    menuNote->setEnabled(true);
    menuFormat->setEnabled(true);
    menuHelp->setEnabled(true);

    actionFindOutlineByName->setEnabled(true);
}

void MainMenuView::showFacetOutlineList(bool repositoryMode)
{
    showAllMenuItems();

    actionOutlineEdit->setEnabled(false);
    actionOutlineClone->setEnabled(false);
    actionOutlineHome->setEnabled(false);
    actionOutlineForget->setEnabled(false);
    menuFormat->setEnabled(false);
    menuNote->setEnabled(false);

    if(!repositoryMode) {
        menuView->setEnabled(false);
        menuOutline->setEnabled(false);
        menuFormat->setEnabled(false);

        actionFindOutlineByName->setEnabled(false);
        actionFindOutlineByTag->setEnabled(false);
    }
}

void MainMenuView::showFacetOutlineView(bool repositoryMode)
{
    showAllMenuItems();

    menuFormat->setEnabled(false);

    if(!repositoryMode) {
        menuView->setEnabled(false);
        menuOutline->setEnabled(false);
        menuFormat->setEnabled(false);

        actionFindOutlineByName->setEnabled(false);
        actionFindOutlineByTag->setEnabled(false);
    }
}

void MainMenuView::showFacetNoteEdit(bool repositoryMode)
{
    showAllMenuItems();

    if(!repositoryMode) {
        menuView->setEnabled(false);
        menuOutline->setEnabled(false);
        menuFormat->setEnabled(false);

        actionFindOutlineByName->setEnabled(false);
        actionFindOutlineByTag->setEnabled(false);
    }
}

void MainMenuView::showFacetMindThink()
{
    actionMindThink->setEnabled(false);
    actionMindSleep->setEnabled(true);
}

void MainMenuView::showFacetMindSleep()
{
    actionMindThink->setEnabled(true);
    actionMindSleep->setEnabled(false);
}

void MainMenuView::addRepositoryOrFileToRelearn(const QString& path)
{
    if(!submenuMindRelearn->isEnabled()) {
        submenuMindRelearn->setEnabled(true);
    }

    submenuMindRelearn->addFile(path);
}

} // m8r namespace
