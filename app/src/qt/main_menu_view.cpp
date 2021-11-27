/*
 main_menu_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
    actionMindHack->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_Z));
#endif

    // new/devise... new MD repository
    submenuMindNew = menuMind->addMenu(QIcon(":/menu-icons/new.svg"), "&New");
    actionMindNewRepository = new QAction(tr("MindForger &Repository"), mainWindow);
    actionMindNewRepository->setStatusTip(tr("Create a brand new MindForger repository..."));
    submenuMindNew->addAction(actionMindNewRepository);
    actionMindNewFile = new QAction(tr("Markdown &File"), mainWindow);
    actionMindNewFile->setStatusTip(tr("Create a brand new Markdown file..."));
    submenuMindNew->addAction(actionMindNewFile);

    // learn... from a repository, Markdown or TXT file
    submenuMindLearn = menuMind->addMenu(QIcon(":/menu-icons/open.svg"), tr("&Learn"));
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
    actionMindRemember = new QAction(QIcon(":/menu-icons/save.svg"), tr("Re&member"), mainWindow);
    actionMindRemember->setStatusTip(tr("Persist all Things in Memory"));

    // recall ... smart *combined* (semantic) mind search using FTS/associations/... to find O or N
    //actionMindRecall = new QAction(tr("&Recall"), mainWindow);
    // IMPROVE show memory dwell as a base for reminding a note
    //actionMindRecall->setStatusTip(tr("Recall a Note by searching memory dwell, associations and similar Notes"));
    //actionMindRecall->setEnabled(false);

    // think ... toggle mental processes ~ enable associations/similarity/search based suggestions on searching/reading/writing notes
    actionMindThink = new QAction(QIcon(":/menu-icons/on.svg"), tr("&Think"), mainWindow);
    actionMindThink->setCheckable(true);
    actionMindThink->setStatusTip(tr("Think to suggest matching, similar and associated Notes while searching, reading and writing"));

    // think ... toggle mental processes ~ enable associations/similarity/search based suggestions on searching/reading/writing notes
    actionMindAutolink = new QAction(QIcon(":/menu-icons/link.svg"), tr("&Autolink"), mainWindow);
#ifdef MF_MD_2_HTML_CMARK
    actionMindAutolink->setCheckable(true);
    actionMindAutolink->setStatusTip(tr("Automatically inject links to relevant Notebooks and Notes when browsing HTML preview"));
    actionMindAutolink->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_A));
#else
    actionMindAutolink->setVisible(false);
#endif

    // scope ... don't show any N/O older than 1Y/3M/...
    actionMindScope = new QAction(QIcon(":/menu-icons/filter.svg"), tr("S&cope"), mainWindow);
    actionMindScope->setStatusTip(tr("Don't show Notebooks and Notes older than..."));

    // TODO remove this menu item > make it Configuration option in Mind menu (Limbo or erase)
    // forget ... move to Limbo or erase
    actionMindForget = new QAction(QIcon(":/menu-icons/delete.svg"), tr("&Forget"), mainWindow);
    actionMindForget->setStatusTip(tr("Limbo vs erase memory..."));
    actionMindForget->setEnabled(false);

    // dream ... sanity, integrity, detox, inference, assoc discovery, ...
    //actionMindDream = new QAction(tr("&Dream"), mainWindow);
    //actionMindDream->setStatusTip(tr("Tidy up, clean, re-infer, check and optimize Memory which is otherwise done on your inactivity"));

    actionMindSnapshot = new QAction(QIcon(":/menu-icons/pin.svg"), tr("Retain"), mainWindow);
    actionMindSnapshot->setStatusTip(tr("Create backup archive of the current repository and store it in home directory"));
    actionMindSnapshot->setEnabled(false);

    // TODO submenu: printer, HTML, PDF

    actionMindPreferences = new QAction(QIcon(":/menu-icons/configure.svg"), tr("A&dapt"), mainWindow);
    actionMindPreferences->setShortcuts(QKeySequence::Preferences);
    actionMindPreferences->setStatusTip(tr("Adapt Mind by setting your preferences..."));

    actionMindTerminal = new QAction(QIcon(":/menu-icons/cli.svg"), tr("Term&inal"), mainWindow);
    actionMindTerminal->setStatusTip(tr("Run simple command line from current MindForger repository..."));

    submenuMindExport = menuMind->addMenu(QIcon(":/menu-icons/export.svg"), "&Export");
    actionMindExportCsv = new QAction(tr("&CSV"), mainWindow);
    actionMindExportCsv->setStatusTip(tr("Export all Notebooks/Markdown files as a single CSV file"));
    submenuMindExport->addAction(actionMindExportCsv);

    actionExit = new QAction(QIcon(":/menu-icons/exit.svg"), tr("E&xit"), mainWindow);
    actionExit->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
    actionExit->setStatusTip(tr("Leave application"));

    menuMind->addMenu(submenuMindNew);
    menuMind->addMenu(submenuMindLearn);
    menuMind->addMenu(submenuMindRelearn);
    menuMind->addAction(actionMindScope);
    menuMind->addAction(actionMindThink);
    menuMind->addAction(actionMindAutolink);
    menuMind->addAction(actionMindRemember);
#ifdef MF_WIP
    menuMind->addAction(actionMindSnapshot);
#endif
    menuMind->addAction(actionMindPreferences);
#ifdef MF_WIP
    menuMind->addAction(actionMindForget);
    menuMind->addSeparator();
#endif
    menuMind->addMenu(submenuMindExport);
    menuMind->addSeparator();
    menuMind->addAction(actionMindTerminal);
    menuMind->addSeparator();
    menuMind->addAction(actionExit);
#ifdef DO_MF_DEBUG
    menuMind->addSeparator();
    menuMind->addAction(actionMindHack);
#endif

    // menu: recall

    actionFindFts = new QAction(QIcon(":/menu-icons/find.svg"), tr("&Full-text Search"), mainWindow);
    actionFindFts->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_F));
    actionFindFts->setStatusTip(tr("Note full-text search"));

    actionFindOutlineByName = new QAction(QIcon(":/menu-icons/find.svg"), tr("Recall Note&book by Name"), mainWindow);
    actionFindOutlineByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_B));
    actionFindOutlineByName->setStatusTip(tr("Find Notebook by name"));

    actionFindNoteByName = new QAction(QIcon(":/menu-icons/find.svg"), tr("Recall &Note by Name"), mainWindow);
    actionFindNoteByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_N));
    actionFindNoteByName->setStatusTip(tr("Find Note by name"));

    actionFindOutlineByTag = new QAction(QIcon(":/menu-icons/find.svg"), tr("Recall Notebook by Ta&gs"), mainWindow);
    actionFindOutlineByTag->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_X));
    actionFindOutlineByTag->setStatusTip(tr("Find Notebook by tags"));

    actionFindNoteByTag = new QAction(QIcon(":/menu-icons/find.svg"), tr("Recall Note by &Tags"), mainWindow);
    actionFindNoteByTag->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_T));
    actionFindNoteByTag->setStatusTip(tr("Find Note by tags"));

#ifdef MF_WIP
    actionFindDocByName = new QAction(QIcon(":/menu-icons/find.svg"), tr("Recall Library &Doc by Name"), mainWindow);
    actionFindDocByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_D));
    actionFindDocByName->setStatusTip(tr("Find Document by name"));
#endif

#ifdef MF_NER
    actionFindNerPersons = new QAction(QIcon(":/menu-icons/find.svg"), tr("Recall &Persons"), mainWindow);
    actionFindNerPersons->setStatusTip(tr("Find persons using Named-entity recognition (NER)"));

    actionFindNerLocations = new QAction(QIcon(":/menu-icons/find.svg"), tr("Recall &Locations"), mainWindow);
    actionFindNerLocations->setStatusTip(tr("Find locations using Named-entity recognition (NER)"));

    actionFindNerOrganizations = new QAction(QIcon(":/menu-icons/find.svg"), tr("Recall Organizations"), mainWindow);
    actionFindNerOrganizations->setStatusTip(tr("Find organizations using Named-entity recognition (NER)"));

    actionFindNerMisc = new QAction(QIcon(":/menu-icons/find.svg"), tr("Recall Other Entities"), mainWindow);
    actionFindNerMisc->setStatusTip(tr("Find miscellaneous entities using Named-entity recognition (NER)"));
#endif

    menuFind = qMenuBar->addMenu(tr("&Recall"));
    menuFind->addAction(actionFindFts);
    menuFind->addAction(actionFindOutlineByName);
    menuFind->addAction(actionFindNoteByName);
    menuFind->addAction(actionFindOutlineByTag);
    menuFind->addAction(actionFindNoteByTag);    
#ifdef MF_WIP
    menuFind->addAction(actionFindDocByName);
#endif
#ifdef MF_NER
    menuFind->addSeparator();
    menuFind->addAction(actionFindNerPersons);
    menuFind->addAction(actionFindNerLocations);
    menuFind->addAction(actionFindNerOrganizations);
    menuFind->addAction(actionFindNerMisc);
#endif

    // menu: view

    actionViewDashboard = new QAction(QIcon(":/menu-icons/dashboard.svg"), tr("Dashboard"), mainWindow);
    actionViewDashboard->setStatusTip(tr("Open Dashboard..."));

    actionViewHome = new QAction(QIcon(":/menu-icons/home.svg"), tr("&Home Notebook"), mainWindow);
    actionViewHome->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_H));
    actionViewHome->setStatusTip(tr("Open Home Notebook..."));

    actionViewDecks = new QAction(QIcon(":/menu-icons/tag.svg"), tr("Flashcard &Decks"), mainWindow);
    actionViewDecks->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_E));
    actionViewDecks->setStatusTip(tr("Show list of flashcard decks..."));

    actionViewOrganizers = new QAction(QIcon(":/menu-icons/cross.svg"), tr("Organiz&ers"), mainWindow);
    actionViewOrganizers->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_E));
    actionViewOrganizers->setStatusTip(tr("Open Eisenhower matrix and Kanban organizers..."));

    actionViewOutlines = new QAction(QIcon(":/menu-icons/list-text.svg"), tr("N&otebooks"), mainWindow);
    actionViewOutlines->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_O));
    actionViewOutlines->setStatusTip(tr("Show list of Notebooks..."));

    actionViewLibraryDocs = new QAction(QIcon(":/menu-icons/copy.svg"), tr("&Library Documents"), mainWindow);
    actionViewLibraryDocs->setStatusTip(tr("List Library documents..."));
    actionViewLibraryDocs->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_L));

    actionViewTags = new QAction(QIcon(":/menu-icons/tag.svg"), tr("&Tags"), mainWindow);
    actionViewTags->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_G));
    actionViewTags->setStatusTip(tr("Open Tag cloud..."));

    actionViewNavigator = new QAction(QIcon(":/menu-icons/grid.svg"), tr("Knowledge Graph &Navigator"), mainWindow);
    actionViewNavigator->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_K));
    actionViewNavigator->setStatusTip(tr("Open knowledge graph Navigator..."));

    actionViewDwell = new QAction(tr("&Memory Dwell"), mainWindow);
    actionViewDwell->setStatusTip(tr("Open memory dwell..."));
    actionViewDwell->setEnabled(false);

    actionViewCli = new QAction(QIcon(":/menu-icons/cli.svg"), tr("&CLI"), mainWindow);
    actionViewCli->setShortcut(QKeySequence(Qt::ALT+Qt::Key_X));
    actionViewCli->setStatusTip(tr("Activate command line interface..."));

    actionViewRecentNotes = new QAction(QIcon(":/menu-icons/open-recent.svg"), tr("&Recent Notes"), mainWindow);
    actionViewRecentNotes->setStatusTip(tr("View recently modified Notes..."));
    actionViewRecentNotes->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_R));

    actionViewStencils = new QAction(QIcon(":/menu-icons/stencil.svg"), tr("&Stencils"), mainWindow);
    // Outline/Note marked w/ tag stencil is MOVED among stencils (NOT indexed/searched/...)
    //  + Stencil view allows making a stencil outline again
    //  + Note stencils are notes in a given Outline
    actionViewStencils->setStatusTip(tr("List Notebook and Note stencils..."));
    actionViewStencils->setEnabled(false);

    actionViewLimbo = new QAction(QIcon(":/menu-icons/limbo.svg"), tr("Li&mbo"), mainWindow);
    actionViewLimbo->setStatusTip(tr("List forgotten Notebooks and Notes..."));
    actionViewLimbo->setEnabled(false);
    // TODO same handler as Help/Documentation - open dir w/ limbo files

    actionViewHoist = new QAction(QIcon(":/menu-icons/hoisting.svg"), tr("Ho&isting"), mainWindow);
    actionViewHoist->setCheckable(true);
    actionViewHoist->setChecked(false);
    actionViewHoist->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_I));
    actionViewHoist->setStatusTip(tr("Hoist/de-hoist Note to focus on Note being viewed or edited"));

    actionViewDistractionFree = new QAction(QIcon(":/menu-icons/off.svg"), tr("D&istraction Free"), mainWindow);
    actionViewDistractionFree->setShortcut(QKeySequence(Qt::Key_F5));
    actionViewDistractionFree->setStatusTip(tr("Toggle distraction free mode"));
    actionViewDistractionFree->setEnabled(false);

    actionViewFullscreen = new QAction(QIcon(":/menu-icons/fullscreen.svg"), tr("&Fullscreen"), mainWindow);
    actionViewFullscreen->setShortcut(QKeySequence(Qt::Key_F11));
    actionViewFullscreen->setStatusTip(tr("Toggle fullscreen"));

    menuView = qMenuBar->addMenu(tr("&View"));
#ifdef MF_WIP
    menuView->addAction(actionViewDashboard);
#endif
    menuView->addAction(actionViewHome);
#ifdef MF_WIP
    menuView->addAction(actionViewDecks);
#endif
    menuView->addAction(actionViewOrganizers);
    menuView->addAction(actionViewOutlines);
#ifdef MF_WIP
    menuView->addAction(actionViewLibraryDocs);
#endif
    menuView->addAction(actionViewTags);
    menuView->addAction(actionViewNavigator);
    menuView->addAction(actionViewCli);
#ifdef MF_WIP
    menuView->addAction(actionViewStencils);
    menuView->addAction(actionViewDwell);
    menuView->addAction(actionViewLimbo);
#endif
    menuView->addSeparator();
    menuView->addAction(actionViewRecentNotes);
    menuView->addSeparator();
    menuView->addAction(actionViewHoist);
#ifdef MF_WIP
    menuView->addAction(actionViewDistractionFree);
#endif
    menuView->addAction(actionViewFullscreen);

    // menu: knowledge
#ifdef MF_WIP_KNOW
    menuKnowledge = qMenuBar->addMenu(tr("&Know"));

    actionKnowledgeWikipedia = new QAction(QIcon(":/menu-icons/link.svg"), tr("&Wikipedia"), mainWindow);
    actionKnowledgeWikipedia->setStatusTip(tr("Find marked text on Wikipedia or open Wikipedia search"));

    actionKnowledgeArxiv = new QAction(QIcon(":/menu-icons/link.svg"), tr("&arXiv"), mainWindow);
    actionKnowledgeArxiv->setStatusTip(tr("Find marked text on arXiv or get article by ID"));

    menuKnowledge->addAction(actionKnowledgeWikipedia);
    menuKnowledge->addAction(actionKnowledgeArxiv);
#endif

    // menu: navigator
#ifdef __APPLE__
    actionNavigatorEdgesStretch = new QAction(tr("Str&etch edges"), mainWindow);
#else
    actionNavigatorEdgesStretch = new QAction(tr("Str&etch edges\te | mouse wheel"), mainWindow);
#endif
    actionNavigatorEdgesStretch->setStatusTip(tr("Stretch knowledge graph edges"));

#ifdef __APPLE__
    actionNavigatorEdgesShrink= new QAction(tr("&Sh&rink edge"), mainWindow);
#else
    actionNavigatorEdgesShrink= new QAction(tr("&Sh&rink edge\tE | mouse wheel"), mainWindow);
#endif
    actionNavigatorEdgesShrink->setStatusTip(tr("Shring knowledge graph edges"));

    actionNavigatorZoomIn = new QAction(tr("Zoom &in\tz"), mainWindow);
    actionNavigatorZoomIn->setStatusTip(tr("Zoom in knowledge graph"));

    actionNavigatorZoomOut = new QAction(tr("Zoom &out\tZ"), mainWindow);
    actionNavigatorZoomOut->setStatusTip(tr("Zoom out knowledge graph"));

    actionNavigatorShuffle = new QAction(tr("&Shuffle\tSpace"), mainWindow);
    actionNavigatorShuffle->setStatusTip(tr("Shuffle knowledge graph"));

    menuNavigator = qMenuBar->addMenu(tr("Na&vigate"));
    menuNavigator->addAction(actionNavigatorEdgesStretch);
    menuNavigator->addAction(actionNavigatorEdgesShrink);
    menuNavigator->addSeparator();
    menuNavigator->addAction(actionNavigatorZoomIn);
    menuNavigator->addAction(actionNavigatorZoomOut);
    menuNavigator->addSeparator();
    menuNavigator->addAction(actionNavigatorShuffle);
    menuNavigator->setEnabled(false);

    // menu: library
#ifdef MF_WIP
    menuLibrary = qMenuBar->addMenu(tr("Libr&ary"));

    actionLibraryAdd = new QAction(QIcon(":/menu-icons/new.svg"), tr("&Add library"), mainWindow);
    actionLibraryAdd->setStatusTip(tr("Add directory with documents, URL or other resource to library..."));

    actionLibraryDeprecate = new QAction(QIcon(":/menu-icons/delete.svg"), tr("&Deprecate library"), mainWindow);
    actionLibraryDeprecate->setStatusTip(tr("Move a library resource with documents to limbo..."));
    actionLibraryDeprecate->setDisabled(true);

    menuLibrary->addAction(actionLibraryAdd);
    // menuLibrary->addAction(actionLibraryDeprecate);
#endif

    // menu: flashcards
#ifdef MF_WIP
    menuFlashcards = qMenuBar->addMenu(tr("Flash&cards"));
    menuFlashcards->setDisabled(true);
#endif

    // menu: organizer
    menuOrganizer = qMenuBar->addMenu(tr("&Organizer"));

    actionOrganizerNew = new QAction(QIcon(":/menu-icons/new.svg"), tr("&New"), mainWindow);
    actionOrganizerNew->setStatusTip(tr("Create new Organizer to prioritize your knowledge in Eisenhower Matrix style"));

#ifdef __APPLE__
    actionOrganizerEdit = new QAction(QIcon(":/menu-icons/edit.svg"), tr("&Edit"), mainWindow);
#else
    actionOrganizerEdit = new QAction(QIcon(":/menu-icons/edit.svg"), tr("&Edit"), mainWindow);
#endif
    actionOrganizerEdit ->setStatusTip(tr("Edit current Organizer - you can also double click view to open the editor"));

    actionOrganizerClone = new QAction(QIcon(":/menu-icons/copy.svg"), tr("C&lone"), mainWindow);
    actionOrganizerClone->setStatusTip(tr("Make copy of the current Organizer"));

    actionOrganizerForget = new QAction(QIcon(":/menu-icons/delete.svg"), tr("&Delete"), mainWindow);
    actionOrganizerForget->setStatusTip(tr("Delete Organizer without undo"));

    actionOrganizerMovePrevious = new QAction(QIcon(":/menu-icons/left.svg"), tr("Move Notebook/Note to &Previous Column/Quadrant\tCtrl+Left"), mainWindow); // handled from Outline tree
    actionOrganizerMovePrevious->setStatusTip(tr("Move Notebook/Note to previous column or quadrant..."));

    actionOrganizerMoveNext = new QAction(QIcon(":/menu-icons/right.svg"), tr("Move Notebook/Note to Ne&xt Column/Quadrant\tCtrl+Right"), mainWindow);
    actionOrganizerMoveNext->setStatusTip(tr("Move Notebook/Note to next column or quadrant..."));

    actionOrganizerFocusPrevious = new QAction(QIcon(":/menu-icons/left.svg"), tr("Focus to Previous Column/Quadrant\tShift+Tab"), mainWindow); // handled from Outline tree
    actionOrganizerFocusPrevious->setStatusTip(tr("Move focus to previous column or quandrant..."));

    actionOrganizerFocusNext = new QAction(QIcon(":/menu-icons/right.svg"), tr("Focus to Next Column/Quadrant\tTab"), mainWindow);
    actionOrganizerFocusNext->setStatusTip(tr("Move focus to next column or quandrant..."));

    menuOrganizer->addAction(actionOrganizerNew);
    menuOrganizer->addAction(actionOrganizerEdit);
    menuOrganizer->addAction(actionOrganizerClone);
    menuOrganizer->addAction(actionOrganizerForget);
    menuOrganizer->addSeparator();
    menuOrganizer->addAction(actionOrganizerFocusPrevious);
    menuOrganizer->addAction(actionOrganizerFocusNext);
    menuOrganizer->addSeparator();
    menuOrganizer->addAction(actionOrganizerMovePrevious);
    menuOrganizer->addAction(actionOrganizerMoveNext);

    // menu: Outline

    menuOutline = qMenuBar->addMenu(tr("Note&book"));

    actionOutlineNew = new QAction(QIcon(":/menu-icons/new.svg"), tr("&New"), mainWindow);
    actionOutlineNew->setStatusTip(tr("Create new Notebook to form new ideas, principles, combinations or applications"));

#ifdef __APPLE__
    actionOutlineEdit = new QAction(QIcon(":/menu-icons/edit.svg"), tr("&Edit"), mainWindow);
    actionOutlineEdit->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
#else
    actionOutlineEdit = new QAction(QIcon(":/menu-icons/edit.svg"), tr("&Edit"), mainWindow);
#endif
    actionOutlineEdit ->setStatusTip(tr("Edit current Notebook - you can also double click view to open the editor"));

    actionOutlineHome = new QAction(QIcon(":/menu-icons/home.svg"), tr("Make &Home"), mainWindow);
    actionOutlineHome->setStatusTip(tr("Toggle tag indicating whether to use the current Notebook as home"));

    actionOutlineStencil = new QAction(QIcon(":/menu-icons/stencil.svg"), tr("Make &Stencil"), mainWindow);
    actionOutlineStencil->setStatusTip(tr("Copy the current Notebook as to Stencil"));
    actionOutlineStencil->setEnabled(false);

    actionOutlineClone = new QAction(QIcon(":/menu-icons/copy.svg"), tr("C&lone"), mainWindow);
    actionOutlineClone->setStatusTip(tr("Make copy of the current Notebook"));

    actionOutlineArtExamine= new QAction(QIcon(":/menu-icons/on.svg"), tr("E&xamine"), mainWindow);
    actionOutlineArtExamine->setStatusTip(tr("Turn Notebook to deck of flashcard and start active recall testing..."));

    actionOutlineForget = new QAction(QIcon(":/menu-icons/delete.svg"), tr("&Forget"), mainWindow);
    actionOutlineForget->setStatusTip(tr("Forget Notebook and move it to Limbo"));

    submenuOutlineExport = menuOutline->addMenu(QIcon(":/menu-icons/export.svg"), "E&xport");
    actionOutlineHtmlExport = new QAction(tr("&HTML"), mainWindow);
    actionOutlineHtmlExport->setStatusTip(tr("Export Notebook to a file in HTML format"));
    // TODO Markdown
    // TODO Markdown with meta (status: Markdown with MindForger metadata
    submenuOutlineExport->addAction(actionOutlineHtmlExport);

    submenuOutlineImport = menuOutline->addMenu(QIcon(":/menu-icons/import.svg"), tr("&Import"));
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
    menuOutline->addSeparator();
#ifdef MF_WIP
    menuOutline->addAction(actionOutlineArtExamine);
#endif
    menuOutline->addSeparator();
    menuOutline->addMenu(submenuOutlineImport);
    menuOutline->addMenu(submenuOutlineExport);

    // menu: Note
#ifdef __APPLE__
    actionNoteNew = new QAction(QIcon(":/menu-icons/new.svg"), tr("&New"), mainWindow);
    actionNoteNew->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
#else
    actionNoteNew = new QAction(QIcon(":/menu-icons/new.svg"), tr("&New"), mainWindow);
#endif
    actionNoteNew->setStatusTip(tr("Create new Note to form new ideas, principles, combinations and applications"));

#ifdef __APPLE__
    actionNoteEdit = new QAction(QIcon(":/menu-icons/edit.svg"), tr("&Edit"), mainWindow);
    actionNoteEdit->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
#else
    actionNoteEdit = new QAction(QIcon(":/menu-icons/edit.svg"), tr("&Edit\tCtrl+E"), mainWindow);
#endif
    actionNoteEdit ->setStatusTip(tr("Edit current Note - you can also double click view to open the editor"));

    actionNoteExternalEdit = new QAction(QIcon(":/menu-icons/edit.svg"), tr("E&xternal Editor Edit\tCtrl+X"), mainWindow);
    actionNoteExternalEdit ->setStatusTip(tr("Edit current Note in an external editor - use Preferences to configure the editor"));

    actionNoteSave = new QAction(QIcon(":/menu-icons/save.svg"), tr("Remember\tCtrl+S"), mainWindow); // Ctrl+S is handled elsewhere and I don't want menu to handle it
    actionNoteSave->setStatusTip(tr("Save Note being edited"));

#ifdef __APPLE__
    actionNoteForget = new QAction(QIcon(":/menu-icons/delete.svg"), tr("&Forget\tCtrl+D"), mainWindow);
#else
    actionNoteForget = new QAction(QIcon(":/menu-icons/delete.svg"), tr("&Forget\tDel"), mainWindow); // Delete is handled elsewhere and I don't want menu to handle it
#endif
    actionNoteForget->setStatusTip(tr("Forget Note"));

#ifdef __APPLE__
    actionNoteClose = new QAction(QIcon(":/menu-icons/save.svg"), tr("Save and Leave\tCtrl+L"), mainWindow);
#else
    actionNoteClose = new QAction(QIcon(":/menu-icons/save.svg"), tr("Leave\tAlt+Left"), mainWindow); // Alt+Left is handled elsewhere and I don't want menu to handle it
#endif
    actionNoteClose->setStatusTip(tr("Save leave editor of Note being changed"));

    actionNotePromote = new QAction(QIcon(":/menu-icons/left.svg"), tr("&Promote\tCtrl+Left"), mainWindow); // handled from Outline tree
    actionNotePromote->setStatusTip(tr("Promote Note"));

    actionNoteDemote = new QAction(QIcon(":/menu-icons/right.svg"), tr("&Demote\tCtrl+Right"), mainWindow);
    actionNoteDemote->setStatusTip(tr("Demote Note"));

    actionNoteFirst = new QAction(QIcon(":/menu-icons/top.svg"), tr("F&irst\tCtrl+Shift+Up"), mainWindow);
    actionNoteFirst->setStatusTip(tr("Move Note to be the first child of its parent"));

    actionNoteUp = new QAction(QIcon(":/menu-icons/up.svg"), tr("&Up\tCtrl+Up"), mainWindow);
    actionNoteUp->setStatusTip(tr("Move Note up"));

    actionNoteDown = new QAction(QIcon(":/menu-icons/down.svg"), tr("Do&wn\tCtrl+Down"), mainWindow);
    actionNoteDown->setStatusTip(tr("Move Note down"));

    actionNoteLast = new QAction(QIcon(":/menu-icons/bottom.svg"), tr("&Last\tCtrl+Shift+Down"), mainWindow);
    actionNoteLast->setStatusTip(tr("Move Note to be the last child of its parent"));

    actionNoteRefactor = new QAction(QIcon(":/menu-icons/refactor.svg"), tr("&Refactor"), mainWindow);
    actionNoteRefactor->setStatusTip(tr("Refactor Note to another Notebook..."));

    actionNoteStencil = new QAction(QIcon(":/menu-icons/stencil.svg"), tr("Make &Stencil"), mainWindow);
    actionNoteStencil->setStatusTip(tr("Copy the current Notebook as to Stencil"));
    actionNoteStencil->setEnabled(false);

    actionNoteClone = new QAction(QIcon(":/menu-icons/copy.svg"), tr("&Clone"), mainWindow);
    actionNoteClone->setStatusTip(tr("Make a copy of the Note to this or other Notebook..."));

    actionNoteExport = new QAction(QIcon(":/menu-icons/export.svg"), tr("E&xport"), mainWindow);
    actionNoteExport->setStatusTip(tr("Export Note to an external file in a supported format"));
    actionNoteExport->setEnabled(false);

    actionNoteImport = new QAction(QIcon(":/menu-icons/import.svg"), tr("Import"), mainWindow);
    actionNoteImport->setStatusTip(tr("Import Note from an external file in a supported format"));
    actionNoteImport->setEnabled(false);

    menuNote = qMenuBar->addMenu(tr("&Note"));
    menuNote->addAction(actionNoteNew);
    menuNote->addAction(actionNoteEdit);
    menuNote->addAction(actionNoteExternalEdit);
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

    actionEditFind = new QAction(QIcon(":/menu-icons/find.svg"), tr("&Find\tCtrl+Shift+F"), mainWindow);
    actionEditFind->setStatusTip(tr("Search Note text"));

    actionEditFindNext = new QAction(QIcon(":/menu-icons/find.svg"), tr("Find Next\tCtrl+F"), mainWindow);
    actionEditFindNext->setStatusTip(tr("Search Note text again"));

    actionEditUndo = new QAction(QIcon(":/menu-icons/undo.svg"), tr("&Undo\tCtrl+Z"), mainWindow);
    actionEditUndo->setStatusTip(tr("Undo"));

    actionEditRedo = new QAction(QIcon(":/menu-icons/redo.svg"), tr("&Redo\tCtrl+Shift+Z"), mainWindow);
    actionEditRedo->setStatusTip(tr("Redo"));

    actionEditCut = new QAction(QIcon(":/menu-icons/cut.svg"), tr("Cu&t\tCtrl+X"), mainWindow);
    actionEditCut->setStatusTip(tr("Cut"));

    actionEditCopy = new QAction(QIcon(":/menu-icons/copy.svg"), tr("&Copy\tCtrl+C"), mainWindow);
    actionEditCopy->setStatusTip(tr("Copy"));

    actionEditPaste = new QAction(QIcon(":/menu-icons/paste.svg"), tr("&Paste\tCtrl+V"), mainWindow);
    actionEditPaste->setStatusTip(tr("Paste"));

    actionEditLiveNotePreview = new QAction(QIcon(":/menu-icons/preview.svg"), tr("&Live Preview"), mainWindow);
    actionEditLiveNotePreview->setCheckable(true);
    actionEditLiveNotePreview->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_P));
    actionEditLiveNotePreview->setStatusTip(tr("Toggle live HTML preview"));

    actionEditWordWrap = new QAction(QIcon(":/menu-icons/word-wrap.svg"), tr("&Word Wrap"), mainWindow);
    actionEditWordWrap->setStatusTip(tr("Toggle word wrap mode"));

    actionEditNameDescFocusSwap = new QAction(QIcon(":/menu-icons/up.svg"), tr("&Swap Name/Description Focus"), mainWindow);
    actionEditNameDescFocusSwap->setStatusTip(tr("Swap focus of N title and description editors"));

    actionEditExtract = new QAction(QIcon(":/menu-icons/cut.svg"), tr("E&xtract"), mainWindow);
    actionEditExtract->setStatusTip(tr("Create new Note from the text selected in the current Note..."));

    actionEditComplete = new QAction(QIcon(":/menu-icons/on.svg"), tr("Complete Link\tCtrl+/"), mainWindow);
    actionEditComplete->setStatusTip(tr("Complete word being written by finding link to Notebook or Note"));

    menuEdit = qMenuBar->addMenu(tr("&Edit"));
    menuEdit->addAction(actionEditFind);
    menuEdit->addAction(actionEditFindNext);
    menuEdit->addSeparator();
    menuEdit->addAction(actionEditUndo);
    menuEdit->addAction(actionEditRedo);
    menuEdit->addSeparator();
    menuEdit->addAction(actionEditCut);
    menuEdit->addAction(actionEditCopy);
    menuEdit->addAction(actionEditPaste);
    menuEdit->addSeparator();
    menuEdit->addAction(actionEditLiveNotePreview);
    menuEdit->addAction(actionEditWordWrap);
    menuEdit->addAction(actionEditNameDescFocusSwap);
    menuEdit->addSeparator();
    menuEdit->addAction(actionEditComplete);
    menuEdit->addAction(actionEditExtract);
    menuEdit->setEnabled(false);

    // menu: format
    menuFormat = qMenuBar->addMenu(tr("&Format"));

    actionFormatBold = new QAction(tr("&Bold"), mainWindow);
    actionFormatBold->setStatusTip(tr("Format text as bold"));

    actionFormatItalic = new QAction(tr("&Italic"), mainWindow);
    actionFormatItalic->setStatusTip(tr("Format text as italic"));

    actionFormatCode = new QAction(tr("&Code"), mainWindow);
    actionFormatCode->setStatusTip(tr("Format text as inlined source code"));

    actionFormatMath = new QAction(tr("&Math"), mainWindow);
    actionFormatMath->setStatusTip(tr("Format text as math (MathJax)"));

    // lists
    submenuFormatLists = menuFormat->addMenu(tr("Lis&ts"));

    actionFormatListBullet = new QAction(tr("&Bulleted List"), mainWindow);
    actionFormatListBullet->setStatusTip(tr("Format block as bulleted list"));
    submenuFormatLists->addAction(actionFormatListBullet);

    actionFormatListNumber = new QAction(tr("&Numbered List"), mainWindow);
    actionFormatListNumber->setStatusTip(tr("Format block as numbered list"));
    submenuFormatLists->addAction(actionFormatListNumber);

    actionFormatListTask = new QAction(tr("&Task List"), mainWindow);
    actionFormatListTask->setStatusTip(tr("Format block as task list"));
    submenuFormatLists->addAction(actionFormatListTask);

    // TODO handling
    actionFormatListTaskItem = new QAction(tr("Task List &Item"), mainWindow);
    actionFormatListTaskItem->setStatusTip(tr("Format block as task list"));
    submenuFormatLists->addAction(actionFormatListTaskItem);

    // blocks
    submenuFormatBlocks = menuFormat->addMenu(tr("Bl&ocks"));

    actionFormatBlockQuote = new QAction(tr("Block &Quote"), mainWindow);
    actionFormatBlockQuote->setStatusTip(tr("Format text block as blockquote"));
    submenuFormatBlocks->addAction(actionFormatBlockQuote);

    actionFormatCodeBlock = new QAction(tr("&Code Block"), mainWindow);
    actionFormatCodeBlock->setStatusTip(tr("Format text block as source code"));
    submenuFormatBlocks->addAction(actionFormatCodeBlock);

    actionFormatMathBlock = new QAction(tr("&Math Block"), mainWindow);
    actionFormatMathBlock->setStatusTip(tr("Format text block as math (MathJax)"));
    submenuFormatBlocks->addAction(actionFormatMathBlock);

    // TODO handling
    actionFormatDiagramBlock = new QAction(tr("&Diagram Block"), mainWindow);
    actionFormatDiagramBlock->setStatusTip(tr("Format code block as diagram (Mermaid)"));
    submenuFormatBlocks->addAction(actionFormatDiagramBlock);

    // diagrams
    submenuFormatDiagrams = menuFormat->addMenu(tr("Diagrams")); // &

    // TODO handling v v v

    actionFormatDiagramsFlow = new QAction(tr("&Flowchart"), mainWindow);
    actionFormatDiagramsFlow->setStatusTip(tr("Insert flowchart Mermaid diagram skeleton"));
    submenuFormatDiagrams->addAction(actionFormatDiagramsFlow);

    actionFormatDiagramsSequence = new QAction(tr("&Sequence Diagram"), mainWindow);
    actionFormatDiagramsSequence->setStatusTip(tr("Insert sequence Mermaid diagram skeleton"));
    submenuFormatDiagrams->addAction(actionFormatDiagramsSequence);

    actionFormatDiagramsClass = new QAction(tr("&Class Diagram"), mainWindow);
    actionFormatDiagramsClass->setStatusTip(tr("Insert class Mermaid diagram skeleton"));
    submenuFormatDiagrams->addAction(actionFormatDiagramsClass);

    actionFormatDiagramsState = new QAction(tr("St&ate Diagram"), mainWindow);
    actionFormatDiagramsState->setStatusTip(tr("Insert state Mermaid diagram skeleton"));
    submenuFormatDiagrams->addAction(actionFormatDiagramsState);

    actionFormatDiagramsGantt = new QAction(tr("&Gantt Diagram"), mainWindow);
    actionFormatDiagramsGantt->setStatusTip(tr("Insert Gantt Mermaid diagram skeleton"));
    submenuFormatDiagrams->addAction(actionFormatDiagramsGantt);

    actionFormatDiagramsPie = new QAction(tr("&Pie Diagram"), mainWindow);
    actionFormatDiagramsPie->setStatusTip(tr("Insert pie Mermaid chart skeleton"));
    submenuFormatDiagrams->addAction(actionFormatDiagramsPie);

    // MathJax
    submenuFormatMathJax = menuFormat->addMenu(tr("MathJa&x"));
    actionFormatMathText = new QAction(tr("&text"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathText);
    submenuFormatMathJax->addSeparator();
    actionFormatMathFraction= new QAction(tr("&fraction"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathFraction);
    actionFormatMathSum = new QAction(tr("&sum"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathSum);
    actionFormatMathSqrt = new QAction(tr("s&quare root"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathSqrt);
    submenuFormatMathJax->addSeparator();
    actionFormatMathInt = new QAction(tr("&integral"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathInt);
    actionFormatMathIiint = new QAction(tr("integrals"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathIiint);
    submenuFormatMathJax->addSeparator();
    actionFormatMathAlpha = new QAction(tr("&alpha"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathAlpha);
    actionFormatMathBeta = new QAction(tr("&beta"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathBeta);
    actionFormatMathGama = new QAction(tr("&Gama"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathGama);
    actionFormatMathDelta = new QAction(tr("&Delta"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathDelta);
    submenuFormatMathJax->addSeparator();
    actionFormatMathBar = new QAction(tr("&bar"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathBar);
    actionFormatMathHat = new QAction(tr("&hat"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathHat);
    actionFormatMathDot = new QAction(tr("&dot"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathDot);
    actionFormatMathOverrightarrow = new QAction(tr("&overrightarrow"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathOverrightarrow);
    submenuFormatMathJax->addSeparator();
    actionFormatMathCup = new QAction(tr("&cup"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathCup);
    actionFormatMathCap = new QAction(tr("&cap"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathCap);
    actionFormatMathEmptyset = new QAction(tr("&empty set"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathEmptyset);
    actionFormatMathIn = new QAction(tr("&in"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathIn);
    actionFormatMathNotin = new QAction(tr("&not in"), mainWindow);
    submenuFormatMathJax->addAction(actionFormatMathNotin);

    actionFormatStrikethrough = new QAction(tr("&Strikethrough"), mainWindow);
    actionFormatStrikethrough->setStatusTip(tr("Format text as strikethrough"));

    actionFormatKeyboard = new QAction(tr("&Keyboard"), mainWindow);
    actionFormatKeyboard->setStatusTip(tr("Format text as keyboard input"));

    actionFormatToc = new QAction(tr("T&able of Contents"), mainWindow);
    actionFormatToc ->setStatusTip(tr("Insert Notebook's table of contents"));

    actionFormatTimestamp = new QAction(tr("Timestam&p"), mainWindow);
    actionFormatTimestamp ->setStatusTip(tr("Insert current date and time"));

    actionFormatLink = new QAction(tr("&Link"), mainWindow);
    actionFormatLink->setStatusTip(tr("Insert link to a document, image or file"));

    actionFormatImage = new QAction(tr("Ima&ge"), mainWindow);
    actionFormatImage->setStatusTip(tr("Insert image"));

    actionFormatTable = new QAction(tr("Tabl&es"), mainWindow);
    actionFormatTable->setStatusTip(tr("Insert table..."));

    actionFormatHr = new QAction(tr("&Horizontal ruler"), mainWindow);
    actionFormatHr->setStatusTip(tr("Horizontal ruler"));

    menuFormat->addAction(actionFormatBold);
    menuFormat->addAction(actionFormatItalic);
    menuFormat->addAction(actionFormatCode);
    menuFormat->addAction(actionFormatMath);
    menuFormat->addAction(actionFormatStrikethrough);
    menuFormat->addAction(actionFormatKeyboard);
    menuFormat->addSeparator();
    menuFormat->addSeparator();
    menuFormat->addAction(actionFormatToc);
    menuFormat->addMenu(submenuFormatLists);
    menuFormat->addMenu(submenuFormatBlocks);
    menuFormat->addMenu(submenuFormatDiagrams);
    menuFormat->addAction(actionFormatTable);
    menuFormat->addMenu(submenuFormatMathJax);
    menuFormat->addAction(actionFormatTimestamp);
    menuFormat->addAction(actionFormatHr);
    menuFormat->addSeparator();
    menuFormat->addAction(actionFormatLink);
    menuFormat->addAction(actionFormatImage);
    menuFormat->setEnabled(false);

    // menu: help

    actionHelpDocumentation = new QAction(QIcon(":/menu-icons/help.svg"), tr("&Documentation"), mainWindow);
    actionHelpDocumentation->setShortcut(tr("F1"));
    actionHelpDocumentation->setStatusTip(tr("Open MindForger documentation"));

    actionHelpWeb = new QAction(QIcon(":/menu-icons/link.svg"), tr("&Web"), mainWindow);
    actionHelpWeb->setStatusTip(tr("Open MindForger web"));

    actionHelpMarkdown = new QAction(QIcon(":/menu-icons/document.svg"), tr("&Markdown tutorial"), mainWindow);
    actionHelpMarkdown->setStatusTip(tr("Open Markdown tutorial"));

    actionHelpMathQuickReference= new QAction(QIcon(":/menu-icons/document.svg"), tr("Math cheatsheet"), mainWindow);
    actionHelpMathQuickReference->setStatusTip(tr("Open MathJax quick reference"));

    actionHelpMathLivePreview = new QAction(QIcon(":/menu-icons/document.svg"), tr("Math live preview"), mainWindow);
    actionHelpMathLivePreview->setStatusTip(tr("Open MathJax live demo"));

    actionHelpDiagrams = new QAction(QIcon(":/menu-icons/document.svg"), tr("Mermaid dia&grams documentation"), mainWindow);
    actionHelpDiagrams->setStatusTip(tr("Open Mermaid diagrams documentation"));

    actionHelpReportBug = new QAction(QIcon(":/menu-icons/bug.svg"), tr("Report &Bug or Request Feature"), mainWindow);
    actionHelpReportBug->setStatusTip(tr("Report bug or suggest an enhancement"));

    actionHelpCheckForUpdates = new QAction(QIcon(":/menu-icons/download.svg"), tr("&Check for Updates"), mainWindow);
    actionHelpCheckForUpdates->setStatusTip(tr("Check for MindForger updates"));

    actionHelpAbout = new QAction(QIcon(":/menu-icons/write.svg"), tr("&About MindForger"), mainWindow);
    actionHelpAbout->setStatusTip(tr("About MindForger..."));

    menuHelp = qMenuBar->addMenu(tr("&Help"));
    menuHelp->addAction(actionHelpDocumentation);
    menuHelp->addAction(actionHelpWeb);
    menuHelp->addSeparator();
    menuHelp->addAction(actionHelpReportBug);
    menuHelp->addAction(actionHelpCheckForUpdates);
    menuHelp->addSeparator();
    menuHelp->addAction(actionHelpMarkdown);
    menuHelp->addAction(actionHelpMathQuickReference);
    menuHelp->addAction(actionHelpMathLivePreview);
    menuHelp->addAction(actionHelpDiagrams);
    menuHelp->addSeparator();
    menuHelp->addAction(actionHelpAbout);
}

MainMenuView::~MainMenuView()
{
    // TODO deletes (menus DO not delete > menuBar creates them)
}

/**
 * @brief Show all menu items.
 *
 * Strategy is to always enable ALL menu items and accelerators, THEN
 * disable what's not desired in given facet.
 */
