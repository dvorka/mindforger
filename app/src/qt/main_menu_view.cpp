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

    menuMind = qMenuBar->addMenu(tr("&Mind"));

#ifdef DO_MF_DEBUG
    actionMindHack = new QAction(tr("Mind Hack"), mainWindow);
    actionMindHack->setStatusTip(tr("Mind hacking and debugging hook"));
    actionMindHack->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_D));
#endif

    // new/devise... new MD repository
    submenuMindNew = menuMind->addMenu("&New");
    actionMindNewRepository = new QAction(tr("MindForger &Repository"), mainWindow);
    actionMindNewRepository->setStatusTip(tr("Create a brand new MindForger repository..."));
    submenuMindNew->addAction(actionMindNewRepository);
    actionMindNewFile = new QAction(tr("Markdown &File"), mainWindow);
    actionMindNewFile->setStatusTip(tr("Create a brand new Markdown file..."));
    submenuMindNew->addAction(actionMindNewFile);

    // learn... from a repository, Markdown or TXT file
    submenuMindLearn = menuMind->addMenu(tr("&Learn"));
    actionMindLearnRepository = new QAction(tr("&Directory with Markdowns or MindForger Repository"), mainWindow);
    actionMindLearnRepository->setStatusTip(tr("Learn knowledge by loading a MindForger repository or a directory with Markdown files..."));
    submenuMindLearn->addAction(actionMindLearnRepository);
    actionMindLearnFile = new QAction(tr("Markdown &File"), mainWindow);
    actionMindLearnFile->setStatusTip(tr("Learn knowledge by loading a Markdown or MindForger file..."));
    submenuMindLearn->addAction(actionMindLearnFile);

    // re-learn/remind ... recent repositories and files
    submenuMindRelearn = new RecentFilesMenu(tr("&Remind"), mainWindow);
    submenuMindRelearn->setStatusTip(tr("Re-learn recently opened MindForger repositories, Markdown repositories or files"));
    submenuMindRelearn->setEnabled(false);

    // remember... by flushing caches, saving unsaved Ns, saving Os/Ns with changed read metadata, ...
    actionMindRemember = new QAction(tr("Re&member"), mainWindow);
    actionMindRemember->setStatusTip(tr("Persist all Things in Memory"));

    // recall ... smart *combined* (semantic) mind search using FTS/associations/... to find O or N
    //actionMindRecall = new QAction(tr("&Recall"), mainWindow);
    // IMPROVE show memory dwell as a base for reminding a note
    //actionMindRecall->setStatusTip(tr("Recall a Note by searching memory dwell, associations and similar Notes"));
    //actionMindRecall->setEnabled(false);

    // think ... toggle mental processes ~ enable associations/similarity/search based suggestions on searching/reading/writing notes
    actionMindThink = new QAction(tr("&Think"), mainWindow);
    actionMindThink->setStatusTip(tr("Start to suggest matching, similar and associated Notes while searching, reading and writing"));

    // sleep ... opposite to think ~ stop mental processes and clear Mind
    actionMindSleep = new QAction(tr("&Sleep"), mainWindow);
    actionMindSleep->setStatusTip(tr("Stop sugggestions and clear Mind"));

    // scope ... don't show any N/O older than 1Y/3M/...
    actionMindScope = new QAction(tr("S&cope"), mainWindow);
    actionMindScope->setStatusTip(tr("Don't show Notebooks and Notes older than..."));

    // TODO remove this menu item > make it Configuration option in Mind menu (Limbo or erase)
    // forget ... move to Limbo or erase
    actionMindForget = new QAction(tr("&Forget"), mainWindow);
    actionMindForget->setStatusTip(tr("Limbo vs erase memory..."));
    actionMindForget->setEnabled(false);

    // dream ... sanity, integrity, detox, inference, assoc discovery, ...
    //actionMindDream = new QAction(tr("&Dream"), mainWindow);
    //actionMindDream->setStatusTip(tr("Tidy up, clean, re-infer, check and optimize Memory which is otherwise done on your inactivity"));

    actionMindSnapshot = new QAction(tr("Retain"), mainWindow);
    actionMindSnapshot->setStatusTip(tr("Create backup archive of the current repository and store it in home directory"));
    actionMindSnapshot->setEnabled(false);

    // TODO submenu: printer, HTML, PDF

    actionMindPreferences = new QAction(tr("&Adapt"), mainWindow);
    actionMindPreferences->setShortcuts(QKeySequence::Preferences);
    actionMindPreferences->setStatusTip(tr("Adapt Mind by setting your preferences..."));

    actionExit = new QAction(tr("E&xit"), mainWindow);
    actionExit->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
    actionExit->setStatusTip(tr("Leave application"));

    menuMind->addMenu(submenuMindNew);
    menuMind->addMenu(submenuMindLearn);
    menuMind->addMenu(submenuMindRelearn);
    menuMind->addAction(actionMindScope);
    menuMind->addAction(actionMindThink);
    menuMind->addAction(actionMindSleep);
    menuMind->addAction(actionMindRemember);
