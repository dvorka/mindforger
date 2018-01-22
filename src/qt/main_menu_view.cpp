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

    // TODO tr() for all menu items

    // menu: mind

#ifdef DO_MF_DEBUG
    actionMindHack = new QAction(QString::fromUtf8("Mind Hack"), mainWindow);
    actionMindHack->setStatusTip(QString::fromUtf8("Mind hacking and debugging hook"));
    actionMindHack->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_D));
#endif

    // devise... new MD repository
    actionMindDevise = new QAction(QString::fromUtf8("&Devise"), mainWindow);
    actionMindDevise->setStatusTip(QString::fromUtf8("Create new MD repository to form new combinations, applications, ideas or principles"));

    // learn... from a repository, Markdown or TXT file
    actionMindLearn = new QAction(QString::fromUtf8("&Learn"), mainWindow);
    // IMPROVE show RECENT repositories and files in the dialog
    actionMindLearn->setStatusTip(QString::fromUtf8("Learn notes by loading a MindForger repository, Markdown repository or file"));

    // remember... by flushing caches and saving unsaved notes
    actionMindRemember = new QAction(QString::fromUtf8("Re&member"), mainWindow);
    actionMindRemember->setStatusTip(QString::fromUtf8("Remember notes by ensuring memory persistence"));

    // remind ... smart mind search using FTS/associations/similarity
    actionMindRemind = new QAction(QString::fromUtf8("&Remind"), mainWindow);
    // IMPROVE show memory dwell as a base for reminding a note
    actionMindRemind->setStatusTip(QString::fromUtf8("Remind a Note by searching memory dwell, associations and similar notes"));

    // associate ... discover associated notes
    actionMindAssociate = new QAction(QString::fromUtf8("&Associate"), mainWindow);
    actionMindAssociate->setStatusTip(QString::fromUtf8("Discover related notes"));

    // think ... toggle mental processes ~ enable associations/similarity/search based suggestions on searching/reading/writing notes
    actionMindThink = new QAction(QString::fromUtf8("&Think"), mainWindow);
    actionMindThink->setStatusTip(QString::fromUtf8("Toggle ability to suggest matching, similar and associated notes on searching, reading and writing"));

    // forget ... don't show any note older than 1Y/3M/...
    actionMindForget = new QAction(QString::fromUtf8("&Forget"), mainWindow);
    actionMindForget->setStatusTip(QString::fromUtf8("Don't show notes older than..."));

    // dream ... sanity, integrity, detox, inference, assoc discovery, ...
    actionMindDream = new QAction(QString::fromUtf8("Dr&eam"), mainWindow);
    actionMindDream->setStatusTip(QString::fromUtf8("Tidy up, clean and optimize memory which is otherwise done on your inactivity"));

    actionMindSnapshot = new QAction(tr("D&ump"), mainWindow);
    actionMindSnapshot->setStatusTip(QString::fromUtf8("Create backup archive of the current repository"));

    actionMindPreferences = new QAction(QString::fromUtf8("&Preferences"), mainWindow);
    actionMindPreferences->setShortcuts(QKeySequence::Preferences);
    actionMindPreferences->setStatusTip(QString::fromUtf8("Open preferences..."));

    actionExit = new QAction(QString::fromUtf8("E&xit"), mainWindow);
    actionExit->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
    actionExit->setStatusTip(QString::fromUtf8("Leave application"));

    menuMindForger = qMenuBar->addMenu(QString::fromUtf8("&Mind"));
#ifdef DO_MF_DEBUG
    menuMindForger->addAction(actionMindHack);
