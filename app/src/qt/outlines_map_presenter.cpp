/*
 outlines_map_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "outlines_map_presenter.h"

namespace m8r {

using namespace std;

OutlinesMapPresenter::OutlinesMapPresenter(
    OutlinesMapView* view, MainWindowPresenter* mwp, QObject* parent
) : QObject(parent)
{
    this->view = view;
    this->model = new OutlinesMapModel{view, mwp->getHtmlRepresentation()};
    this->view->setModel(this->model);
    this->mind = mwp->getMind();

    // ensure HTML cells rendering
    HtmlDelegate* delegate = new HtmlDelegate{};
    this->view->setItemDelegate(delegate);

    // signals
    QObject::connect(
        view, SIGNAL(signalSelectNextRow()),
        this, SLOT(slotSelectNextRow()));
    QObject::connect(
        view, SIGNAL(signalSelectPreviousRow()),
        this, SLOT(slotSelectPreviousRow()));

    QObject::connect(
        view, SIGNAL(signalOutlineShow()),
        mwp, SLOT(doActionOutlineShow()));

    QObject::connect(
        view, SIGNAL(signalChangePromote()),
        mwp, SLOT(doActionNotePromote()));
    QObject::connect(
        view, SIGNAL(signalChangeDemote()),
        mwp, SLOT(doActionNoteDemote()));
    QObject::connect(
        view, SIGNAL(signalChangeFirst()),
        mwp, SLOT(doActionNoteFirst()));
    QObject::connect(
        view, SIGNAL(signalChangeUp()),
        mwp, SLOT(doActionNoteUp()));
    QObject::connect(
        view, SIGNAL(signalChangeDown()),
        mwp, SLOT(doActionNoteDown()));
    QObject::connect(
        view, SIGNAL(signalChangeLast()),
        mwp, SLOT(doActionNoteLast()));

    QObject::connect(
        view, SIGNAL(signalOutlineOrNoteEdit()),
        mwp, SLOT(doActionOutlineOrNoteEdit()));
    QObject::connect(
        view, SIGNAL(signalOutlineOrNoteExternalEdit()),
        mwp, SLOT(doActionNoteExternalEdit()));
    QObject::connect(
        view, SIGNAL(signalEdit()),
        mwp, SLOT(doActionNoteEdit()));
    QObject::connect(
        view, SIGNAL(signalForget()),
        mwp, SLOT(doActionNoteForget()));
}

OutlinesMapPresenter::~OutlinesMapPresenter()
{
    if(model) delete model;
}

void OutlinesMapPresenter::refresh(Outline* outline, Outline::Patch* patch)
{
    // If FORGET aspect is used, then only VIEW is filtered, but operations are performed
    // on the non-filtered O's Ns. UI view serves just as a FILTER of what can be changed.
    // Therefore anything special is needed on the backend.
    //   Unfortunately PATCH will NOT help if VIEW is filtered and everyhing must be
    // refreshed.
    if(outline) {
        if(patch) {
            const vector<Note*>& notes = outline->getNotes();
            switch(patch->diff) {
            case Outline::Patch::Diff::CHANGE:
                for(unsigned int i=patch->start; i<=patch->start+patch->count; i++) {
                    model->refresh(notes[i], i, false);
                }
                break;
            case Outline::Patch::Diff::MOVE:
                for(unsigned int i=patch->start; i<=patch->start+patch->count; i++) {
                    model->refresh(notes[i], i, true);
                }
                break;
            default:
                break;
            }
        } else {
            model->removeAllRows();
            for(Note* note:outline->getNotes()) {
                model->addNote(note);
            }
        }

        // forget / time scope: hide view rows ~ there is full model, I just hide what's visible > patch should work
        if(mind->getScopeAspect().isEnabled()) {
            vector<int> parents;
            for(size_t i=0; i<outline->getNotesCount(); i++) {
                if(mind->getScopeAspect().isInScope(outline->getNotes()[i])) {
                    // N's parents
                    parents.clear();
                    outline->getNotePathToRoot(i, parents);
                    if(parents.size()) {
                        for(size_t p=0; p<parents.size(); p++) {
                            view->showRow(parents[p]);
                        }
                    }
                    // N
                    view->showRow(i);
                } else {
                    view->hideRow(i);
                }
            }
        }
    }
}

void OutlinesMapPresenter::refresh(Note* note)
{
    QItemSelectionModel *select = view->selectionModel();
    if(select->hasSelection()) {
        model->refresh(note, select->selectedRows());
    } else {
        model->refresh(note);
    }
}

void OutlinesMapPresenter::selectRow(int row)
{
    view->scrollTo(model->index(row, 0));
    view->selectRow(row);
}

void OutlinesMapPresenter::insertAndSelect(Note* note)
{
    this->selectRow(model->insertNote(note));
}

void OutlinesMapPresenter::clearSelection()
{
    view->clearSelection();
}

void OutlinesMapPresenter::selectRowByNote(const Note* note)
{
    if(note) {
        int row = model->getRowByNote(note);
        if(row >= 0) {
            view->selectRow(row);
            view->scrollTo(model->index(row, 0));
            return;
        }
    }
    view->clearSelection();
}

int OutlinesMapPresenter::getCurrentRow() const
{
    QModelIndexList indexes = view->selectionModel()->selection().indexes();
    for(int i=0; i<indexes.count(); i++) {
        return indexes.at(i).row();
    }
    return NO_ROW;
}

Note* OutlinesMapPresenter::getCurrentNote() const
{
    int row = getCurrentRow();
    // IMPROVE constant w/ a name
    if(row != -1) {
        return model->item(row)->data().value<Note*>();
    }

    return nullptr;
}

/**
 * @brief Get adjacent N - adjacent above or below N.
 *
 * @return adjacent note or NULL if no such N.
 */
Note* OutlinesMapPresenter::getAdjacentNote() const
{
    int row = getCurrentRow();
    if(row != NO_ROW) {
        if(row > 0) {
            return model->item(row-1)->data().value<Note*>();
        }
        // ELSE row == 0 and child row cannot be selected
        // as its not clear upfront whether/how many
        // children will be deleted
        // therefore it is expected that this function
        // returns nullptr, so that row 0 can be selected
        // AFTER N is deleted (if any row is remaining in O)
    }

    return nullptr;
}

void OutlinesMapPresenter::slotSelectPreviousRow()
{
    int row = getCurrentRow();
    if(row) {
        QModelIndex previousIndex = model->index(row-1, 0);
        view->setCurrentIndex(previousIndex);
    }
}

void OutlinesMapPresenter::slotSelectNextRow()
{
    int row = getCurrentRow();
    if(row < model->rowCount()-1) {
        QModelIndex nextIndex = model->index(row+1, 0);
        view->setCurrentIndex(nextIndex);
    }
}

} // m8r namespace
