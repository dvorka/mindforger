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

#include "../../lib/src/gear/lang_utils.h"

using namespace std;

namespace m8r {

OrlojPresenter::OrlojPresenter(MainWindowPresenter* mainPresenter,
                               OrlojView* view,
                               Mind* mind)
    : activeFacet{OrlojPresenterFacets::FACET_NONE}
{
    this->mainPresenter = mainPresenter;
    this->view = view;
    this->mind = mind;

    this->organizerPresenter = new OrganizerPresenter(view->getOrganizer(), this);
    this->tagCloudPresenter = new TagsTablePresenter(view->getTagCloud(), mainPresenter->getHtmlRepresentation());
    this->outlinesTablePresenter = new OutlinesTablePresenter(view->getOutlinesTable(), mainPresenter->getHtmlRepresentation());
    this->notesTablePresenter = new NotesTablePresenter(view->getNotesTable());
    this->recentNotesTablePresenter = new RecentNotesTablePresenter(view->getRecentNotesTable(), mainPresenter->getHtmlRepresentation());
    this->outlineViewPresenter = new OutlineViewPresenter(view->getOutlineView(), this);
    this->outlineHeaderViewPresenter = new OutlineHeaderViewPresenter(view->getOutlineHeaderView(), this);
    this->outlineHeaderEditPresenter = new OutlineHeaderEditPresenter(view->getOutlineHeaderEdit(), mainPresenter, this);
    this->noteViewPresenter = new NoteViewPresenter(view->getNoteView(), this);
    this->noteEditPresenter = new NoteEditPresenter(view->getNoteEdit(), mainPresenter, this);
#ifdef MF_WIP
    this->navigatorPresenter = new NavigatorPresenter(view->getNavigator(), this, mind->getKnowledgeGraph());
#endif

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
        SLOT(slotShowNoteAsFtsResult(const QItemSelection&, const QItemSelection&)));
    // click Tag in Tags to view Recall by Tag detail
    QObject::connect(
        view->getRecentNotesTable()->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this,
        SLOT(slotShowRecentNote(const QItemSelection&, const QItemSelection&)));
    // click Tag in Tags to view Recall by Tag detail
    QObject::connect(
        view->getTagCloud()->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this,
        SLOT(slotShowTagRecallDialog(const QItemSelection&, const QItemSelection&)));
}