#endif
    menuMindForger->addAction(actionMindDevise);
    menuMindForger->addAction(actionMindLearn);
    menuMindForger->addAction(actionMindRemember);
    menuMindForger->addAction(actionMindRemind);
    menuMindForger->addAction(actionMindAssociate);
    menuMindForger->addAction(actionMindThink);
    menuMindForger->addAction(actionMindForget);
    menuMindForger->addAction(actionMindDream);
    menuMindForger->addSeparator();
    menuMindForger->addAction(actionMindSnapshot);
    menuMindForger->addAction(actionMindPreferences);
    menuMindForger->addSeparator();
    menuMindForger->addAction(actionExit);

    // menu: recall

    // TODO tr()
    actionFts = new QAction(QString::fromUtf8("&Full-text Search"), mainWindow);
    actionFts->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_F));
    actionFts->setStatusTip(QString::fromUtf8("Note full-text search"));

    actionFindOutlineByName = new QAction(QString::fromUtf8("Recall &Outline by Name"), mainWindow);
    actionFindOutlineByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_O));
    actionFindOutlineByName->setStatusTip(QString::fromUtf8("Find outline by name"));

    actionFindNoteByName = new QAction(QString::fromUtf8("Recall &Note by Name"), mainWindow);
    actionFindNoteByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_N));
    actionFindNoteByName->setStatusTip(QString::fromUtf8("Find note by name"));

    actionFindTagByName = new QAction(QString::fromUtf8("Recall &Tag by Name"), mainWindow);
    actionFindTagByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_T));
    actionFindTagByName->setStatusTip(QString::fromUtf8("Find tag by name"));

    menuFind = qMenuBar->addMenu(QString::fromUtf8("&Recall"));
    menuFind->addAction(actionFts);
    menuFind->addAction(actionFindOutlineByName);
    menuFind->addAction(actionFindNoteByName);
    menuFind->addAction(actionFindTagByName);

    // menu: view
    actionViewHome = new QAction(QString::fromUtf8("&Home"), mainWindow);
    actionViewHome->setStatusTip(QString::fromUtf8("Open Home"));
    actionViewHome->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_H));

    actionViewOrganizer = new QAction(QString::fromUtf8("O&rganizer"), mainWindow);
    actionViewOrganizer->setStatusTip(QString::fromUtf8("Open Organizer"));
    //actionViewHome->setShortcut(QKeySequence(Qt::ALT+Qt::Key_X));

    actionViewOutlines = new QAction(QString::fromUtf8("&Outlines"), mainWindow);
    actionViewOutlines->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_O));
    actionViewOutlines->setStatusTip(QString::fromUtf8("Show list of Outlines"));

    actionViewNavigator = new QAction(QString::fromUtf8("&Navigator"), mainWindow);
    actionViewNavigator->setStatusTip(QString::fromUtf8("Open Navigator"));
    //actionViewHome->setShortcut(QKeySequence(Qt::ALT+Qt::Key_X));

    actionViewDwell = new QAction(QString::fromUtf8("&Memory Dwell"), mainWindow);
    actionViewDwell->setStatusTip(QString::fromUtf8("Open memory dwell"));

#ifdef DO_MF_DEBUG
    actionViewCli = new QAction(QString::fromUtf8("&CLI"), mainWindow);
    actionViewCli->setShortcut(QKeySequence(Qt::ALT+Qt::Key_X));
    actionViewCli->setStatusTip(QString::fromUtf8("Toggle command line"));
#else
    cliShortcut = new QShortcut(QKeySequence(Qt::ALT+Qt::Key_X), mainWindow);
    QObject::connect(cliShortcut, SIGNAL(activated()), this, SLOT(slotShowCli()));