#ifdef MF_WIP
    menuMind->addAction(actionMindSnapshot);
#endif
    menuMind->addAction(actionMindPreferences);
#ifdef MF_WIP
    menuMind->addAction(actionMindForget);
#endif
    menuMind->addSeparator();
    menuMind->addAction(actionExit);
#ifdef DO_MF_DEBUG
    menuMind->addSeparator();
    menuMind->addAction(actionMindHack);
#endif

    // menu: recall

    actionFts = new QAction(tr("&Full-text Search"), mainWindow);
    actionFts->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_F));
    actionFts->setStatusTip(tr("Note full-text search"));

    actionFindOutlineByName = new QAction(tr("Recall Note&book by Name"), mainWindow);
    actionFindOutlineByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_B));
    actionFindOutlineByName->setStatusTip(tr("Find Notebook by name"));

    actionFindNoteByName = new QAction(tr("Recall &Note by Name"), mainWindow);
    actionFindNoteByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_N));
    actionFindNoteByName->setStatusTip(tr("Find Note by name"));

    actionFindOutlineByTag = new QAction(tr("Recall Notebook by T&ags"), mainWindow);
    actionFindOutlineByTag->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_A));
    actionFindOutlineByTag->setStatusTip(tr("Find Notebook by tags"));

    actionFindNoteByTag = new QAction(tr("Recall Note by &Tags"), mainWindow);
    actionFindNoteByTag->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_T));
    actionFindNoteByTag->setStatusTip(tr("Find Note by tags"));

#ifdef MF_NER
    actionFindNerPersons = new QAction(tr("Recall &Persons"), mainWindow);
    actionFindNerPersons->setStatusTip(tr("Find persons using Named-entity recognition (NER)"));

    actionFindNerLocations = new QAction(tr("Recall &Locations"), mainWindow);
    actionFindNerLocations->setStatusTip(tr("Find locations using Named-entity recognition (NER)"));

    actionFindNerOrganizations = new QAction(tr("Recall Organizations"), mainWindow);
    actionFindNerOrganizations->setStatusTip(tr("Find organizations using Named-entity recognition (NER)"));

    actionFindNerMisc = new QAction(tr("Recall Other Entities"), mainWindow);
    actionFindNerMisc->setStatusTip(tr("Find miscellaneous entities using Named-entity recognition (NER)"));
#endif

    menuFind = qMenuBar->addMenu(tr("&Recall"));
    menuFind->addAction(actionFts);
    menuFind->addAction(actionFindOutlineByName);
    menuFind->addAction(actionFindNoteByName);
    menuFind->addAction(actionFindOutlineByTag);
    menuFind->addAction(actionFindNoteByTag);    
#ifdef MF_NER
    menuFind->addSeparator();
    menuFind->addAction(actionFindNerPersons);
    menuFind->addAction(actionFindNerLocations);
    menuFind->addAction(actionFindNerOrganizations);
    menuFind->addAction(actionFindNerMisc);