void MainMenuView::showAllMenuItems()
{
    menuMind->setEnabled(true);
    // autolink: leave as is - it's not that simple as it's status, not just action
    actionExit->setEnabled(true);

    menuFind->setEnabled(true);
    actionFindFts->setEnabled(true);
    actionFindOutlineByName->setEnabled(true);
    actionFindOutlineByTag->setEnabled(true);
    actionFindNoteByName->setEnabled(true);
    actionFindNoteByTag->setEnabled(true);

    menuView->setEnabled(true);
    actionViewDashboard->setEnabled(true);
    actionViewHome->setEnabled(true);
    actionViewOrganizers->setEnabled(true);
    actionViewOutlines->setEnabled(true);
    actionViewTags->setEnabled(true);
    actionViewNavigator->setEnabled(true);
    actionViewLimbo->setEnabled(true);
    actionViewRecentNotes->setEnabled(true);

#ifdef MF_WIP
    menuLibrary->setEnabled(true);

    actionLibraryAdd->setEnabled(true);
    // TODO to be implemented
    actionLibraryDeprecate->setEnabled(false);
#endif

#ifdef MF_WIP
    menuFlashcards->setEnabled(true);
#endif

    menuOrganizer->setEnabled(true);
    actionOrganizerNew->setEnabled(true);
    actionOrganizerEdit->setEnabled(true);
    actionOrganizerClone->setEnabled(true);
    actionOrganizerForget->setEnabled(true);
    actionOrganizerFocusPrevious->setEnabled(true);
    actionOrganizerFocusNext->setEnabled(true);
    actionOrganizerMovePrevious->setEnabled(true);
    actionOrganizerMoveNext->setEnabled(true);

    menuNavigator->setEnabled(true);
    actionViewOrganizers->setEnabled(true);
    actionOutlineEdit->setEnabled(true);
    actionOutlineClone->setEnabled(true);
    actionOutlineHome->setEnabled(true);
    actionOutlineForget->setEnabled(true);

    menuOutline->setEnabled(true);
    submenuOutlineExport->setEnabled(true);

    menuNote->setEnabled(true);
    actionNoteForget->setEnabled(true);
    actionNotePromote->setEnabled(true);
    actionNoteDemote->setEnabled(true);
    actionNoteFirst->setEnabled(true);
    actionNoteUp->setEnabled(true);
    actionNoteDown->setEnabled(true);
    actionNoteLast->setEnabled(true);

    menuEdit->setEnabled(true);

    menuFormat->setEnabled(true);

    menuHelp->setEnabled(true);

    mainWindow->getToolBar()->setEnabled(true);
}

