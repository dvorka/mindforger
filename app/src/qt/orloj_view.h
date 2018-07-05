/*
 outline_view.h     MindForger thinking notebook

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
#ifndef M8RUI_ORLOJ_VIEW_H
#define M8RUI_ORLOJ_VIEW_H

#include <QtWidgets>

#include "organizer_view.h"
#include "tags_table_view.h"
#include "outlines_table_view.h"
#include "notes_table_view.h"
#include "recent_notes_table_view.h"
#include "outline_view_splitter.h"
#include "outline_header_view.h"
#include "outline_header_edit_view.h"
#include "note_view.h"
#include "note_edit_view.h"
#include "3rdparty/elasticnodes/graphwidget.h"

namespace m8r {

class NoteEditorView;
class OutlineHeaderEditView;

/**
 * @brief The Orloj view.
 *
 * Orloj provides 0 horizontal frames that are shown/hidden
 * based on the desired view:
 *
 * 1: home, organizer, outlines, navigator, fts/... note search result
 * 2: outline tree, outline hoist
 * 3: note view, outline edit
 * 4: note edit
 * ...
 */
class OrlojView : public QSplitter
{
    Q_OBJECT

private:
    OrganizerView* organizer;
    TagsTableView* tagCloud;
    OutlinesTableView* outlinesTable;
    NotesTableView* notesTable;
    RecentNotesTableView* recentNotesTable;
    OutlineViewSplitter* outlineView;
    OutlineHeaderView* outlineHeaderView;
    OutlineHeaderEditView* outlineHeaderEdit;
    NoteView* noteView;
    NoteEditView* noteEdit;

    NavigatorView* navigator;

public:
    explicit OrlojView(QWidget* parent);
    OrlojView(const OrlojView&) = delete;
    OrlojView(const OrlojView&&) = delete;
    OrlojView &operator=(const OrlojView&) = delete;
    OrlojView &operator=(const OrlojView&&) = delete;

    OrganizerView* getOrganizer() const { return organizer; }
    TagsTableView* getTagCloud() const { return tagCloud; }
    OutlinesTableView* getOutlinesTable() const { return outlinesTable; }
    NotesTableView* getNotesTable() const { return notesTable; }
    RecentNotesTableView* getRecentNotesTable() const { return recentNotesTable; }
    OutlineViewSplitter* getOutlineView() const { return outlineView; }
    OutlineHeaderView* getOutlineHeaderView() const { return outlineHeaderView; }
    OutlineHeaderEditView* getOutlineHeaderEdit() const { return outlineHeaderEdit; }
    NoteView* getNoteView() const { return noteView; }
    NoteEditView* getNoteEdit() const { return noteEdit; }
    NavigatorView* getNavigator() const { return navigator; }

    /**
     * @brief Organizer
     */
    void showFacetOrganizer();

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
     * @brief FTS result list - Notes w/o detail
     */
    void showFacetFtsResult();

    /**
     * @brief FTS result list - Notes w/ detail
     */
    void showFacetFtsResultDetail();

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
    void showFacetKnowledgeGraphNavigator();

    /*
     * Hoisting
     */

    bool isHoistView();
    void showFacetHoistedOutlineHeaderView();
    void showFacetHoistedOutlineHeaderEdit();
    void showFacetHoistedNoteView();
    void showFacetHoistedNoteEdit();

private:
    /**
     * @brief Hide all children except listed ones.
     */
    void hideChildren(const QSet<QWidget*>& visibleChildren);

    /**
     * @brief Application window may be resized - this method to be called
     * on resize/regularly.
     */
    void fiftyFifty();
};

}

#endif // M8RUI_ORLOJ_VIEW_H