#endif

    // menu: view

    actionViewHome = new QAction(tr("&Home"), mainWindow);
    actionViewHome->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_H));
    actionViewHome->setStatusTip(tr("Open Home Notebook..."));

    actionViewOrganizer = new QAction(tr("&Eisenhower Matrix"), mainWindow);
    actionViewOrganizer->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_E));
    actionViewOrganizer->setStatusTip(tr("Open Eisenhower matrix..."));

    actionViewOutlines = new QAction(tr("N&otebooks"), mainWindow);
    actionViewOutlines->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_O));
    actionViewOutlines->setStatusTip(tr("Show list of Notebooks..."));

    actionViewTags = new QAction(tr("&Tags"), mainWindow);
    actionViewTags->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_G));
    actionViewTags->setStatusTip(tr("Open Tag cloud..."));

    actionViewNavigator = new QAction(tr("Knowledge Graph &Navigator"), mainWindow);
    actionViewNavigator->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_K));
    actionViewNavigator->setStatusTip(tr("Open knowledge graph Navigator..."));

    actionViewDwell = new QAction(tr("&Memory Dwell"), mainWindow);
    actionViewDwell->setStatusTip(tr("Open memory dwell..."));
    actionViewDwell->setEnabled(false);

#ifdef DO_MF_DEBUG
    actionViewCli = new QAction(tr("&CLI"), mainWindow);
    actionViewCli->setShortcut(QKeySequence(Qt::ALT+Qt::Key_X));
    actionViewCli->setStatusTip(tr("Toggle command line"));
#endif

    actionViewRecentNotes = new QAction(tr("&Recent Notes"), mainWindow);
    actionViewRecentNotes->setStatusTip(tr("View recently modified Notes..."));
    actionViewRecentNotes->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_R));

    actionViewStencils = new QAction(tr("&Stencils"), mainWindow);
    // Outline/Note marked w/ tag stencil is MOVED among stencils (NOT indexed/searched/...)
    //  + Stencil view allows making a stencil outline again
    //  + Note stencils are notes in a given Outline
    actionViewStencils->setStatusTip(tr("List Notebook and Note stencils..."));
    actionViewStencils->setEnabled(false);

    actionViewLimbo = new QAction(tr("&Limbo"), mainWindow);
    actionViewLimbo->setStatusTip(tr("List forgotten Notebooks and Notes..."));
    actionViewLimbo->setEnabled(false);

    actionViewDistractionFree = new QAction(tr("&Distraction Free"), mainWindow);
    actionViewDistractionFree->setShortcut(QKeySequence(Qt::Key_F5));
    actionViewDistractionFree->setStatusTip(tr("Toggle distraction free mode"));
    actionViewDistractionFree->setEnabled(false);

    actionViewFullscreen = new QAction(tr("&Fullscreen"), mainWindow);
    actionViewFullscreen->setShortcut(QKeySequence(Qt::Key_F11));
    actionViewFullscreen->setStatusTip(tr("Toggle fullscreen"));

    menuView = qMenuBar->addMenu(tr("&View"));
    menuView->addAction(actionViewHome);
    menuView->addAction(actionViewOrganizer);
    menuView->addAction(actionViewOutlines);
    menuView->addAction(actionViewTags);
#ifdef MF_WIP
    menuView->addAction(actionViewNavigator);
    menuView->addAction(actionViewStencils);
    menuView->addAction(actionViewDwell);
    // OBSOLETE feature: menuView->addAction(actionViewCli);
    menuView->addAction(actionViewLimbo);
#endif
    menuView->addSeparator();
    menuView->addAction(actionViewRecentNotes);
    menuView->addSeparator();
#ifdef MF_WIP
    menuView->addAction(actionViewDistractionFree);