int dialogSaveOrCancel()
{
    QMessageBox msgBox{};
    msgBox.setText("Do you want to save changes to the edited Note?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    return msgBox.exec();
}

void OrlojPresenter::onFacetChange(const OrlojPresenterFacets targetFacet) const
{
    MF_DEBUG("Facet CHANGE: " << activeFacet << " > " << targetFacet << endl);

    if(activeFacet == OrlojPresenterFacets::FACET_EDIT_NOTE) {
        if(targetFacet != OrlojPresenterFacets::FACET_VIEW_NOTE) {
            int decision = dialogSaveOrCancel();
            if(decision==QMessageBox::Save) {
                  noteEditPresenter->slotSaveNote();
            }
            return;
        }
    } else if(activeFacet == OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER) {
        if(targetFacet != OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER) {
            int decision = dialogSaveOrCancel();
            if(decision==QMessageBox::Save) {
                outlineHeaderEditPresenter->slotSaveOutlineHeader();
            }
            return;
        }
    }

    if(targetFacet == OrlojPresenterFacets::FACET_VIEW_OUTLINE) {
        outlineViewPresenter->getOutlineTree()->focus();
    }
}

void OrlojPresenter::showFacetRecentNotes(const vector<Note*>& notes)
{
    setFacet(OrlojPresenterFacets::FACET_RECENT_NOTES);
    recentNotesTablePresenter->refresh(notes);
    view->showFacetRecentNotes();
    mainPresenter->getStatusBar()->showMindStatistics();
}

void OrlojPresenter::showFacetOrganizer(const vector<Outline*>& outlines)
{
    setFacet(OrlojPresenterFacets::FACET_ORGANIZER);
    organizerPresenter->refresh(outlines);
    view->showFacetOrganizer();
    mainPresenter->getMainMenu()->showFacetOrganizer();
    mainPresenter->getStatusBar()->showMindStatistics();
}

void OrlojPresenter::showFacetKnowledgeGraphNavigator()
{
    setFacet(OrlojPresenterFacets::FACET_NAVIGATOR);
    navigatorPresenter->showInitialView();
    view->showFacetKnowledgeGraphNavigator();
    mainPresenter->getStatusBar()->showMindStatistics();
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

void OrlojPresenter::slotShowOutlines()
{
    showFacetOutlineList(mind->getOutlines());
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

    outline->incReads();
    outline->makeDirty();

    mainPresenter->getMainMenu()->showFacetOutlineView();
    mainPresenter->getStatusBar()->showInfo(QString("Notebook '%1'   %2").arg(outline->getName().c_str()).arg(outline->getKey().c_str()));
}

void OrlojPresenter::slotShowOutline(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    if(activeFacet!=OrlojPresenterFacets::FACET_ORGANIZER
         &&
       activeFacet!=OrlojPresenterFacets::FACET_TAG_CLOUD
         &&
       activeFacet!=OrlojPresenterFacets::FACET_RECENT_NOTES
      )
    {
        QModelIndexList indices = selected.indexes();
        if(indices.size()) {
            const QModelIndex& index = indices.at(0);
            QStandardItem* item = outlinesTablePresenter->getModel()->itemFromIndex(index);
            // TODO introduce name my user role - replace constant with my enum name > do it for whole file e.g. MfDataRole
            Outline* outline = item->data(Qt::UserRole + 1).value<Outline*>();
            showFacetOutline(outline);
        } else {
            mainPresenter->getStatusBar()->showInfo(QString(tr("No Notebook selected!")));
        }
    }
}

void OrlojPresenter::showFacetTagCloud()
{
    setFacet(OrlojPresenterFacets::FACET_TAG_CLOUD);

    map<const Tag*,int> tags{};
    mind->getTagsCardinality(tags);
    tagCloudPresenter->refresh(tags);

    view->showFacetTagCloud();

    mainPresenter->getStatusBar()->showInfo(QString("%2 Tags").arg(tags.size()));
}

void OrlojPresenter::slotShowTagRecallDialog(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    if(activeFacet == OrlojPresenterFacets::FACET_TAG_CLOUD) {
        QModelIndexList indices = selected.indexes();
        if(indices.size()) {
            const QModelIndex& index = indices.at(0);
            QStandardItem* item = tagCloudPresenter->getModel()->itemFromIndex(index);
            // TODO introduce name my user role - replace constant with my enum name > do it for whole file e.g. MfDataRole
            const Tag* tag = item->data(Qt::UserRole + 1).value<const Tag*>();
            mainPresenter->doTriggerFindNoteByTag(tag);
        } else {
            mainPresenter->getStatusBar()->showInfo(QString(tr("No Tag selected!")));
        }
    }
}


void OrlojPresenter::showFacetNoteView()
{
    view->showFacetNoteView();
    mainPresenter->getMainMenu()->showFacetOutlineView();
    setFacet(OrlojPresenterFacets::FACET_VIEW_NOTE);
}

void OrlojPresenter::showFacetNoteView(Note* note)
{
    if(outlineViewPresenter->getCurrentOutline()!=note->getOutline()) {
        showFacetOutline(note->getOutline());
    }
    noteViewPresenter->refresh(note);
    view->showFacetNoteView();
    outlineViewPresenter->selectRowByNote(note);
    mainPresenter->getMainMenu()->showFacetOutlineView();

    QString p{note->getOutline()->getKey().c_str()};
    p += "#";
    p += note->getMangledName().c_str();
    mainPresenter->getStatusBar()->showInfo(QString(tr("Note '%1'   %2")).arg(note->getName().c_str()).arg(p));
    setFacet(OrlojPresenterFacets::FACET_VIEW_NOTE);
}

void OrlojPresenter::showFacetNoteEdit(Note* note)
{
    // if there is FTS result table on the left, then switch to O view facet & N view w/ Note selection, otherwise edit
    if(notesTablePresenter->getView()->isVisible()) {
        showFacetNoteView(note);
    } else {
        noteEditPresenter->setNote(note);
        view->showFacetNoteEdit();
        setFacet(OrlojPresenterFacets::FACET_EDIT_NOTE);
        mainPresenter->getMainMenu()->showFacetNoteEdit();
    }
}

void OrlojPresenter::showFacetOutlineHeaderEdit(Outline* outline)
{
    outlineHeaderEditPresenter->setOutline(outline);
    view->showFacetOutlineHeaderEdit();
    setFacet(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER);
    mainPresenter->getMainMenu()->showFacetNoteEdit();
}

void OrlojPresenter::toggleCurrentFacetHoisting()
{
    if(view->isHoistView()) {
        if(isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
             ||
           isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)) {
            view->showFacetOutlineHeaderView();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)) {
            view->showFacetNoteView();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
            view->showFacetNoteEdit();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
            view->showFacetOutlineHeaderEdit();
        }
    } else {
        if(isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
             ||
           isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)) {
            view->showFacetHoistedOutlineHeaderView();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_VIEW_NOTE)) {
            view->showFacetHoistedNoteView();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_NOTE)) {
            view->showFacetHoistedNoteEdit();
        } else if(isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
            view->showFacetHoistedOutlineHeaderEdit();
        }
    }
}

