/*
 main_menu_view.cpp     MindForger thinking notebook

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

    // new/devise... new Workspaces (MD repository)
    submenuMindNew = menuMind->addMenu(QIcon(":/menu-icons/new.svg"), "&New");
    actionMindNewRepository = new QAction(tr("&Workspace"), mainWindow);
    actionMindNewRepository->setStatusTip(
        tr("Create a brand new MindForger workspace...")
    );
    submenuMindNew->addAction(actionMindNewRepository);
    actionMindNewFile = new QAction(tr("Markdown &File"), mainWindow);
    actionMindNewFile->setStatusTip(tr("Create a brand new Markdown file..."));
    submenuMindNew->addAction(actionMindNewFile);

    // learn... from a workspace, Markdown or TXT file
    submenuMindLearn = menuMind->addMenu(QIcon(":/menu-icons/open.svg"), tr("&Learn"));
    actionMindLearnRepository = new QAction(
        tr("&Workspace"), mainWindow
    );
    actionMindLearnRepository->setStatusTip(
        tr("Learn knowledge by loading a MindForger workspace...")
    );
    submenuMindLearn->addAction(actionMindLearnRepository);
    actionMindLearnDirectory = new QAction(
        tr("&Directory with Markdowns"), mainWindow
    );
    actionMindLearnDirectory->setStatusTip(
        tr("Learn knowledge by loading a directory with Markdown files...")
    );
    submenuMindLearn->addAction(actionMindLearnDirectory);
    actionMindLearnFile = new QAction(tr("Markdown &File"), mainWindow);
    actionMindLearnFile->setStatusTip(
        tr("Learn knowledge by loading a Markdown file...")
    );
    submenuMindLearn->addAction(actionMindLearnFile);

    // re-learn/remind ... recent repositories and files
    submenuMindRelearn = new RecentFilesMenu(tr("&Remind"), mainWindow);
    submenuMindRelearn->setStatusTip(
        tr(
            "Re-learn recently opened MindForger workspaces, "
            "Markdown directories or files")
        );
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
    actionMindAutolink->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_L));
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
    actionMindSnapshot->setStatusTip(
        tr(
            "Create backup archive of the current workspace and "
            "store it in home directory"
        )
    );
    actionMindSnapshot->setEnabled(false);

    // TODO submenu: printer, HTML, PDF

    actionMindPreferences = new QAction(QIcon(":/menu-icons/configure.svg"), tr("A&dapt"), mainWindow);
    actionMindPreferences->setShortcuts(QKeySequence::Preferences);
    actionMindPreferences->setStatusTip(tr("Adapt Mind by setting your preferences..."));

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
    menuMind->addAction(actionExit);
#ifdef DO_MF_DEBUG
    menuMind->addSeparator();
    menuMind->addAction(actionMindHack);
#endif

    // menu: recall

    actionFindFts = new QAction(QIcon(":/menu-icons/find.svg"), tr("&Full-text Search"), mainWindow);
#ifdef __APPLE__
    actionFindFts->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_F));
#else
    actionFindFts->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_F));
#endif
    actionFindFts->setStatusTip(tr("Note full-text search"));

    actionFindOutlineByName = new QAction(
        QIcon(":/menu-icons/find.svg"), tr("Recall Note&book by Name"), mainWindow);
    actionFindOutlineByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_B));
    actionFindOutlineByName->setStatusTip(tr("Find Notebook by name"));

    actionFindNoteByName = new QAction(
        QIcon(":/menu-icons/find.svg"), tr("Recall &Note by Name"), mainWindow);
    actionFindNoteByName->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_N));
    actionFindNoteByName->setStatusTip(tr("Find Note by name"));

    actionFindOutlineByTag = new QAction(
        QIcon(":/menu-icons/find.svg"), tr("Recall Notebook by Ta&gs"), mainWindow);
    actionFindOutlineByTag->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_X));
    actionFindOutlineByTag->setStatusTip(tr("Find Notebook by tags"));

    actionFindNoteByTag = new QAction(
        QIcon(":/menu-icons/find.svg"), tr("Recall Note by T&ags"), mainWindow);
    actionFindNoteByTag->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_A));
    actionFindNoteByTag->setStatusTip(tr("Find Note by tags"));

#ifdef MF_WIP
    actionFindDocByName = new QAction(
        QIcon(":/menu-icons/find.svg"), tr("Recall Library &Doc by Name"), mainWindow);
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
    actionViewDecks->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_D));
    actionViewDecks->setStatusTip(tr("Show list of flashcard decks..."));

    actionViewOrganizers = new QAction(QIcon(":/menu-icons/cross.svg"), tr("Organiz&ers"), mainWindow);
    actionViewOrganizers->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_E));
    actionViewOrganizers->setStatusTip(tr("Open Eisenhower matrix and Kanban organizers..."));

    actionViewOutlines = new QAction(QIcon(":/menu-icons/list-text.svg"), tr("N&otebooks"), mainWindow);
    actionViewOutlines->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_O));
    actionViewOutlines->setStatusTip(tr("Show list of Notebooks..."));

    actionViewOutlinesMap = new QAction(QIcon(":/menu-icons/dashboard.svg"), tr("Note&books Tree"), mainWindow);
    actionViewOutlinesMap->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_T));
    actionViewOutlinesMap->setStatusTip(tr("Show tree of Notebooks..."));

#ifdef MF_WIP
    actionViewLibraryDocs = new QAction(QIcon(":/menu-icons/copy.svg"), tr("&Library Documents"), mainWindow);
    actionViewLibraryDocs->setStatusTip(tr("List Library documents..."));
    //actionViewLibraryDocs->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_<REPLACE>));
#endif

    actionViewTags = new QAction(QIcon(":/menu-icons/tag.svg"), tr("&Tags"), mainWindow);
    actionViewTags->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_G));
    actionViewTags->setStatusTip(tr("Open Tag cloud..."));

    actionViewNavigator = new QAction(QIcon(":/menu-icons/grid.svg"), tr("Knowledge Graph &Navigator"), mainWindow);
    actionViewNavigator->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_K));
    actionViewNavigator->setStatusTip(tr("Open knowledge graph Navigator..."));

    actionViewDwell = new QAction(tr("&Memory Dwell"), mainWindow);
    actionViewDwell->setStatusTip(tr("Open memory dwell..."));
    actionViewDwell->setEnabled(false);

    actionViewCli = new QAction(QIcon(":/menu-icons/cli.svg"), tr("&Wingman"), mainWindow);
    actionViewCli->setShortcut(QKeySequence(Qt::ALT+Qt::Key_X));
    actionViewCli->setStatusTip(tr("Activate command line interface..."));

    actionViewTerminal = new QAction(
        QIcon(":/menu-icons/terminal.svg"), tr("Ter&minal"), mainWindow
    );
    actionViewTerminal->setStatusTip(
        tr("Run simple command line from current MindForger workspace...")
    );

    actionViewRecentNotes = new QAction(
        QIcon(":/menu-icons/open-recent.svg"),
        tr("&Recent Notes"),
        mainWindow
    );
    actionViewRecentNotes->setStatusTip(tr("View recently modified Notes..."));
    actionViewRecentNotes->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_R));

    actionViewStencils = new QAction(
        QIcon(":/menu-icons/stencil.svg"), tr("&Stencils"), mainWindow);
    // Outline/Note marked w/ tag stencil is MOVED among stencils (NOT indexed/searched/...)
    //  + Stencil view allows making a stencil outline again
    //  + Note stencils are notes in a given Outline
    actionViewStencils->setStatusTip(tr("List Notebook and Note stencils..."));
    actionViewStencils->setEnabled(false);

    actionViewLimbo = new QAction(
        QIcon(":/menu-icons/limbo.svg"), tr("Li&mbo"), mainWindow);
    actionViewLimbo->setStatusTip(tr("List forgotten Notebooks and Notes..."));
    actionViewLimbo->setEnabled(true);

    actionViewHoist = new QAction(
        QIcon(":/menu-icons/hoisting.svg"), tr("Ho&isting"), mainWindow);
    actionViewHoist->setCheckable(true);
    actionViewHoist->setChecked(false);
    actionViewHoist->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_I));
    actionViewHoist->setStatusTip(
        tr("Hoist/de-hoist Note to focus on Note being viewed or edited"));

    actionViewDistractionFree = new QAction(
        QIcon(":/menu-icons/off.svg"), tr("D&istraction Free"), mainWindow);
    actionViewDistractionFree->setShortcut(QKeySequence(Qt::Key_F5));
    actionViewDistractionFree->setStatusTip(tr("Toggle distraction free mode"));
    actionViewDistractionFree->setEnabled(false);

    actionViewFullscreen = new QAction(
        QIcon(":/menu-icons/fullscreen.svg"), tr("&Fullscreen"), mainWindow);
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
    menuView->addAction(actionViewOutlinesMap);
    menuView->addAction(actionViewOutlines);
#ifdef MF_WIP
    menuView->addAction(actionViewLibraryDocs);
#endif
    menuView->addAction(actionViewTags);
    menuView->addAction(actionViewNavigator);
    menuView->addAction(actionViewCli);
    menuView->addAction(actionViewTerminal);
#ifdef MF_WIP
    menuView->addAction(actionViewStencils);
    menuView->addAction(actionViewDwell);
#endif
    menuView->addAction(actionViewLimbo);
    menuView->addSeparator();
    menuView->addAction(actionViewRecentNotes);
    menuView->addSeparator();
    menuView->addAction(actionViewHoist);
#ifdef MF_WIP
    menuView->addAction(actionViewDistractionFree);
#endif
    menuView->addAction(actionViewFullscreen);

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

    menuNavigator = qMenuBar->addMenu(tr("N&avigate"));
    menuNavigator->addAction(actionNavigatorEdgesStretch);
    menuNavigator->addAction(actionNavigatorEdgesShrink);
    menuNavigator->addSeparator();
    menuNavigator->addAction(actionNavigatorZoomIn);
    menuNavigator->addAction(actionNavigatorZoomOut);
    menuNavigator->addSeparator();
    menuNavigator->addAction(actionNavigatorShuffle);
    menuNavigator->setEnabled(false);

    // menu: library
    menuLibrary = qMenuBar->addMenu(tr("Lib&rary"));

    actionLibraryAdd = new QAction(
        QIcon(":/menu-icons/new.svg"), tr("&New library"), mainWindow);
    actionLibraryAdd->setStatusTip(
        tr("Add path to the directory with documents (PDF, txt, HTML)..."));

    // choose library > determine library src directory > re-index src directory
    // show side-by-side comparison: ONLY in src / ACTION <.del> / ONLY in MF
    // - includes synchronization in one on another direction
    // - decisions executed AFTER user clicks DO IT button (not while editing dialog)
    actionLibrarySync= new QAction(
        QIcon(":/menu-icons/edit.svg"),
        tr("&Update library"),
        mainWindow);
    actionLibrarySync->setStatusTip(
        tr(
            "Synchronize library source directory with MindForger notebook(s) which represent"
            "library resources..."));

    actionLibraryDeprecate = new QAction(
        QIcon(":/menu-icons/delete.svg"), tr("&Delete library"), mainWindow);
    actionLibraryDeprecate->setStatusTip(tr(
        "Delete all Notebooks representing the library resources..."));

    menuLibrary->addAction(actionLibraryAdd);
    menuLibrary->addAction(actionLibrarySync);
    menuLibrary->addAction(actionLibraryDeprecate);

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
    actionOrganizerEdit = new QAction(
        QIcon(":/menu-icons/edit.svg"),
        // stupid & ugly shortcut w/o effect
        tr("&Edit                                                                                     ⌘↩"),
        mainWindow
    );
#else
    actionOrganizerEdit = new QAction(QIcon(":/menu-icons/edit.svg"), tr("&Edit       Alt-Enter"), mainWindow);
#endif
    actionOrganizerEdit ->setStatusTip(tr("Edit current Organizer - you can also double click view to open the editor"));

    actionOrganizerClone = new QAction(QIcon(":/menu-icons/copy.svg"), tr("C&lone"), mainWindow);
    actionOrganizerClone->setStatusTip(tr("Make copy of the current Organizer"));

    actionOrganizerForget = new QAction(QIcon(":/menu-icons/delete.svg"), tr("&Delete"), mainWindow);
    actionOrganizerForget->setStatusTip(tr("Delete Organizer without undo"));

    actionOrganizerMovePrevious = new QAction(
        QIcon(":/menu-icons/left.svg"),
#ifdef __APPLE__
        // IMPROVE: ugly & stupid shortcut spacing - make it signal w/ quadrant with focus detection
        tr("Move Notebook/Note to Previous Column/Quadrant       ⌘["),
#else
        tr("Move Notebook/Note to &Previous Column/Quadrant\tCtrl+Left"),
#endif
        mainWindow
    );
    actionOrganizerMovePrevious->setStatusTip(tr("Move Notebook/Note to previous column or quadrant..."));

    actionOrganizerMoveNext = new QAction(
        QIcon(":/menu-icons/right.svg"),
#ifdef __APPLE__
         // IMPROVE: ugly & stupid shortcut spacing - make it signal w/ quadrant with focus detection
        tr("Move Notebook/Note to Next Column/Quadrant              ⌘]"),
#else
        tr("Move Notebook/Note to Ne&xt Column/Quadrant\tCtrl+Right"),
#endif
        mainWindow
    );
    actionOrganizerMoveNext->setStatusTip(tr("Move Notebook/Note to next column or quadrant..."));

    actionOrganizerFocusPrevious = new QAction(QIcon(
       ":/menu-icons/left.svg"),
#ifdef __APPLE__
       // IMPROVE: ugly & stupid shortcut spacing - make it signal w/ quadrant with focus detection
       tr("Focus to Previous Column/Quadrant                              ⇧⇥"),
#else
        tr("Move Notebook/Note to &Previous Column/Quadrant\tCtrl+Left"),
#endif
       mainWindow
   );
    actionOrganizerFocusPrevious->setStatusTip(tr("Move focus to previous column or quandrant..."));

    actionOrganizerFocusNext = new QAction(
        QIcon(":/menu-icons/right.svg"),
#ifdef __APPLE__
        // IMPROVE: ugly & stupid shortcut spacing - make it signal w/ quadrant with focus detection
        tr("Focus to Next Column/Quadrant                                        ⇥"),
#else
        tr("Move Notebook/Note to Ne&xt Column/Quadrant\tCtrl+Right"),
#endif
        mainWindow
    );
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
    actionOutlineNew->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_B));
#endif


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

    actionOutlineClone = new QAction(QIcon(":/menu-icons/copy.svg"), tr("&Clone"), mainWindow);
    actionOutlineClone->setStatusTip(tr("Make copy of the current Notebook"));

    actionOutlineArtExamine= new QAction(QIcon(":/menu-icons/on.svg"), tr("E&xamine"), mainWindow);
    actionOutlineArtExamine->setStatusTip(tr("Turn Notebook to deck of flashcard and start active recall testing..."));

    actionOutlineForget = new QAction(QIcon(":/menu-icons/delete.svg"), tr("&Forget"), mainWindow);
    actionOutlineForget->setStatusTip(tr("Forget Notebook and move it to Limbo"));

    actionOutlinePromote = new QAction(QIcon(":/menu-icons/left.svg"), tr("&Promote"), mainWindow);
    actionOutlinePromote->setStatusTip(tr("Promote Notebook"));

    actionOutlineDemote = new QAction(QIcon(":/menu-icons/right.svg"), tr("De&mote"), mainWindow);
    actionOutlineDemote->setStatusTip(tr("Demote Notebook"));

    actionOutlineFirst = new QAction(
        QIcon(":/menu-icons/top.svg"), tr("Move to &First"),
        mainWindow);
    actionOutlineFirst->setStatusTip(tr("Move the Notebook to be the first child of its parent"));

    actionOutlineUp = new QAction(
        QIcon(":/menu-icons/up.svg"), tr("Move &Up"), mainWindow);
    actionOutlineUp->setStatusTip(tr("Move the Notebook up"));

    actionOutlineDown = new QAction(
        QIcon(":/menu-icons/down.svg"), tr("Move Do&wn"), mainWindow);
    actionOutlineDown->setStatusTip(tr("Move the Notebook down"));

    actionOutlineLast = new QAction(
        QIcon(":/menu-icons/bottom.svg"), tr("Move to &Last"), mainWindow);
    actionOutlineLast->setStatusTip(tr("Move the Notebook to be the last child of its parent"));

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
    menuOutline->addAction(actionOutlinePromote);
    menuOutline->addAction(actionOutlineDemote);
    menuOutline->addSeparator();
    menuOutline->addAction(actionOutlineFirst);
    menuOutline->addAction(actionOutlineUp);
    menuOutline->addAction(actionOutlineDown);
    menuOutline->addAction(actionOutlineLast);
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

    actionNoteExternalEdit = new QAction(QIcon(":/menu-icons/edit_external.svg"), tr("E&xternal Editor Edit\tCtrl+X"), mainWindow);
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

    actionNoteFirst = new QAction(
        QIcon(":/menu-icons/top.svg"), tr("Move to F&irst\tCtrl+Shift+Up"),
        mainWindow);
    actionNoteFirst->setStatusTip(tr("Move the Note to be the first child of its parent"));

    actionNoteUp = new QAction(
        QIcon(":/menu-icons/up.svg"), tr("Move &Up\tCtrl+Up"), mainWindow);
    actionNoteUp->setStatusTip(tr("Move the Note up"));

    actionNoteDown = new QAction(
        QIcon(":/menu-icons/down.svg"), tr("Move Do&wn\tCtrl+Down"), mainWindow);
    actionNoteDown->setStatusTip(tr("Move the Note down"));

    actionNoteLast = new QAction(
        QIcon(":/menu-icons/bottom.svg"),
        tr("Move to &Last\tCtrl+Shift+Down"),
        mainWindow);
    actionNoteLast->setStatusTip(tr("Move the Note to be the last child of its parent"));

    actionNoteRefactor = new QAction(
        QIcon(":/menu-icons/refactor.svg"),
#ifdef __APPLE__
        tr("Move to Notebook\tCtrl+R"),
#else
        tr("&Move to Notebook"),
#endif
        mainWindow
    );
    actionNoteRefactor->setStatusTip(tr("Move the current Note to another Notebook..."));

    actionNoteStencil = new QAction(
        QIcon(":/menu-icons/stencil.svg"),
        tr("Make &Stencil"),
        mainWindow
    );
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
    menuNote->addAction(actionNoteClone);
    menuNote->addAction(actionNoteRefactor);
#ifdef MF_WIP
    menuNote->addAction(actionNoteStencil);
#endif
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

    actionEditRunTool = new QAction(QIcon(":/menu-icons/on.svg"), tr("Find Knowledge\tCtrl+/"), mainWindow);
    actionEditRunTool->setStatusTip(tr("Run an external tool to find, explain, process text under the cursor"));

    actionEditComplete = new QAction(QIcon(":/menu-icons/link.svg"), tr("Complete Link\tCtrl+L"), mainWindow);
    actionEditComplete->setStatusTip(tr("Complete word being written by finding link to Notebook or Note"));

    actionEditSpellCheck = new QAction(QIcon(":/menu-icons/paste.svg"), tr("Sp&ell Check"), mainWindow);
    actionEditSpellCheck->setStatusTip(tr("Spell check Notebook or Note description"));

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
    menuEdit->addAction(actionEditRunTool);
    menuEdit->addAction(actionEditComplete);
    menuEdit->addAction(actionEditExtract);
    menuEdit->addSeparator();
    menuEdit->addAction(actionEditSpellCheck);
    menuEdit->setEnabled(false);

    // menu: format
    menuFormat = qMenuBar->addMenu(tr("&Format"));

    actionFormatBold = new QAction(tr("&Bold"), mainWindow);
    actionFormatBold->setStatusTip(tr("Format text as bold"));
#ifdef __APPLE__
    actionFormatBold->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_B));
#endif

    actionFormatItalic = new QAction(tr("&Italic"), mainWindow);
    actionFormatItalic->setStatusTip(tr("Format text as italic"));
#ifdef __APPLE__
    actionFormatItalic->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_I));
#endif

    actionFormatCode = new QAction(tr("&Code"), mainWindow);
    actionFormatCode->setStatusTip(tr("Format text as inlined source code"));
#ifdef __APPLE__
    actionFormatCode->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_O));
#endif

    actionFormatMath = new QAction(tr("&Math"), mainWindow);
    actionFormatMath->setStatusTip(tr("Format text as math (MathJax)"));

    actionFormatKeyboard = new QAction(tr("&Keyboard"), mainWindow);
    actionFormatKeyboard->setStatusTip(tr("Format text as keyboard input"));

    actionFormatComment= new QAction(tr("Comment"), mainWindow);
    actionFormatComment->setStatusTip(tr("Add comment to hide text in rendered HTML"));

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

    submenuFormatLists->addSeparator();

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

    // toc
    submenuFormatToc = menuFormat->addMenu(tr("T&able of Contents"));
    actionFormatTocWithoutTags = new QAction(tr("With&out tags"), mainWindow);
    actionFormatTocWithoutTags ->setStatusTip(tr("Insert Notebook's table of contents without tags"));
    submenuFormatToc->addAction(actionFormatTocWithoutTags);
    actionFormatTocWithTags = new QAction(tr("&With tags"), mainWindow);
    actionFormatTocWithTags ->setStatusTip(tr("Insert Notebook's table of contents with tags"));
    submenuFormatToc->addAction(actionFormatTocWithTags);

    actionFormatTimestamp = new QAction(tr("Timestam&p"), mainWindow);
    actionFormatTimestamp ->setStatusTip(tr("Insert current date and time"));

    actionFormatLink = new QAction(tr("&Link"), mainWindow);
    actionFormatLink->setStatusTip(tr("Insert link to a document, image or file"));
#ifdef __APPLE__
    actionFormatLink->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_K));
#endif

    actionFormatImage = new QAction(tr("Ima&ge"), mainWindow);
    actionFormatImage->setStatusTip(tr("Insert image"));
#ifdef __APPLE__
    actionFormatImage->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_M));
#endif

    actionFormatTable = new QAction(tr("Tabl&es"), mainWindow);
    actionFormatTable->setStatusTip(tr("Insert table..."));

    actionFormatHr = new QAction(tr("&Horizontal ruler"), mainWindow);
    actionFormatHr->setStatusTip(tr("Horizontal ruler"));

    actionFormatEmojis = new QAction(tr("Emo&jis"), mainWindow);
    actionFormatEmojis->setStatusTip(
        tr("Open dialog with emoji characters to be copy/pasted to names, descriptions and text...")
    );

    menuFormat->addAction(actionFormatBold);
    menuFormat->addAction(actionFormatItalic);
    menuFormat->addAction(actionFormatCode);
    menuFormat->addAction(actionFormatMath);
    menuFormat->addAction(actionFormatStrikethrough);
    menuFormat->addAction(actionFormatKeyboard);
    menuFormat->addAction(actionFormatComment);
    menuFormat->addSeparator();
    menuFormat->addMenu(submenuFormatToc);
    menuFormat->addMenu(submenuFormatLists);
    menuFormat->addMenu(submenuFormatBlocks);
    menuFormat->addMenu(submenuFormatDiagrams);
    menuFormat->addAction(actionFormatTable);
    menuFormat->addMenu(submenuFormatMathJax);
    menuFormat->addAction(actionFormatTimestamp);
    menuFormat->addAction(actionFormatHr);
    menuFormat->addAction(actionFormatEmojis);
    menuFormat->addSeparator();
    menuFormat->addAction(actionFormatLink);
    menuFormat->addAction(actionFormatImage);
    menuFormat->setEnabled(false);

    // menu: help

    actionHelpDocumentation = new QAction(
        QIcon(":/menu-icons/help.svg"), tr("&Documentation"), mainWindow);
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

    actionHelpAboutQt = new QAction(QIcon(":/menu-icons/about_qt.svg"), tr("About &Qt"), mainWindow);
    actionHelpAboutQt->setStatusTip(tr("About Qt..."));

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
#ifndef __APPLE__
    menuHelp->addSeparator();
#endif
    menuHelp->addAction(actionHelpAboutQt);
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
    actionMindScope->setEnabled(true);
    actionExit->setEnabled(true);

    menuFind->setEnabled(true);
    actionFindFts->setEnabled(true);
    actionFindOutlineByName->setEnabled(true);
    actionFindOutlineByTag->setEnabled(true);
    actionFindNoteByName->setEnabled(true);
    actionFindNoteByTag->setEnabled(true);
#ifdef MF_WIP
    actionFindDocByName->setEnabled(true);
#endif

    menuView->setEnabled(true);
    actionViewDashboard->setEnabled(true);
    actionViewHome->setEnabled(true);
    actionViewOrganizers->setEnabled(true);
    actionViewOutlinesMap->setEnabled(true);
    actionViewOutlines->setEnabled(true);
    actionViewTags->setEnabled(true);
    actionViewNavigator->setEnabled(true);
#ifdef MF_WIP
    actionViewLibraryDocs->setEnabled(true);
    actionViewDecks->setEnabled(true);
#endif
    actionViewLimbo->setEnabled(true);
    actionViewRecentNotes->setEnabled(true);

    menuLibrary->setEnabled(true);
    actionLibraryAdd->setEnabled(true);
    actionLibraryDeprecate->setEnabled(true);

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
    actionOutlineEdit->setEnabled(true);
    actionOutlineClone->setEnabled(true);
    actionOutlineHome->setEnabled(true);
    actionOutlineForget->setEnabled(true);
    actionOutlineUp->setEnabled(true);
    actionOutlineDown->setEnabled(true);
    actionOutlineFirst->setEnabled(true);
    actionOutlineLast->setEnabled(true);
    actionOutlinePromote->setEnabled(true);
    actionOutlineDemote->setEnabled(true);
        actionOutlineHome->setEnabled(true);
#ifdef MF_WIP
    actionOutlineArtExamine->setEnabled(true);
#endif
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

void MainMenuView::showModeAwareFacet(bool repositoryMode, bool mfMode)
{
   if(!repositoryMode) {
        menuView->setEnabled(false);
        menuLibrary->setEnabled(false);
        menuOutline->setEnabled(false);
        menuEdit->setEnabled(false);
        menuFormat->setEnabled(false);

        actionMindScope->setEnabled(false);

        actionFindOutlineByName->setEnabled(false);
        actionFindOutlineByTag->setEnabled(false);
    }
    if(!mfMode) {
        menuLibrary->setEnabled(false);
#ifdef MF_WIP
        menuFlashcards->setEnabled(false);
#endif

        actionMindScope->setEnabled(false);

        actionViewHome->setEnabled(false);
        actionViewOrganizers->setEnabled(false);
        actionViewOutlinesMap->setEnabled(false);
#ifdef MF_WIP
        actionViewLibraryDocs->setEnabled(false);
#endif
        actionViewTags->setEnabled(false);
        actionViewLimbo->setEnabled(false);
#ifdef MF_WIP
        actionViewDashboard->setEnabled(false);
        actionViewDecks->setEnabled(false);
#endif

        actionFindOutlineByTag->setEnabled(false);
        actionFindNoteByTag->setEnabled(false);
#ifdef MF_WIP
        actionFindDocByName->setEnabled(false);
#endif

        actionOutlineHome->setEnabled(false);
#ifdef MF_WIP
        actionOutlineArtExamine->setEnabled(false);
#endif
    }
}


void MainMenuView::showFacetOrganizerList(bool repositoryMode, bool mfMode)
{
    showAllMenuItems();

    actionOrganizerEdit->setEnabled(false);
    actionOrganizerClone->setEnabled(false);
    actionOrganizerForget->setEnabled(false);
    actionOrganizerFocusPrevious->setEnabled(false);
    actionOrganizerFocusNext->setEnabled(false);
    actionOrganizerMovePrevious->setEnabled(false);
    actionOrganizerMoveNext->setEnabled(false);

    menuNavigator->setEnabled(false);
    menuLibrary->setEnabled(false);
    menuOutline->setEnabled(false);
    menuNote->setEnabled(false);
    menuEdit->setEnabled(false);
    menuFormat->setEnabled(false);
    submenuOutlineExport->setEnabled(false);

    showModeAwareFacet(repositoryMode, mfMode);
}

void MainMenuView::showFacetOrganizerView(bool repositoryMode, bool mfMode)
{
    showAllMenuItems();

    menuNavigator->setEnabled(false);
#ifdef MF_WIP
    menuLibrary->setEnabled(false);
#endif
    menuOutline->setEnabled(false);
    menuNote->setEnabled(false);
    menuEdit->setEnabled(false);
    menuFormat->setEnabled(false);
    submenuOutlineExport->setEnabled(false);

    showModeAwareFacet(repositoryMode, mfMode);
}

void MainMenuView::showFacetOutlineList(bool repositoryMode, bool mfMode)
{
    showAllMenuItems();

    menuNavigator->setEnabled(false);
    menuOrganizer->setEnabled(false);
    menuEdit->setEnabled(false);
    menuFormat->setEnabled(false);
    menuNote->setEnabled(false);
    submenuOutlineExport->setEnabled(false);

    actionOutlineEdit->setEnabled(false);
    actionOutlineClone->setEnabled(false);
    actionOutlineHome->setEnabled(false);
    actionOutlineForget->setEnabled(false);
    actionOutlineUp->setEnabled(false);
    actionOutlineDown->setEnabled(false);
    actionOutlineFirst->setEnabled(false);
    actionOutlineLast->setEnabled(false);
    actionOutlinePromote->setEnabled(false);
    actionOutlineDemote->setEnabled(false);

    showModeAwareFacet(repositoryMode, mfMode);
}

void MainMenuView::showFacetOutlinesMap(bool repositoryMode, bool mfMode)
{
    showAllMenuItems();

    menuNavigator->setEnabled(false);
    menuOrganizer->setEnabled(false);
    menuEdit->setEnabled(false);
    menuFormat->setEnabled(false);
    menuNote->setEnabled(false);
    submenuOutlineExport->setEnabled(false);

    showModeAwareFacet(repositoryMode, mfMode);
}

void MainMenuView::showFacetOutlineView(bool repositoryMode, bool mfMode)
{
    showAllMenuItems();

    menuNavigator->setEnabled(false);
#ifdef MF_WIP
    menuLibrary->setEnabled(false);
#endif
    menuOrganizer->setEnabled(false);
    menuEdit->setEnabled(false);
    menuFormat->setEnabled(false);

    actionOutlineUp->setEnabled(false);
    actionOutlineDown->setEnabled(false);
    actionOutlineFirst->setEnabled(false);
    actionOutlineLast->setEnabled(false);
    actionOutlinePromote->setEnabled(false);
    actionOutlineDemote->setEnabled(false);

    showModeAwareFacet(repositoryMode, mfMode);
}

void MainMenuView::showFacetNoteEdit(bool repositoryMode, bool mfMode)
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

    showModeAwareFacet(repositoryMode, mfMode);
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