#endif
    menuView->addAction(actionViewFullscreen);

    // menu: navigator

    actionNavigatorEdgesStretch = new QAction(tr("Str&etch edges\te | mouse wheel"), mainWindow);
    actionNavigatorEdgesStretch->setStatusTip(tr("Stretch knowledge graph edges"));

    actionNavigatorEdgesShrink= new QAction(tr("&Sh&rink edge\tE | mouse wheel"), mainWindow);
    actionNavigatorEdgesShrink->setStatusTip(tr("Shring knowledge graph edges"));

    actionNavigatorZoomIn = new QAction(tr("Zoom &in\tz"), mainWindow);
    actionNavigatorZoomIn->setStatusTip(tr("Zoom in knowledge graph"));

    actionNavigatorZoomOut = new QAction(tr("Zoom &out\tZ"), mainWindow);
    actionNavigatorZoomOut->setStatusTip(tr("Zoom out knowledge graph"));

    actionNavigatorShuffle = new QAction(tr("&Shuffle\tSpace"), mainWindow);
    actionNavigatorShuffle->setStatusTip(tr("Shuffle knowledge graph"));

    menuNavigator = qMenuBar->addMenu(tr("Na&vigator"));
    menuNavigator->addAction(actionNavigatorEdgesStretch);
    menuNavigator->addAction(actionNavigatorEdgesShrink);
    menuNavigator->addSeparator();
    menuNavigator->addAction(actionNavigatorZoomIn);
    menuNavigator->addAction(actionNavigatorZoomOut);
    menuNavigator->addSeparator();
    menuNavigator->addAction(actionNavigatorShuffle);
    menuNavigator->setEnabled(false);

    // menu: Outline

    menuOutline = qMenuBar->addMenu(tr("N&otebook"));

    actionOutlineNew = new QAction(tr("&New"), mainWindow);
    actionOutlineNew->setStatusTip(tr("Create new Notebook to form new ideas, principles, combinations or applications"));

#ifdef __APPLE__
    actionOutlineEdit = new QAction(tr("&Edit"), mainWindow);
    actionOutlineEdit->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
#else
    actionOutlineEdit = new QAction(tr("&Edit"), mainWindow);
#endif
    actionOutlineEdit ->setStatusTip(tr("Edit current Notebook - you can also double click view to open the editor"));

    actionOutlineHome = new QAction(tr("Make &Home"), mainWindow);
    actionOutlineHome->setStatusTip(tr("Toggle tag indicating whether to use the current Notebook as home"));

    actionOutlineStencil = new QAction(tr("Make &Stencil"), mainWindow);
    actionOutlineStencil->setStatusTip(tr("Copy the current Notebook as to Stencil"));
    actionOutlineStencil->setEnabled(false);

    actionOutlineClone = new QAction(tr("C&lone"), mainWindow);
    actionOutlineClone->setStatusTip(tr("Make copy of the current Notebook"));

    actionOutlineForget = new QAction(tr("&Forget"), mainWindow);
    actionOutlineForget->setStatusTip(tr("Forget Notebook and move it to Limbo"));

    submenuOutlineExport = menuOutline->addMenu("E&xport");
    actionOutlineHtmlExport = new QAction(tr("&HTML"), mainWindow);
    actionOutlineHtmlExport->setStatusTip(tr("Export Notebook to a file in HTML format"));
    // TODO Markdown
    // TODO Markdown with meta (status: Markdown with MindForger metadata
    submenuOutlineExport->addAction(actionOutlineHtmlExport);

    submenuOutlineImport = menuOutline->addMenu(tr("&Import"));
    actionOutlineTWikiImport= new QAction(tr("&TWiki"), mainWindow);
    actionOutlineTWikiImport->setStatusTip(tr("Import Notebook from an external TWiki file and restart MindForger"));
    submenuOutlineImport->addAction(actionOutlineTWikiImport);

    menuOutline->addAction(actionOutlineNew);
    menuOutline->addAction(actionOutlineEdit);
    menuOutline->addAction(actionOutlineForget);
    menuOutline->addSeparator();
    menuOutline->addAction(actionOutlineHome);
#ifdef MF_WIP
    menuOutline->addAction(actionOutlineStencil);
#endif
    menuOutline->addAction(actionOutlineClone);   
