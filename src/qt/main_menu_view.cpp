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

    // menu: mind

#ifdef DO_MF_DEBUG
    actionMindHack = new QAction(tr("Mind Hack"), mainWindow);
    actionMindHack->setStatusTip(tr("Mind hacking and debugging hook"));
    actionMindHack->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_D));
#endif

    // devise... new MD repository
    actionMindDevise = new QAction(tr("&Devise"), mainWindow);
    actionMindDevise->setStatusTip(tr("Create new MD repository to form new combinations, applications, ideas or principles"));

    // learn... from a repository, Markdown or TXT file
    actionMindLearn = new QAction(tr("&Learn"), mainWindow);
    // IMPROVE show RECENT repositories and files in the dialog
    actionMindLearn->setStatusTip(tr("Learn notes by loading a MindForger repository, Markdown repository or file"));

    // remember... by flushing caches and saving unsaved notes
    actionMindRemember = new QAction(tr("Re&member"), mainWindow);
    actionMindRemember->setStatusTip(tr("Remember notes by ensuring memory persistence"));

    // remind ... smart mind search using FTS/associations/similarity
    actionMindRemind = new QAction(tr("&Remind"), mainWindow);
    // IMPROVE show memory dwell as a base for reminding a note
    actionMindRemind->setStatusTip(tr("Remind a Note by searching memory dwell, associations and similar notes"));

    // associate ... discover associated notes
    actionMindAssociate = new QAction(tr("&Associate"), mainWindow);
    actionMindAssociate->setStatusTip(tr("Discover Notes associated with current Outline or Note"));

    // think ... toggle mental processes ~ enable associations/similarity/search based suggestions on searching/reading/writing notes
    actionMindThink = new QAction(tr("&Think"), mainWindow);
    actionMindThink->setStatusTip(tr("Toggle suggestions of matching, similar and associated notes while searching, reading and writing"));

    // forget ... don't show any note older than 1Y/3M/...
    actionMindForget = new QAction(tr("&Forget"), mainWindow);
    actionMindForget->setStatusTip(tr("Don't show notes older than..."));

    // dream ... sanity, integrity, detox, inference, assoc discovery, ...
    actionMindDream = new QAction(tr("Dr&eam"), mainWindow);
    actionMindDream->setStatusTip(tr("Tidy up, clean and optimize memory which is otherwise done on your inactivity"));

    actionMindSnapshot = new QAction(tr("D&ump"), mainWindow);
    actionMindSnapshot->setStatusTip(tr("Create backup archive of the current repository"));

    actionMindPreferences = new QAction(tr("&Preferences"), mainWindow);
    actionMindPreferences->setShortcuts(QKeySequence::Preferences);
    actionMindPreferences->setStatusTip(tr("Open preferences"));

    actionExit = new QAction(tr("E&xit"), mainWindow);
    actionExit->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
    actionExit->setStatusTip(tr("Leave application"));

    menuMind = qMenuBar->addMenu(tr("&Mind"));
#ifdef DO_MF_DEBUG
    menuMind->addAction(actionMindHack);
#endif
    menuMind->addAction(actionMindDevise);
    menuMind->addAction(actionMindLearn);
    menuMind->addAction(actionMindRemember);
    menuMind->addAction(actionMindRemind);
    menuMind->addAction(actionMindAssociate);
    menuMind->addAction(actionMindThink);
    menuMind->addAction(actionMindForget);
    menuMind->addAction(actionMindDream);
    menuMind->addSeparator();
    menuMind->addAction(actionMindSnapshot);
    menuMind->addAction(actionMindPreferences);
    menuMind->addSeparator();
    menuMind->addAction(actionExit);

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

    actionFindOutlineByTag = new QAction(tr("Recall O&utline by Tag"), mainWindow);
    actionFindOutlineByTag->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_U));
    actionFindOutlineByTag->setStatusTip(tr("Find Outline by tag"));
    actionFindOutlineByTag->setEnabled(false);

    actionFindNoteByTag = new QAction(tr("Recall Note by &Tag"), mainWindow);
    actionFindNoteByTag->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_T));
    actionFindNoteByTag->setStatusTip(tr("Find Note by tag"));
    actionFindNoteByTag->setEnabled(false);

    menuFind = qMenuBar->addMenu(tr("&Recall"));
    menuFind->addAction(actionFts);
    menuFind->addAction(actionFindOutlineByName);
    menuFind->addAction(actionFindNoteByName);
    menuFind->addAction(actionFindOutlineByTag);
    menuFind->addAction(actionFindNoteByTag);

    // menu: view
    actionViewHome = new QAction(tr("&Home"), mainWindow);
    actionViewHome->setStatusTip(tr("Open Home"));
    actionViewHome->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_H));

    actionViewOrganizer = new QAction(tr("O&rganizer"), mainWindow);
    actionViewOrganizer->setStatusTip(tr("Open Organizer"));

    actionViewOutlines = new QAction(tr("&Outlines"), mainWindow);
    actionViewOutlines->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_O));
    actionViewOutlines->setStatusTip(tr("Show list of Outlines"));

    actionViewNavigator = new QAction(tr("&Navigator"), mainWindow);
    actionViewNavigator->setStatusTip(tr("Open Navigator"));

    actionViewDwell = new QAction(tr("&Memory Dwell"), mainWindow);
    actionViewDwell->setStatusTip(tr("Open memory dwell"));