#endif

    actionViewToggleRecent = new QAction(QString::fromUtf8("&Recent Notes"), mainWindow);
    actionViewToggleRecent->setStatusTip(QString::fromUtf8("View recently modified Notes"));
    actionViewToggleRecent->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));

    actionViewStencils = new QAction(QString::fromUtf8("&Stencils"), mainWindow);
    // Outline/Note marked w/ tag stencil is MOVED among stencils (NOT indexed/searched/...)
    //  + Stencil view allows making a stencil outline again
    //  + Note stencils are notes in a given Outline
    actionViewStencils->setStatusTip(QString::fromUtf8("List outline and note stencils..."));

    actionViewLimbo = new QAction(QString::fromUtf8("&Limbo"), mainWindow);
    actionViewLimbo->setStatusTip(QString::fromUtf8("List forgotten outlines and notes..."));

    actionViewDistractionFree = new QAction(QString::fromUtf8("&Distraction Free"), mainWindow);
    actionViewDistractionFree->setShortcut(QKeySequence(Qt::Key_F5));
    actionViewDistractionFree->setStatusTip(QString::fromUtf8("Toggle distraction free mode"));

    actionViewFullscreen = new QAction(QString::fromUtf8("&Fullscreen"), mainWindow);
    actionViewFullscreen->setShortcut(tr("F11"));
    actionViewFullscreen->setStatusTip(QString::fromUtf8("Fullscreen"));

    menuView = qMenuBar->addMenu(QString::fromUtf8("&View"));
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

    actionFormatBold = new QAction(QString::fromUtf8("&Bold"), mainWindow);
    //actionFormatBold->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatBold->setStatusTip(QString::fromUtf8("Format text as bold"));

    actionFormatItalic = new QAction(QString::fromUtf8("&Italic"), mainWindow);
    //actionFormatItalic->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatItalic->setStatusTip(QString::fromUtf8("Format text as italic"));

    actionFormatCode = new QAction(QString::fromUtf8("&Code"), mainWindow);
    //actionFormatCode->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatCode->setStatusTip(QString::fromUtf8("Format text as inlined source code"));

    actionFormatStrikethrough = new QAction(QString::fromUtf8("&Strikethrough"), mainWindow);
    //actionFormatStrikethrough->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatStrikethrough->setStatusTip(QString::fromUtf8("Format text as strikethrough"));

    actionFormatHighlight = new QAction(QString::fromUtf8("&Highlight"), mainWindow);
    //actionFormatHighlight->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatHighlight->setStatusTip(QString::fromUtf8("Format text as highlighted"));

    actionFormatSubscript = new QAction(QString::fromUtf8("Subscript"), mainWindow);
    //actionFormatSubscript->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatSubscript->setStatusTip(QString::fromUtf8("Format text as subscript"));

    actionFormatSuperscript = new QAction(QString::fromUtf8("Superscript"), mainWindow);
    //actionFormatSubscript->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatSuperscript->setStatusTip(QString::fromUtf8("Format text as superscript"));

    // ---

    actionFormatListBullet = new QAction(QString::fromUtf8("Bulleted List"), mainWindow);
    //actionFormatListBullet->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatListBullet->setStatusTip(QString::fromUtf8("Format block as bulleted list"));

    actionFormatListNumber = new QAction(QString::fromUtf8("Numbered List"), mainWindow);
    //actionFormatListNumber->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatListNumber->setStatusTip(QString::fromUtf8("Format block as numbered list"));

    // ---

    actionFormatCodeBlock = new QAction(QString::fromUtf8("C&ode Block"), mainWindow);
    //actionFormatCodeBlock->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatCodeBlock->setStatusTip(QString::fromUtf8("Format text block as source code"));

    actionFormatBlockQuote = new QAction(QString::fromUtf8("Block &Quote"), mainWindow);
    //actionFormatBlockQuote->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatBlockQuote->setStatusTip(QString::fromUtf8("Format text block as blockquote"));

    // ---

    actionFormatLink = new QAction(QString::fromUtf8("&Link"), mainWindow);
    //actionFormatLink->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatLink->setStatusTip(QString::fromUtf8("Insert link to a document, image or file"));

    actionFormatImage = new QAction(QString::fromUtf8("I&mage"), mainWindow);
    //actionFormatImage->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatImage->setStatusTip(QString::fromUtf8("Insert image"));

    actionFormatTable = new QAction(QString::fromUtf8("&Table"), mainWindow);
    //actionFormatTable->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
    actionFormatTable->setStatusTip(QString::fromUtf8("Insert table"));

    menuEdit = qMenuBar->addMenu(QString::fromUtf8("&Format"));
    menuEdit->addAction(actionFormatBold);
    menuEdit->addAction(actionFormatItalic);
    menuEdit->addAction(actionFormatCode);
    menuEdit->addAction(actionFormatStrikethrough);
    menuEdit->addAction(actionFormatHighlight);
    menuEdit->addAction(actionFormatSubscript);
    menuEdit->addAction(actionFormatSuperscript);
    menuEdit->addSeparator();
    menuEdit->addAction(actionFormatListBullet);
    menuEdit->addAction(actionFormatListNumber);
    menuEdit->addSeparator();
    menuEdit->addAction(actionFormatCodeBlock);
    menuEdit->addAction(actionFormatBlockQuote);
    menuEdit->addSeparator();
    menuEdit->addAction(actionFormatLink);
    menuEdit->addAction(actionFormatImage);
    menuEdit->addAction(actionFormatTable);

    // menu: outline

    actionOutlineNew = new QAction(QString::fromUtf8("&Devise"), mainWindow);
    actionOutlineNew->setStatusTip(QString::fromUtf8("Create new Outline to form new combinations, applications, ideas or principles"));

    actionOutlineOpen = new QAction(QString::fromUtf8("Rec&all"), mainWindow);
    actionOutlineOpen->setShortcut(tr("Ctrl-Shift-O"));
    actionOutlineOpen->setStatusTip(QString::fromUtf8("Open outline by name"));

    actionOutlineClone = new QAction(QString::fromUtf8("C&lone"), mainWindow);
    actionOutlineClone->setStatusTip(QString::fromUtf8("Make copy of the current outline..."));

    actionOutlineForget = new QAction(QString::fromUtf8("&Forget"), mainWindow);
    actionOutlineForget->setStatusTip(QString::fromUtf8("Forget Outline and move it to Limbo"));

    actionOutlineExport = new QAction(QString::fromUtf8("Dump"), mainWindow);
    actionOutlineExport->setStatusTip(QString::fromUtf8("Export Outline to a file in supported format"));

    actionOutlineImport = new QAction(QString::fromUtf8("Learn"), mainWindow);
    actionOutlineImport->setStatusTip(QString::fromUtf8("Import Outline from an external file in a supported format"));

    menuOutline = qMenuBar->addMenu(QString::fromUtf8("&Outline"));
    menuOutline->addAction(actionOutlineNew);
    menuOutline->addAction(actionOutlineOpen);
    menuOutline->addAction(actionOutlineForget);
    menuOutline->addSeparator();
    menuOutline->addAction(actionOutlineClone);
    menuOutline->addSeparator();
    menuOutline->addAction(actionOutlineExport);
    menuOutline->addAction(actionOutlineImport);

    // menu: note

    actionNoteNew = new QAction(QString::fromUtf8("&Devise"), mainWindow);
    actionNoteNew->setStatusTip(QString::fromUtf8("Create new note to form new combinations, applications, ideas or principles"));

    actionNoteOpen = new QAction(QString::fromUtf8("Rec&all"), mainWindow);
    actionNoteOpen->setStatusTip(QString::fromUtf8("Find note by name in the current outline..."));

    actionNoteSave = new QAction(QString::fromUtf8("Remember\tCtrl+S"), mainWindow);
    // Alt-Left is handled elsewhere and I don't want menu to handle it
    actionNoteSave->setStatusTip(QString::fromUtf8("Save note being edited"));

    actionNoteForget = new QAction(QString::fromUtf8("&Forget"), mainWindow);
    actionNoteForget->setStatusTip(QString::fromUtf8("Forget note"));

    actionNoteClose = new QAction(QString::fromUtf8("&Leave\tAlt+Left"), mainWindow);
    actionNoteClose->setStatusTip(QString::fromUtf8("Save leave editor of Note being changed"));

    actionNoteFirst = new QAction(QString::fromUtf8("&Top"), mainWindow);
    actionNoteFirst->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_T));
    actionNoteFirst->setStatusTip(QString::fromUtf8("Move note to be the first child of its parent"));

    actionNoteUp = new QAction(QString::fromUtf8("&Up"), mainWindow);
    actionNoteUp->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Up));
    actionNoteUp->setStatusTip(QString::fromUtf8("Move note up"));

    actionNotePromote = new QAction(QString::fromUtf8("&Promote"), mainWindow);
    actionNotePromote->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Right));
    actionNotePromote->setStatusTip(QString::fromUtf8("Promote note"));

    actionNoteDemote = new QAction(QString::fromUtf8("D&emote"), mainWindow);
    actionNoteDemote->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Left));
    actionNoteDemote->setStatusTip(QString::fromUtf8("Demote note"));

    actionNoteDown = new QAction(QString::fromUtf8("Do&wn"), mainWindow);
    actionNoteDown->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Down));
    actionNoteDown->setStatusTip(QString::fromUtf8("Move note down"));

    actionNoteLast = new QAction(QString::fromUtf8("&Bottom"), mainWindow);
    actionNoteLast->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_B));
    actionNoteLast->setStatusTip(QString::fromUtf8("Move note to be the last child of its parent"));

    // TODO Attach vs. COPY to repository
    actionNoteAttach = new QAction(QString::fromUtf8("&Attach"), mainWindow);
    actionNoteAttach->setStatusTip(QString::fromUtf8("Attach a document (by value) to note and create link to it..."));

    actionNoteRefactor = new QAction(QString::fromUtf8("&Refactor"), mainWindow);
    actionNoteRefactor->setStatusTip(QString::fromUtf8("Refactor note to another outline..."));

    actionNoteClone = new QAction(QString::fromUtf8("&Clone"), mainWindow);
    actionNoteClone->setStatusTip(QString::fromUtf8("Make a copy of the Note to this or other outline..."));

    actionNoteExport = new QAction(QString::fromUtf8("Dump"), mainWindow);
    actionNoteExport->setStatusTip(QString::fromUtf8("Export Note to an external file in a supported format"));

    actionNoteImport = new QAction(QString::fromUtf8("Learn"), mainWindow);
    actionNoteImport->setStatusTip(QString::fromUtf8("Import Note from an external file in a supported format"));

    menuNote = qMenuBar->addMenu(QString::fromUtf8("&Note"));
    menuNote->addAction(actionNoteNew);
    menuNote->addAction(actionNoteOpen);
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

    // menu: help

    actionHelpDocumentation = new QAction(QString::fromUtf8("&Documentation"), mainWindow);
    actionHelpDocumentation->setShortcut(tr("F1"));
    actionHelpDocumentation->setStatusTip(QString::fromUtf8("Open MindForger documentation"));

    actionHelpWeb = new QAction(QString::fromUtf8("&Web"), mainWindow);
    actionHelpWeb->setStatusTip(QString::fromUtf8("Open MindForger web"));

    actionHelpReportBug = new QAction(QString::fromUtf8("Report &Bug or Request Feature"), mainWindow);
    actionHelpReportBug->setStatusTip(QString::fromUtf8("Report bug or suggest an enhancement"));

    actionHelpCheckForUpdates = new QAction(QString::fromUtf8("&Check for Updates"), mainWindow);
    actionHelpCheckForUpdates->setStatusTip(QString::fromUtf8("Check for MindForger updates"));

    actionHelpAbout = new QAction(QString::fromUtf8("&About MindForger"), mainWindow);
    actionHelpAbout->setStatusTip(QString::fromUtf8("About MindForger..."));

    menuHelp = qMenuBar->addMenu(QString::fromUtf8("&Help"));
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

}