#ifdef MF_WIP
    menuOutline->addSeparator();
    menuOutline->addMenu(submenuOutlineExport);
    menuOutline->addMenu(submenuOutlineImport);
#endif

    // menu: Note

    actionNoteNew = new QAction(tr("&New"), mainWindow);
    actionNoteNew->setStatusTip(tr("Create new Note to form new ideas, principles, combinations and applications"));

    actionNoteHoist= new QAction(tr("&Hoist"), mainWindow);
    actionNoteHoist->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_I));
    actionNoteHoist->setStatusTip(tr("Hoist/de-hoist Note to focus on Note being viewed or edited"));

#ifdef __APPLE__
    actionNoteEdit = new QAction(tr("&Edit"), mainWindow);
    actionNoteEdit->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
#else
    actionNoteEdit = new QAction(tr("&Edit\tCtrl+E"), mainWindow);
#endif
    actionNoteEdit ->setStatusTip(tr("Edit current Note - you can also double click view to open the editor"));

    actionNoteSave = new QAction(tr("Remember\tCtrl+S"), mainWindow); // Ctrl+S is handled elsewhere and I don't want menu to handle it
    actionNoteSave->setStatusTip(tr("Save Note being edited"));

    actionNoteForget = new QAction(tr("&Forget\tDel"), mainWindow); // Delete is handled elsewhere and I don't want menu to handle it
    actionNoteForget->setStatusTip(tr("Forget Note"));

#ifdef __APPLE__
    actionNoteClose = new QAction(tr("Leave\t⌘+Left"), mainWindow); // Alt+Left is handled elsewhere and I don't want menu to handle it
#else
    actionNoteClose = new QAction(tr("Leave\tAlt+Left"), mainWindow); // Alt+Left is handled elsewhere and I don't want menu to handle it
#endif
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

    actionNoteRefactor = new QAction(tr("&Refactor"), mainWindow);
    actionNoteRefactor->setStatusTip(tr("Refactor Note to another Notebook..."));

    actionNoteExtract = new QAction(tr("E&xtract"), mainWindow);
    actionNoteExtract->setStatusTip(tr("Create new Note from the text selected in the current Note..."));

    actionNoteStencil = new QAction(tr("Make &Stencil"), mainWindow);
    actionNoteStencil->setStatusTip(tr("Copy the current Notebook as to Stencil"));
    actionNoteStencil->setEnabled(false);

    actionNoteClone = new QAction(tr("&Clone"), mainWindow);
    actionNoteClone->setStatusTip(tr("Make a copy of the Note to this or other Notebook..."));

    actionNoteExport = new QAction(tr("E&xport"), mainWindow);
    actionNoteExport->setStatusTip(tr("Export Note to an external file in a supported format"));
    actionNoteExport->setEnabled(false);

    actionNoteImport = new QAction(tr("Import"), mainWindow);
    actionNoteImport->setStatusTip(tr("Import Note from an external file in a supported format"));
    actionNoteImport->setEnabled(false);

    menuNote = qMenuBar->addMenu(tr("&Note"));
    menuNote->addAction(actionNoteNew);
    menuNote->addAction(actionNoteHoist);
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
    menuNote->addAction(actionNoteRefactor);
    menuNote->addAction(actionNoteExtract);
#ifdef MF_WIP
    menuNote->addAction(actionNoteStencil);
#endif
    menuNote->addAction(actionNoteClone);
#ifdef MF_WIP
    menuNote->addSeparator();
    menuNote->addAction(actionNoteExport);
    menuNote->addAction(actionNoteImport);