#ifdef DO_MF_DEBUG
    actionViewCli = new QAction(tr("&CLI"), mainWindow);
    actionViewCli->setShortcut(QKeySequence(Qt::ALT+Qt::Key_X));
    actionViewCli->setStatusTip(tr("Toggle command line"));
#else
    cliShortcut = new QShortcut(QKeySequence(Qt::ALT+Qt::Key_X), mainWindow);
    QObject::connect(cliShortcut, SIGNAL(activated()), this, SLOT(slotShowCli()));
#endif

    actionViewToggleRecent = new QAction(tr("&Recent Notes"), mainWindow);
    actionViewToggleRecent->setStatusTip(tr("View recently modified Notes"));
    actionViewToggleRecent->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));

    actionViewStencils = new QAction(tr("&Stencils"), mainWindow);
    // Outline/Note marked w/ tag stencil is MOVED among stencils (NOT indexed/searched/...)
    //  + Stencil view allows making a stencil outline again
    //  + Note stencils are notes in a given Outline
    actionViewStencils->setStatusTip(tr("List Outline and Note stencils"));

    actionViewLimbo = new QAction(tr("&Limbo"), mainWindow);
    actionViewLimbo->setStatusTip(tr("List forgotten Outlines and Notes"));

    actionViewDistractionFree = new QAction(tr("&Distraction Free"), mainWindow);
    actionViewDistractionFree->setShortcut(QKeySequence(Qt::Key_F5));
    actionViewDistractionFree->setStatusTip(tr("Toggle distraction free mode"));

    actionViewFullscreen = new QAction(tr("&Fullscreen"), mainWindow);
    actionViewFullscreen->setShortcut(tr("F11"));
    actionViewFullscreen->setStatusTip(tr("Fullscreen"));

    menuView = qMenuBar->addMenu(tr("&View"));
    menuView->addAction(actionViewHome);
    menuView->addAction(actionViewOrganizer);
    menuView->addAction(actionViewOutlines);
    menuView->addAction(actionViewStencils);
    menuView->addAction(actionViewNavigator);
    menuView->addAction(actionViewDwell);