void OrlojPresenter::fromOutlineHeaderEditBackToView(Outline* outline)
{
    // LEFT: update O name above Ns tree
    outlineViewPresenter->refresh(outline->getName());
    // RIGHT
    outlineHeaderViewPresenter->refresh(outline);
    view->showFacetOutlineHeaderView();
    setFacet(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER);
    mainPresenter->getMainMenu()->showFacetOutlineView();
}

void OrlojPresenter::fromNoteEditBackToView(Note* note)
{
    noteViewPresenter->clearSearchExpression();
    noteViewPresenter->refresh(note);
    // update N in the tree
    outlineViewPresenter->refresh(note);

    showFacetNoteView();
}

void OrlojPresenter::slotShowOutlineHeader()
{
    outlineHeaderViewPresenter->refresh(outlineViewPresenter->getCurrentOutline());
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

        note->incReads();
        note->makeDirty();

        showFacetNoteView(note);
    } else {
        Outline* outline = outlineViewPresenter->getCurrentOutline();
        mainPresenter->getStatusBar()->showInfo(QString("Notebook '%1'   %2").arg(outline->getName().c_str()).arg(outline->getKey().c_str()));
    }
}

void OrlojPresenter::slotShowNoteAsFtsResult(const QItemSelection& selected, const QItemSelection& deselected)
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
        mainPresenter->getStatusBar()->showInfo(QString(note->getName().c_str()));
        setFacet(OrlojPresenterFacets::FACET_FTS_VIEW_NOTE);
    } else {
        mainPresenter->getStatusBar()->showInfo(QString(tr("No Notebook selected!")));
    }
}

void OrlojPresenter::slotShowRecentNote(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    if(activeFacet == OrlojPresenterFacets::FACET_RECENT_NOTES) {
        QModelIndexList indices = selected.indexes();
        if(indices.size()) {
            const QModelIndex& index = indices.at(0);
            QStandardItem* item = recentNotesTablePresenter->getModel()->itemFromIndex(index);
            // TODO make my role constant
            const Note* note = item->data(Qt::UserRole + 1).value<const Note*>();

            showFacetOutline(note->getOutline());
            showFacetNoteView();
            getOutlineView()->selectRowByNote(note);
            // IMPROVE make this more efficient
            mainPresenter->getStatusBar()->showInfo(QString(tr("Note "))+QString::fromStdString(note->getName()));
        } else {
            mainPresenter->getStatusBar()->showInfo(QString(tr("No Note selected!")));
        }
    }
}

} // m8r namespace