#endif
    menuNote->setEnabled(false);

    // menu: edit

    actionEditUndo= new QAction(tr("&Undo\tCtrl+Z"), mainWindow);
    actionEditUndo->setStatusTip(tr("Undo"));

    actionEditRedo= new QAction(tr("&Redo\tCtrl+Shift+Z"), mainWindow);
    actionEditRedo->setStatusTip(tr("Redo"));

    actionEditCut= new QAction(tr("Cu&t\tCtrl+X"), mainWindow);
    actionEditCut->setStatusTip(tr("Cut"));

    actionEditCopy= new QAction(tr("&Copy\tCtrl+C"), mainWindow);
    actionEditCopy->setStatusTip(tr("Copy"));

    actionEditPaste= new QAction(tr("&Paste\tCtrl+V"), mainWindow);
    actionEditPaste->setStatusTip(tr("Paste"));

    actionEditComplete= new QAction(tr("Complete Text\tAlt+/"), mainWindow);
    actionEditComplete->setStatusTip(tr("Complete word being written using other words in the current Note"));

    menuEdit = qMenuBar->addMenu(tr("&Edit"));
    menuEdit->addAction(actionEditUndo);
    menuEdit->addAction(actionEditRedo);
    menuEdit->addSeparator();
    menuEdit->addAction(actionEditCut);
    menuEdit->addAction(actionEditCopy);
    menuEdit->addAction(actionEditPaste);
    menuEdit->addSeparator();
    menuEdit->addAction(actionEditComplete);
    menuEdit->setEnabled(false);

    // menu: format

    actionFormatBold = new QAction(tr("&Bold"), mainWindow);
    actionFormatBold->setStatusTip(tr("Format text as bold"));

    actionFormatItalic = new QAction(tr("&Italic"), mainWindow);
    actionFormatItalic->setStatusTip(tr("Format text as italic"));

    actionFormatCode = new QAction(tr("&Code"), mainWindow);
    actionFormatCode->setStatusTip(tr("Format text as inlined source code"));

    actionFormatMath = new QAction(tr("&Math"), mainWindow);
    actionFormatMath->setStatusTip(tr("Format text as math (MathJax)"));

    actionFormatStrikethrough = new QAction(tr("&Strikethrough"), mainWindow);
    actionFormatStrikethrough->setStatusTip(tr("Format text as strikethrough"));

    actionFormatKeyboard = new QAction(tr("&Keyboard"), mainWindow);
    actionFormatKeyboard->setStatusTip(tr("Format text as keyboard input"));

    actionFormatListBullet = new QAction(tr("Bulleted List"), mainWindow);
    actionFormatListBullet->setStatusTip(tr("Format block as bulleted list"));

    actionFormatListNumber = new QAction(tr("Numbered List"), mainWindow);
    actionFormatListNumber->setStatusTip(tr("Format block as numbered list"));

    actionFormatListTask = new QAction(tr("Task List"), mainWindow);
    actionFormatListTask->setStatusTip(tr("Format block as task list"));

    actionFormatToc = new QAction(tr("T&able of Contents"), mainWindow);
    actionFormatToc ->setStatusTip(tr("Insert Notebook's table of contents"));

    actionFormatCodeBlock = new QAction(tr("C&ode Block"), mainWindow);
    actionFormatCodeBlock->setStatusTip(tr("Format text block as source code"));

    actionFormatMathBlock = new QAction(tr("Math Block"), mainWindow);
    actionFormatMathBlock->setStatusTip(tr("Format text block as math (MathJax)"));

    actionFormatBlockQuote = new QAction(tr("Block &Quote"), mainWindow);
    actionFormatBlockQuote->setStatusTip(tr("Format text block as blockquote"));

    actionFormatLink = new QAction(tr("&Link"), mainWindow);
    actionFormatLink->setStatusTip(tr("Insert link to a document, image or file"));

    actionFormatImage = new QAction(tr("Ima&ge"), mainWindow);
    actionFormatImage->setStatusTip(tr("Insert image"));

    actionFormatTable = new QAction(tr("&Table"), mainWindow);
    actionFormatTable->setStatusTip(tr("Insert table"));

    actionFormatHr = new QAction(tr("&Horizontal ruler"), mainWindow);
    actionFormatHr->setStatusTip(tr("Horizontal ruler"));

    menuFormat = qMenuBar->addMenu(tr("&Format"));
    menuFormat->addAction(actionFormatBold);
    menuFormat->addAction(actionFormatItalic);
    menuFormat->addAction(actionFormatCode);
    menuFormat->addAction(actionFormatMath);
    menuFormat->addAction(actionFormatStrikethrough);
    menuFormat->addAction(actionFormatKeyboard);
    menuFormat->addSeparator();
    menuFormat->addAction(actionFormatListBullet);
    menuFormat->addAction(actionFormatListNumber);
    menuFormat->addAction(actionFormatListTask);
    menuFormat->addSeparator();
    menuFormat->addAction(actionFormatToc);
    menuFormat->addAction(actionFormatCodeBlock);
    menuFormat->addAction(actionFormatMathBlock);
    menuFormat->addAction(actionFormatBlockQuote);
    menuFormat->addAction(actionFormatTable);
    menuFormat->addAction(actionFormatHr);
    menuFormat->addSeparator();
    menuFormat->addAction(actionFormatLink);
    menuFormat->addAction(actionFormatImage);
    menuFormat->setEnabled(false);

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
    menuEdit->setEnabled(true);
    menuOutline->setEnabled(true);
    menuNavigator->setEnabled(true);
    actionViewOrganizer->setEnabled(true);
    actionOutlineEdit->setEnabled(true);
    actionOutlineClone->setEnabled(true);
    actionOutlineHome->setEnabled(true);
    actionOutlineForget->setEnabled(true);
    menuNote->setEnabled(true);
    menuFormat->setEnabled(true);
    menuHelp->setEnabled(true);
    submenuOutlineExport->setEnabled(true);

    actionFindOutlineByName->setEnabled(true);
}

