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
#include "orloj_presenter.h"

#include "lib/src/gear/lang_utils.h"

using namespace std;

namespace m8r {

OrlojPresenter::OrlojPresenter(MainWindowPresenter* mainPresenter,
                               OrlojView* view,
                               Mind* mind) : activeFacet{OrlojPresenterFacets::FACET_NONE}
{
    this->mainPresenter = mainPresenter;
    this->view = view;
    this->mind = mind;

    this->outlinesTablePresenter = new OutlinesTablePresenter(view->getOutlinesTable());
    this->notesTablePresenter = new NotesTablePresenter(view->getNotesTable());
    this->outlineViewPresenter = new OutlineViewPresenter(view->getOutlineView(), this);
    this->outlineHeaderViewPresenter = new OutlineHeaderViewPresenter(view->getOutlineHeaderView(), this);
    this->outlineHeaderEditPresenter = new OutlineHeaderEditPresenter(view->getOutlineHeaderEdit(), mainPresenter, this);
    this->noteViewPresenter = new NoteViewPresenter(view->getNoteView(), this);
    this->noteEditPresenter = new NoteEditPresenter(view->getNoteEdit(), mainPresenter, this);

    /* Orloj presenter WIRES signals and slots between VIEWS and PRESENTERS.
     *
     * It's done by Orloj presenter as it has access to all its child windows
     * and widgets - it can show/hide what's needed and then pass control to children.
     */

    // click Outline in Outlines to view Outline detail
    QObject::connect(
        view->getOutlinesTable()->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this,
        SLOT(slotShowOutline(const QItemSelection&, const QItemSelection&)));
    // click Outline tree to view Note
    QObject::connect(
        view->getOutlineView()->getOutlineTree()->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this,
        SLOT(slotShowNote(const QItemSelection&, const QItemSelection&)));
    // click FTS result to view Note
    QObject::connect(
        view->getNotesTable()->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this,
        SLOT(slotShowNoteAsResult(const QItemSelection&, const QItemSelection&)));
}

void OrlojPresenter::showFacetOutlineList(const vector<Outline*>& outlines)
{
    setFacet(OrlojPresenterFacets::FACET_LIST_OUTLINES);
    // IMPROVE reload ONLY if dirty, otherwise just show
    outlinesTablePresenter->refresh(outlines);
    view->showFacetOutlines();
    mainPresenter->getMainMenu()->showFacetOutlineList();
    mainPresenter->getStatusBar()->showMindStatistics();
}

void OrlojPresenter::slotShowOutlines(void)
{
    showFacetOutlineList(mind->remind().getOutlines());
}

void OrlojPresenter::showFacetFtsResult(vector<Note*>* result)
{
    setFacet(OrlojPresenterFacets::FACET_FTS_RESULT);
    notesTablePresenter->refresh(result);
    mainPresenter->getMainMenu()->showFacetOutlineList();
    view->showFacetFtsResult();
}

void OrlojPresenter::showFacetOutline(Outline* outline)
{
    setFacet(OrlojPresenterFacets::FACET_VIEW_OUTLINE);

    outlineViewPresenter->refresh(outline);
    outlineHeaderViewPresenter->refresh(outline);
    view->showFacetOutlineHeaderView();

    mainPresenter->getMainMenu()->showFacetOutlineView();
    mainPresenter->getStatusBar()->showInfo(QString("Outline '%1' selected...").arg(outline->getTitle().c_str()));
}

void OrlojPresenter::slotShowOutline(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    QModelIndexList indices = selected.indexes();
    if(indices.size()) {
        const QModelIndex& index = indices.at(0);
        QStandardItem* item = outlinesTablePresenter->getModel()->itemFromIndex(index);
        // TODO introduce name my user role - replace constant with my enum name > do it for whole file e.g. MfDataRole
        Outline* outline = item->data(Qt::UserRole + 1).value<Outline*>();
        showFacetOutline(outline);
    } else {
        // TODO i18n
        mainPresenter->getStatusBar()->showInfo(QString(tr("No outline selected!")));
    }
}

void OrlojPresenter::showFacetNoteView(void)
{
    view->showFacetNoteView();
    mainPresenter->getMainMenu()->showFacetOutlineView();
    setFacet(OrlojPresenterFacets::FACET_VIEW_NOTE);
}

void OrlojPresenter::showFacetNoteEdit(Note* note)
{
    noteEditPresenter->setNote(note);
    view->showFacetNoteEdit();
    setFacet(OrlojPresenterFacets::FACET_EDIT_NOTE);
    mainPresenter->getMainMenu()->showFacetNoteEdit();
}

void OrlojPresenter::showFacetOutlineHeaderEdit(Outline* outline, std::string* html)
{
    outlineHeaderEditPresenter->setCurrentOutline(outline, html);
    view->showFacetOutlineHeaderEdit();
    setFacet(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER);
    mainPresenter->getMainMenu()->showFacetNoteEdit();
}

void OrlojPresenter::fromOutlineHeaderEditBackToView(Outline* outline)
{
    outlineHeaderViewPresenter->refresh(outline);
    view->showFacetOutlineHeaderView();
    setFacet(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER);
    mainPresenter->getMainMenu()->showFacetOutlineView();
}

void OrlojPresenter::fromNoteEditBackToView(Note* note)
{
    noteViewPresenter->clearSearchExpression();
    noteViewPresenter->refresh(note);
    // update note in outline tree
    outlineViewPresenter->refresh(note);

    showFacetNoteView();
}

void OrlojPresenter::slotShowOutlineHeader(void)
{
    // TODO update view w/ new outline
    view->showFacetOutlineHeaderView();
}

void OrlojPresenter::slotShowNote(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    QModelIndexList indices = selected.indexes();
    if(indices.size()) {
        const QModelIndex& index = indices.at(0);
        QStandardItem* item
            = outlineViewPresenter->getOutlineTree()->getModel()->itemFromIndex(index);
        // IMPROVE make my role constant
        Note* note = item->data(Qt::UserRole + 1).value<Note*>();

        noteViewPresenter->refresh(note);
        view->showFacetNoteView();
        mainPresenter->getMainMenu()->showFacetOutlineView();
        mainPresenter->getStatusBar()->showInfo(QString(note->getTitle().c_str()));
        setFacet(OrlojPresenterFacets::FACET_VIEW_NOTE);
    } else {
        // TODO i18n
        mainPresenter->getStatusBar()->showInfo(QString(tr("No Outline selected!")));
    }
}

void OrlojPresenter::slotShowNoteAsResult(
        const QItemSelection& selected,
        const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    QModelIndexList indices = selected.indexes();
    if(indices.size()) {
        const QModelIndex& index = indices.at(0);
        QStandardItem* item = notesTablePresenter->getModel()->itemFromIndex(index);
        // TODO make my role constant
        Note* note = item->data(Qt::UserRole + 1).value<Note*>();

        noteViewPresenter->refresh(note);
        view->showFacetFtsResultDetail();
        mainPresenter->getMainMenu()->showFacetOutlineList();
        mainPresenter->getStatusBar()->showInfo(QString(note->getTitle().c_str()));
        setFacet(OrlojPresenterFacets::FACET_VIEW_NOTE);
    } else {
        // TODO i18n
        mainPresenter->getStatusBar()->showInfo(QString("No outline selected!"));
    }
}

}
