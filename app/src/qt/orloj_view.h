/*
 orloj_view.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_ORLOJ_VIEW_H
#define M8RUI_ORLOJ_VIEW_H

#include <QtWidgets>

#include "dashboard_view.h"
#include "organizers_table_view.h"
#include "organizer_view.h"
#include "kanban_view.h"
#include "tags_table_view.h"
#include "outlines_table_view.h"
#include "notes_table_view.h"
#include "recent_notes_table_view.h"
#include "outline_view_splitter.h"
#include "outline_header_view.h"
#include "outline_header_edit_view.h"
#include "note_view.h"
#include "note_edit_view.h"
#include "main_menu_view.h"
#include "navigator/navigator_view.h"

namespace m8r {

class MainMenuView;
class NoteEditorView;
class OutlineHeaderEditView;

/**
 * @brief The Orloj view.
 *
 * Orloj provides 0 horizontal frames that are shown/hidden
 * based on the desired view:
 *
 * 1. home, organizer, outlines, navigator, fts/... note search result
 * 2. outline tree, outline hoist
 * 3. note view, outline edit
 * 4. note edit
 * ...
 *
 */
class OrlojView : public QSplitter
{
    Q_OBJECT

private:
    DashboardView* dashboard;
    OrganizersTableView* organizersTable;
    OrganizerView* organizer;
    KanbanView* kanban;
    TagsTableView* tagCloud;
    OutlinesTableView* outlinesTable;
    RecentNotesTableView* recentNotesTable;
    OutlineViewSplitter* outlineView;
    OutlineHeaderView* outlineHeaderView;
    OutlineHeaderEditView* outlineHeaderEdit;
    NoteView* noteView;
    NoteEditView* noteEdit;

    MainMenuView* menuView;
    NavigatorView* navigator;

    int leftPanelIndex = -1;
    int savedLeftPanelSize = 0;

public:
    explicit OrlojView(QWidget* parent);
    OrlojView(const OrlojView&) = delete;
    OrlojView(const OrlojView&&) = delete;
    OrlojView &operator=(const OrlojView&) = delete;
    OrlojView &operator=(const OrlojView&&) = delete;
    virtual ~OrlojView() {};

    DashboardView* getDashboard() const { return dashboard; }
    OrganizerView* getOrganizer() const { return organizer; }
    OrganizersTableView* getOrganizersTable() const { return organizersTable; }
    KanbanView* getKanban() const { return kanban; }
    TagsTableView* getTagCloud() const { return tagCloud; }
    OutlinesTableView* getOutlinesTable() const { return outlinesTable; }
    RecentNotesTableView* getRecentNotesTable() const { return recentNotesTable; }
    OutlineViewSplitter* getOutlineView() const { return outlineView; }
    OutlineHeaderView* getOutlineHeaderView() const { return outlineHeaderView; }
    OutlineHeaderEditView* getOutlineHeaderEdit() const { return outlineHeaderEdit; }
    NoteView* getNoteView() const { return noteView; }
    NoteEditView* getNoteEdit() const { return noteEdit; }
    NavigatorView* getNavigator() const { return navigator; }

    void setMainMenu(MainMenuView* menuView) { this->menuView=menuView; }

    /**
     * @brief Dashboard
     */
    void showFacetDashboard();

    /**
     * @brief List of Organizers
     */
    void showFacetOrganizers();

    /**
     * @brief Organizer
     */
    void showFacetOrganizer();

    /**
     * @brief Kanban
     */
    void showFacetKanban();

    /**
     * @brief Tag cloud
     */
    void showFacetTagCloud();

    /**
     * @brief List of Outlines
     */
    void showFacetOutlines();

    /**
     * @brief Outline detail: name and tree of Notes
     */
    void showFacetOutlinesDetail();

    /**
     * @brief Recent Notes.
     */
    void showFacetRecentNotes();

    /**
     * @brief Outline header view
     */
    void showFacetOutlineHeaderView();

    /**
     * @brief Outline header edit
     */
    void showFacetOutlineHeaderEdit();

    /**
     * @brief Note view
     */
    void showFacetNoteView();

    /**
     * @brief Note edit
     */
    void showFacetNoteEdit();

    /**
     * @brief Knowledge graph navigator
     */
    void showFacetNavigator();
    void showFacetNavigatorOutline();
    void showFacetNavigatorNote();

    /*
     * Hoisting
     */

    bool isHoisting() const;
    void showFacetHoistedOutlineHeaderView();
    void showFacetHoistedOutlineHeaderEdit();
    void showFacetHoistedNoteView();
    void showFacetHoistedNoteEdit();

private:
    /**
     * @brief Hide all children except listed ones.
     */
    void hideChildren(const QList<QWidget*>& visibleChildren);
};

}

#endif // M8RUI_ORLOJ_VIEW_H
