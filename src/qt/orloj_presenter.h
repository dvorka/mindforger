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

#include "lib/src/mind/mind.h"

#include <QtWidgets>

#include "orloj_view.h"
#include "main_window_presenter.h"
#include "outlines_table_presenter.h"
#include "notes_table_presenter.h"
#include "outline_view_presenter.h"
#include "outline_header_view_presenter.h"
#include "outline_header_edit_presenter.h"
#include "note_view_presenter.h"
#include "note_edit_presenter.h"

namespace m8r {

class NotePresenter;
class NoteViewPresenter;
class NoteEditPresenter;
class OutlineHeaderViewPresenter;
class OutlineHeaderEditPresenter;
class OutlineViewPresenter;

enum OrlojPresenterFacets {
    FACET_NONE,                 // 0
    FACET_FTS_RESULT,           // 1
    FACET_LIST_OUTLINES,        // 2
    FACET_VIEW_OUTLINE,         // 3
    FACET_VIEW_OUTLINE_HEADER,  // 4
    FACET_EDIT_OUTLINE_HEADER,  // 5
    FACET_VIEW_NOTE,            // 6
    FACET_EDIT_NOTE             // 7
};

class OrlojPresenter : public QObject
{
    Q_OBJECT

private:
    MainWindowPresenter* mainPresenter;

    OrlojPresenterFacets activeFacet;

    OrlojView* view;
    Mind* mind;

    OutlinesTablePresenter* outlinesTablePresenter;
    NotesTablePresenter* notesTablePresenter;
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
    OutlinesTablePresenter* getOutlinesTable() const { return outlinesTablePresenter; }
    NotesTablePresenter* getNotesTable() const { return notesTablePresenter; }
    OutlineViewPresenter* getOutlineView() const { return outlineViewPresenter; }
    NoteViewPresenter* getNoteView() const { return noteViewPresenter; }

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

    void showFacetOutlineList(const std::vector<Outline*>& outlines);
    void showFacetFtsResult(std::vector<Note*>* result);
    void showFacetOutline(Outline* outline);
    void showFacetNoteView();
    void showFacetNoteEdit(Note* note);
    void showFacetOutlineHeaderEdit(Outline* outline);

    void fromOutlineHeaderEditBackToView(Outline* outline);
    void fromNoteEditBackToView(Note* note);

public slots:
    void slotShowOutlines();
    void slotShowOutline(const QItemSelection& selected, const QItemSelection& deselected);
    void slotShowOutlineHeader();
    void slotShowNote(const QItemSelection& selected, const QItemSelection& deselected);
    void slotShowNoteAsResult(const QItemSelection& selected, const QItemSelection& deselected);
};

} // m8r namespace

#endif // M8RUI_ORLOJ_PRESENTER_H
