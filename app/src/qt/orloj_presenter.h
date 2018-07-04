/*
 outline_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_ORLOJ_PRESENTER_H
#define M8RUI_ORLOJ_PRESENTER_H

#include <iostream>

#include "../../lib/src/debug.h"
#include "../../lib/src/mind/mind.h"

#include <QtWidgets>

#include "orloj_view.h"
#include "organizer_presenter.h"
#include "tags_table_presenter.h"
#include "recent_notes_table_presenter.h"
#include "main_window_presenter.h"
#include "outlines_table_presenter.h"
#include "notes_table_presenter.h"
#include "outline_view_presenter.h"
#include "outline_header_view_presenter.h"
#include "outline_header_edit_presenter.h"
#include "note_view_presenter.h"
#include "note_edit_presenter.h"

namespace m8r {

class OrganizerPresenter;
class TagCloudPresenter;
class NotePresenter;
class NoteViewPresenter;
class NoteEditPresenter;
class OutlineHeaderViewPresenter;
class OutlineHeaderEditPresenter;
class OutlineViewPresenter;

enum OrlojPresenterFacets {
    FACET_NONE,                 // 0
    FACET_LIST_OUTLINES,        // 1
    FACET_VIEW_OUTLINE,         // 2
    FACET_VIEW_OUTLINE_HEADER,  // 3
    FACET_EDIT_OUTLINE_HEADER,  // 4
    FACET_VIEW_NOTE,            // 5
    FACET_EDIT_NOTE,            // 6
    FACET_FTS_RESULT,           // 7
    FACET_FTS_VIEW_NOTE,        // 8
    FACET_ORGANIZER,            // 9
    FACET_TAG_CLOUD,            // 10
    FACET_RECENT_NOTES          // 11
};

class OrlojPresenter : public QObject
{
    Q_OBJECT

private:
    MainWindowPresenter* mainPresenter;

    OrlojPresenterFacets activeFacet;

    OrlojView* view;
    Mind* mind;

    OrganizerPresenter* organizerPresenter;
    TagsTablePresenter* tagCloudPresenter;
    OutlinesTablePresenter* outlinesTablePresenter;
    NotesTablePresenter* notesTablePresenter;
    RecentNotesTablePresenter* recentNotesTablePresenter;
    OutlineViewPresenter* outlineViewPresenter;
    OutlineHeaderViewPresenter* outlineHeaderViewPresenter;
    OutlineHeaderEditPresenter* outlineHeaderEditPresenter;
    NoteViewPresenter* noteViewPresenter;
    NoteEditPresenter* noteEditPresenter;

public:
    OrlojPresenter(MainWindowPresenter* mainPresenter,
                   OrlojView* view,
                   Mind* mind);

    Mind* getMind() { return mind; }

    OrlojView* getView() const { return view; }
    OrganizerPresenter* getOrganizer() const { return organizerPresenter; }
    MainWindowPresenter* getMainWindow() const { return mainPresenter; }
    OutlinesTablePresenter* getOutlinesTable() const { return outlinesTablePresenter; }
    NotesTablePresenter* getNotesTable() const { return notesTablePresenter; }
    RecentNotesTablePresenter* getRecentNotesTable() const { return recentNotesTablePresenter; }
    OutlineViewPresenter* getOutlineView() const { return outlineViewPresenter; }
    OutlineHeaderViewPresenter* getOutlineHeaderView() const { return outlineHeaderViewPresenter; }
    OutlineHeaderEditPresenter* getOutlineHeaderEdit() const { return outlineHeaderEditPresenter; }
    NoteViewPresenter* getNoteView() const { return noteViewPresenter; }
    NoteEditPresenter* getNoteEdit() const { return noteEditPresenter; }

    bool isFacetActive(const OrlojPresenterFacets facet) const { return activeFacet==facet;}
    bool isFacetActiveOutlineManagement() {
        if(isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
             ||
           isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)
             ||
           isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)
             ||
           isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
             ||
           isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER))
        {
            return true;
        } else {
            return false;
        }
    }

    OrlojPresenterFacets getFacet() const { return activeFacet; }
    void setFacet(OrlojPresenterFacets facet) {
        onFacetChange(facet);
        activeFacet = facet;
    }

    /**
     * @brief This method is invoked whenever a facet is changed i.e. it allows to perform desired actions.
     */
    void onFacetChange(const OrlojPresenterFacets targetFacet) const;

    void showFacetOrganizer(const std::vector<Outline*>& outlines);
    void showFacetTagCloud();
    void showFacetOutlineList(const std::vector<Outline*>& outlines);
    void showFacetRecentNotes(const std::vector<Note*>& notes);
    void showFacetFtsResult(std::vector<Note*>* result);
    void showFacetOutline(Outline* outline);
    void showFacetNoteView();
    void showFacetNoteView(Note* note);
    void showFacetNoteEdit(Note* note);
    void showFacetOutlineHeaderEdit(Outline* outline);

    void toggleCurrentFacetHoisting();

    void fromOutlineHeaderEditBackToView(Outline* outline);
    void fromNoteEditBackToView(Note* note);

public slots:
    void slotShowOutlines();
    void slotShowOutline(const QItemSelection& selected, const QItemSelection& deselected);
    void slotShowOutlineHeader();
    void slotShowNote(const QItemSelection& selected, const QItemSelection& deselected);
    void slotShowNoteAsFtsResult(const QItemSelection& selected, const QItemSelection& deselected);
    void slotShowRecentNote(const QItemSelection& selected, const QItemSelection& deselected);
    void slotShowTagRecallDialog(const QItemSelection& selected, const QItemSelection& deselected);
};

} // m8r namespace

#endif // M8RUI_ORLOJ_PRESENTER_H