#ifdef DO_MF_DEBUG
    menuView->addAction(actionViewCli);
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

    // menu: outline

    actionOutlineNew = new QAction(tr("&Devise"), mainWindow);
    actionOutlineNew->setStatusTip(tr("Create new Outline to form new combinations, applications, ideas or principles"));

    actionOutlineClone = new QAction(tr("C&lone"), mainWindow);
    actionOutlineClone->setStatusTip(tr("Make copy of the current Outline"));

    actionOutlineForget = new QAction(tr("&Forget"), mainWindow);
    actionOutlineForget->setStatusTip(tr("Forget Outline and move it to Limbo"));

    actionOutlineExport = new QAction(tr("Dump"), mainWindow);
    actionOutlineExport->setStatusTip(tr("Export Outline to a file in supported format"));

    actionOutlineImport = new QAction(tr("Learn"), mainWindow);
    actionOutlineImport->setStatusTip(tr("Import Outline from an external file in a supported format"));

    menuOutline = qMenuBar->addMenu(tr("&Outline"));
    menuOutline->addAction(actionOutlineNew);
    menuOutline->addAction(actionOutlineForget);
    menuOutline->addSeparator();
    menuOutline->addAction(actionOutlineClone);
    menuOutline->addSeparator();
    menuOutline->addAction(actionOutlineExport);
    menuOutline->addAction(actionOutlineImport);

    // menu: note

    actionNoteNew = new QAction(tr("&Devise"), mainWindow);
    actionNoteNew->setStatusTip(tr("Create new Note to form new combinations, applications, ideas or principles"));

    actionNoteSave = new QAction(tr("Remember\tCtrl+S"), mainWindow); // Ctrl+S is handled elsewhere and I don't want menu to handle it
    actionNoteSave->setStatusTip(tr("Save Note being edited"));

    actionNoteForget = new QAction(tr("&Forget"), mainWindow);
    actionNoteForget->setStatusTip(tr("Forget note"));

    actionNoteClose = new QAction(tr("&Leave\tAlt+Left"), mainWindow); // Alt+Left is handled elsewhere and I don't want menu to handle it
    actionNoteClose->setStatusTip(tr("Save leave editor of Note being changed"));

    actionNoteFirst = new QAction(tr("&Top"), mainWindow);
    actionNoteFirst->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_T));
    actionNoteFirst->setStatusTip(tr("Move Note to be the first child of its parent"));

    actionNoteUp = new QAction(tr("&Up"), mainWindow);
    actionNoteUp->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Up));
    actionNoteUp->setStatusTip(tr("Move Note up"));

    actionNotePromote = new QAction(tr("&Promote"), mainWindow);
    actionNotePromote->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Right));
    actionNotePromote->setStatusTip(tr("Promote Note"));

    actionNoteDemote = new QAction(tr("D&emote"), mainWindow);
    actionNoteDemote->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Left));
    actionNoteDemote->setStatusTip(tr("Demote Note"));

    actionNoteDown = new QAction(tr("Do&wn"), mainWindow);
    actionNoteDown->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Down));
    actionNoteDown->setStatusTip(tr("Move Note down"));

    actionNoteLast = new QAction(tr("&Bottom"), mainWindow);
    actionNoteLast->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_B));
    actionNoteLast->setStatusTip(tr("Move Note to be the last child of its parent"));

    // TODO Attach vs. COPY to repository
    actionNoteAttach = new QAction(tr("&Attach"), mainWindow);
    actionNoteAttach->setStatusTip(tr("Attach a document (by value) to Note and create link to it..."));

    actionNoteRefactor = new QAction(tr("&Refactor"), mainWindow);
    actionNoteRefactor->setStatusTip(tr("Refactor Note to another outline..."));

    actionNoteClone = new QAction(tr("&Clone"), mainWindow);
    actionNoteClone->setStatusTip(tr("Make a copy of the Note to this or other outline..."));

    actionNoteExport = new QAction(tr("Dump"), mainWindow);
    actionNoteExport->setStatusTip(tr("Export Note to an external file in a supported format"));

    actionNoteImport = new QAction(tr("Learn"), mainWindow);
    actionNoteImport->setStatusTip(tr("Import Note from an external file in a supported format"));

    menuNote = qMenuBar->addMenu(tr("&Note"));
    menuNote->addAction(actionNoteNew);
    menuNote->addAction(actionNoteSave);
    menuNote->addAction(actionNoteClose);
    menuNote->addAction(actionNoteForget);
    menuNote->addSeparator();
    menuNote->addAction(actionNoteFirst);
    menuNote->addAction(actionNoteUp);
    menuNote->addAction(actionNotePromote);
    menuNote->addAction(actionNoteDemote);
    menuNote->addAction(actionNoteDown);
    menuNote->addAction(actionNoteLast);
    menuNote->addSeparator();
    menuNote->addAction(actionNoteAttach);
    menuNote->addAction(actionNoteRefactor);
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
    menuNote->setEnabled(true);
    menuFormat->setEnabled(true);
    menuHelp->setEnabled(true);
}

void MainMenuView::showFacetOutlineList()
{
    showAllMenuItems();
    menuFormat->setEnabled(false);
    menuNote->setEnabled(false);
}

void MainMenuView::showFacetOutlineView()
{
    showAllMenuItems();
    menuFormat->setEnabled(false);
}

void MainMenuView::showFacetNoteEdit()
{
    showAllMenuItems();
}

}