void MainMenuView::showFacetOutlineList(bool repositoryMode)
{
    showAllMenuItems();

    actionOutlineEdit->setEnabled(false);
    actionOutlineClone->setEnabled(false);
    actionOutlineHome->setEnabled(false);
    actionOutlineForget->setEnabled(false);
    menuNavigator->setEnabled(false);
    menuEdit->setEnabled(false);
    menuFormat->setEnabled(false);
    menuNote->setEnabled(false);
    submenuOutlineExport->setEnabled(false);

    if(!repositoryMode) {
        menuView->setEnabled(false);
        menuOutline->setEnabled(false);
        menuEdit->setEnabled(false);
        menuFormat->setEnabled(false);

        actionFindOutlineByName->setEnabled(false);
        actionFindOutlineByTag->setEnabled(false);
    }
}

void MainMenuView::showFacetOutlineView(bool repositoryMode)
{
    showAllMenuItems();

    menuNavigator->setEnabled(false);
    menuEdit->setEnabled(false);
    menuFormat->setEnabled(false);

    if(!repositoryMode) {
        menuView->setEnabled(false);
        menuOutline->setEnabled(false);
        menuEdit->setEnabled(false);
        menuFormat->setEnabled(false);

        actionFindOutlineByName->setEnabled(false);
        actionFindOutlineByTag->setEnabled(false);
    }
}

void MainMenuView::showFacetNoteEdit(bool repositoryMode)
{
    showAllMenuItems();

    menuNavigator->setEnabled(false);

    if(!repositoryMode) {
        menuView->setEnabled(false);
        menuOutline->setEnabled(false);
        menuEdit->setEnabled(false);

        actionFindOutlineByName->setEnabled(false);
        actionFindOutlineByTag->setEnabled(false);
    }
}

void MainMenuView::showFacetMindThink()
{
    actionMindThink->setVisible(false);
    actionMindSleep->setVisible(true);
}

void MainMenuView::showFacetMindSleep()
{
    actionMindThink->setVisible(true);
    actionMindSleep->setVisible(false);
}

void MainMenuView::showFacetNavigator()
{
    showAllMenuItems();

    menuNote->setEnabled(false);
    menuEdit->setEnabled(false);
    menuFormat->setEnabled(false);
}

void MainMenuView::addRepositoryOrFileToRelearn(const QString& path)
{
    if(!submenuMindRelearn->isEnabled()) {
        submenuMindRelearn->setEnabled(true);
    }

    submenuMindRelearn->addFile(path);
}

} // m8r namespace
