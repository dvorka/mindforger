/*
 main_menu.h     MindForger thinking notebook

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
#ifndef M8RUI_MAIN_MENU_H
#define M8RUI_MAIN_MENU_H

#include <QtWidgets>

#include "lib/src/debug.h"

#include "main_menu_presenter.h"
#include "main_window_view.h"

namespace m8r {

class MainWindowView;

/**
 * @brief MindForger main window menu view.
 *
 * View creates all menu items and Qt actions while presenter
 * implements doActions() and connects them using signals/slots
 * to this view.
 */
class MainMenuView : public QObject
{
    Q_OBJECT

private:
    MainWindowView* mainWindow;
    QMenuBar* qMenuBar;

public:    
    QMenu* menuMind;
    QMenu* menuFind;
    QMenu* menuView;
    QMenu* menuOutline;
    QMenu* menuNote;
    QMenu* menuFormat;
    QMenu* menuHelp;

#ifdef DO_MF_DEBUG
    QAction* actionMindHack;
#endif
    // menu: MindForger
    QAction* actionMindDevise;
    QAction* actionMindLearn;
    QAction* actionMindRemember;
    QAction* actionMindRemind;
    QAction* actionMindAssociate;
    QAction* actionMindThink;
    QAction* actionMindForget;
    QAction* actionMindDream;
    QAction* actionMindSnapshot;
    QAction* actionMindPreferences;
    QAction* actionExit;

    // menu: Find
    QAction* actionFts;
    QAction* actionFindOutlineByName;
    QAction* actionFindNoteByName;
    QAction* actionFindOutlineByTag;
    QAction* actionFindNoteByTag;

    // menu: View
    QAction* actionViewHome;
    QAction* actionViewOrganizer;
    QAction* actionViewOutlines;
    QAction* actionViewNavigator;
    QAction* actionViewDwell;
    QAction* actionViewStencils;
#ifdef DO_MF_DEBUG
    QAction* actionViewCli;
#else
    QShortcut* cliShortcut;
#endif
    QAction* actionViewToggleRecent;
    QAction* actionViewLimbo;
    QAction* actionViewDistractionFree;
    QAction* actionViewFullscreen;

    // menu: Format
    QAction* actionFormatBold;
    QAction* actionFormatItalic;
    QAction* actionFormatStrikethrough;
    QAction* actionFormatCode;
    QAction* actionFormatHighlight;
    QAction* actionFormatSubscript;
    QAction* actionFormatSuperscript;
    QAction* actionFormatCodeBlock;
    QAction* actionFormatBlockQuote;
    QAction* actionFormatListNumber;
    QAction* actionFormatListBullet;
    QAction* actionFormatLink;
    QAction* actionFormatImage;
    QAction* actionFormatTable;

    // menu: Outline
    QAction* actionOutlineNew;
    QAction* actionOutlineOpen;
    QAction* actionOutlineClone;
    QAction* actionOutlineForget;
    QAction* actionOutlineExport;
    QAction* actionOutlineImport;

    // menu: Note
    QAction* actionNoteNew;
    QAction* actionNoteOpen;
    QAction* actionNoteSave;
    QAction* actionNoteClose;
    QAction* actionNoteForget;
    QAction* actionNoteFirst;
    QAction* actionNoteUp;
    QAction* actionNotePromote;
    QAction* actionNoteDemote;
    QAction* actionNoteDown;
    QAction* actionNoteLast;
    QAction* actionNoteAttach;
    QAction* actionNoteRefactor;
    QAction* actionNoteClone;
    QAction* actionNoteExport;
    QAction* actionNoteImport;

    // menu: Help
    QAction* actionHelpDocumentation;
    QAction* actionHelpWeb;
    QAction* actionHelpReportBug;
    QAction* actionHelpCheckForUpdates;
    QAction* actionHelpAbout;

public:
    MainMenuView(MainWindowView& mainWindow);
    MainMenuView(const MainMenuView&) = delete;
    MainMenuView(const MainMenuView&&) = delete;
    MainMenuView &operator=(const MainMenuView&) = delete;
    MainMenuView &operator=(const MainMenuView&&) = delete;
    virtual ~MainMenuView();

    void showFacetOutlineList();
    void showFacetOutlineView();
    void showFacetNoteEdit();

private:
    void showAllMenuItems();

private slots:
    void slotShowCli() { emit showCli(); }

signals:
    void showCli();
};

}

#endif // M8RUI_MAIN_MENU_H
