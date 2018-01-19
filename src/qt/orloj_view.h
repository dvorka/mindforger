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
#ifndef ORLOJ_VIEW_H
#define ORLOJ_VIEW_H

#include <QtWidgets>

#include "outlines_table_view.h"
#include "notes_table_view.h"
#include "outline_view.h"
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
 */
class OrlojView : public QSplitter
{
    Q_OBJECT

private:
    OutlinesTableView* outlinesTable;
    NotesTableView* notesTable;
    OutlineView* outlineView;
    OutlineHeaderView* outlineHeaderView;
    OutlineHeaderEditView* outlineHeaderEdit;
    NoteView* noteView;
    NoteEditView* noteEdit;

    GraphWidget* navigator;

public:
    explicit OrlojView(QWidget* parent);
    OrlojView(const OrlojView&) = delete;
    OrlojView(const OrlojView&&) = delete;
    OrlojView &operator=(const OrlojView&) = delete;
    OrlojView &operator=(const OrlojView&&) = delete;

    OutlinesTableView* getOutlinesTable(void) const { return outlinesTable; }
    NotesTableView* getNotesTable(void) const { return notesTable; }
    OutlineView* getOutlineView(void) const { return outlineView; }
    OutlineHeaderView* getOutlineHeaderView(void) const { return outlineHeaderView; }
    OutlineHeaderEditView* getOutlineHeaderEdit(void) const { return outlineHeaderEdit; }
    NoteView* getNoteView(void) const { return noteView; }
    NoteEditView* getNoteEdit(void) const { return noteEdit; }

    /**
     * @brief List of outlines
     */
    void showFacetOutlines(void);

    /**
     * @brief Outline detail: name and tree of notes
     */
    void showFacetOutlinesDetail(void);

    /**
     * @brief FTS result list - notes w/o detail
     */
    void showFacetFtsResult(void);

    /**
     * @brief FTS result list - notes w/ detail
     */
    void showFacetFtsResultDetail(void);

    /**
     * @brief Outline header view
     */
    void showFacetOutlineHeaderView(void);

    /**
     * @brief Outline header edit
     */
    void showFacetOutlineHeaderEdit(void);

    /**
     * @brief Note view
     */
    void showFacetNoteView(void);

    /**
     * @brief Note edit
     */
    void showFacetNoteEdit(void);

private:
    /**
     * @brief Hide all children except listed ones.
     */
    void hideChildren(const QSet<QWidget*>& visibleChildren);

    /**
     * @brief Application window may be resized - this method to be called
     * on resize/regularly.
     */
    void fiftyFifty(void);
};

}

#endif // ORLOJ_VIEW_H