void MainMenuView::showFacetOrganizerList(bool repositoryMode)
{
    showAllMenuItems();

    actionOrganizerEdit->setEnabled(false);
    actionOrganizerClone->setEnabled(false);
    actionOrganizerForget->setEnabled(false);
    actionOrganizerFocusPrevious->setEnabled(false);
    actionOrganizerFocusNext->setEnabled(false);
    actionOrganizerMovePrevious->setEnabled(false);
    actionOrganizerMoveNext->setEnabled(false);

#ifdef MF_WIP_KNOW
    menuKnowledge->setEnabled(false);
#endif
    menuNavigator->setEnabled(false);
#ifdef MF_WIP
    menuLibrary->setEnabled(false);
#endif
    menuOutline->setEnabled(false);
    menuNote->setEnabled(false);
    menuEdit->setEnabled(false);
    menuFormat->setEnabled(false);
    submenuOutlineExport->setEnabled(false);

    if(!repositoryMode) {
        menuView->setEnabled(false);
        menuFormat->setEnabled(false);

        actionFindOutlineByName->setEnabled(false);
        actionFindOutlineByTag->setEnabled(false);
    }
}

void MainMenuView::showFacetOrganizerView(bool repositoryMode)
{
    showAllMenuItems();

#ifdef MF_WIP_KNOW
    menuKnowledge->setEnabled(false);
#endif
    menuNavigator->setEnabled(false);
#ifdef MF_WIP
    menuLibrary->setEnabled(false);
#endif
    menuOutline->setEnabled(false);
    menuNote->setEnabled(false);
    menuEdit->setEnabled(false);
    menuFormat->setEnabled(false);
    submenuOutlineExport->setEnabled(false);

    if(!repositoryMode) {
        menuView->setEnabled(false);
        menuFormat->setEnabled(false);

        actionFindOutlineByName->setEnabled(false);
        actionFindOutlineByTag->setEnabled(false);
    }
}

