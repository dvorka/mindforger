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
#ifndef ORLOJ_PRESENTER_H
#define ORLOJ_PRESENTER_H

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
    FACET_NONE,
    FACET_FTS_RESULT,
    FACET_LIST_OUTLINES,
    FACET_VIEW_OUTLINE,
    FACET_VIEW_OUTLINE_HEADER,
    FACET_EDIT_OUTLINE_HEADER,
    FACET_VIEW_NOTE,
    FACET_EDIT_NOTE
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

    Mind* getMind(void) { return mind; }

    OrlojView* getView(void) const { return view; }
    OutlinesTablePresenter* getOutlinesTable(void) const { return outlinesTablePresenter; }
    NotesTablePresenter* getNotesTable(void) const { return notesTablePresenter; }
    OutlineViewPresenter* getOutlineView(void) const { return outlineViewPresenter; }

    bool isFacetActive(const OrlojPresenterFacets facet) const { return activeFacet==facet;}
    bool isFacetActiveOutlineManagement(void) {
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

    OrlojPresenterFacets getFacet(void) const { return activeFacet; }
    void setFacet(OrlojPresenterFacets facet) {
        qDebug() << "Changing facet: " << activeFacet << " > " << facet;
        activeFacet = facet;
    }

    void showFacetOutlineList(const std::vector<Outline*>& outlines);
    void showFacetFtsResult(std::vector<Note*>* result);
    void showFacetOutline(Outline* outline);
    void showFacetNoteEdit(Note* note, std::string* md);
    void showFacetOutlineHeaderEdit(Outline* outline, std::string* html);

    void fromOutlineHeaderEditBackToView(Outline* outline);
    void fromNoteEditBackToView(Note* note);

public slots:
    void slotShowOutlines(void);
    void slotShowOutline(const QItemSelection& selected, const QItemSelection& deselected);
    void slotShowOutlineHeader(void);
    void slotShowNote(const QItemSelection& selected, const QItemSelection& deselected);
    void slotShowNoteAsResult(const QItemSelection& selected, const QItemSelection& deselected);
};

} // m8r namespace

#endif // ORLOJ_PRESENTER_H