void MainMenuView::showFacetOutlineList(bool repositoryMode)
{
    showAllMenuItems();

    actionOutlineEdit->setEnabled(false);
    actionOutlineClone->setEnabled(false);
    actionOutlineHome->setEnabled(false);
    actionOutlineForget->setEnabled(false);

    menuNavigator->setEnabled(false);
    menuOrganizer->setEnabled(false);
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
#ifdef MF_WIP
    menuLibrary->setEnabled(false);
#endif
    menuOrganizer->setEnabled(false);
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

    menuMind->setEnabled(false);
    actionExit->setEnabled(false);

#ifdef MF_WIP
    menuLibrary->setEnabled(false);
#endif
    menuOrganizer->setEnabled(false);
    actionOrganizerNew->setEnabled(false);
    actionOrganizerEdit->setEnabled(false);
    actionOrganizerClone->setEnabled(false);
    actionOrganizerForget->setEnabled(false);
    actionOrganizerFocusPrevious->setEnabled(false);
    actionOrganizerFocusNext->setEnabled(false);
    actionOrganizerMovePrevious->setEnabled(false);
    actionOrganizerMoveNext->setEnabled(false);

    menuFind->setEnabled(false);
    actionFindOutlineByName->setEnabled(false);
    actionFindOutlineByTag->setEnabled(false);
    actionFindNoteByName->setEnabled(false);
    actionFindNoteByTag->setEnabled(false);

    menuView->setEnabled(false);
    actionViewDashboard->setEnabled(false);
    actionViewHome->setEnabled(false);
    actionViewOrganizers->setEnabled(false);
    actionViewOutlines->setEnabled(false);
    actionViewTags->setEnabled(false);
    actionViewNavigator->setEnabled(false);
    actionViewLimbo->setEnabled(false);
    actionViewRecentNotes->setEnabled(false);

    menuNavigator->setEnabled(false);
    actionViewOrganizers->setEnabled(false);
    actionOutlineEdit->setEnabled(false);
    actionOutlineClone->setEnabled(false);
    actionOutlineHome->setEnabled(false);
    actionOutlineForget->setEnabled(false);

    menuOutline->setEnabled(false);
    submenuOutlineExport->setEnabled(false);

    menuNote->setEnabled(false);
    actionNoteForget->setEnabled(false);
    actionNotePromote->setEnabled(false);
    actionNoteDemote->setEnabled(false);
    actionNoteFirst->setEnabled(false);
    actionNoteUp->setEnabled(false);
    actionNoteDown->setEnabled(false);
    actionNoteLast->setEnabled(false);

    mainWindow->getToolBar()->setEnabled(false);

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
    actionMindThink->setChecked(true);
}

void MainMenuView::showFacetMindSleep()
{
    actionMindThink->setChecked(false);
}

void MainMenuView::showFacetMindAutolink(bool enabled)
{
    actionMindAutolink->setChecked(enabled);
}

void MainMenuView::showFacetLiveNotePreview(bool enabled)
{
    actionEditLiveNotePreview->setChecked(enabled);
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
