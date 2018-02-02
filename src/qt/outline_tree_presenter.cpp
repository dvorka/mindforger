/*
 outline_tree_presenter.cpp     MindForger thinking notebook

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
#include "outline_tree_presenter.h"

namespace m8r {

using namespace std;

OutlineTreePresenter::OutlineTreePresenter(OutlineTreeView* view, MainWindowPresenter* mwp, QObject* parent)
    : QObject(parent)
{
    this->view = view;
    this->model = new OutlineTreeModel{view};
    this->view->setModel(this->model);

    // ensure HTML cells rendering
    HtmlDelegate* delegate = new HtmlDelegate{};
    this->view->setItemDelegate(delegate);

    // signals and slots
    QObject::connect(view, SIGNAL(signalSelectNextRow()), this, SLOT(slotSelectNextRow()));
    QObject::connect(view, SIGNAL(signalSelectLastRow()), this, SLOT(slotSelectLastRow()));

    QObject::connect(view, SIGNAL(signalChangePromote()), mwp, SLOT(doActionNotePromote()));
    QObject::connect(view, SIGNAL(signalChangeDemote()), mwp, SLOT(doActionNoteDemote()));
}

OutlineTreePresenter::~OutlineTreePresenter()
{
    if(model) delete model;
}

void OutlineTreePresenter::refresh(Outline* outline, Outline::Patch* patch)
{
    if(outline) {
        if(patch) {
            const vector<Note*>& notes = outline->getNotes();
            switch(patch->diff) {
            case Outline::Patch::Diff::CHANGE:
                for(unsigned int i=patch->start; i<=patch->start+patch->count; i++) {
                    model->refresh(notes[i], i);
                }
                break;
            default:
                break;
            }
        } else {
            model->removeAllRows();
            for(Note* note:outline->getNotes()) {
                model->addRow(note);
            }
        }
    }
}

void OutlineTreePresenter::refresh(Note* note)
{
    QItemSelectionModel *select = view->selectionModel();
    if(select->hasSelection()) {
        model->refresh(note, select->selectedRows());
    } else {
        model->refresh(note);
    }
}

void OutlineTreePresenter::clearSelection()
{
    view->clearSelection();
}

void OutlineTreePresenter::selectRowByNote(const Note* note)
{
    if(note) {
        int row = model->getRowByNote(note);
        if(row >= 0) {
            view->selectRow(row);
            return;
        }
    }
    view->clearSelection();
}

int OutlineTreePresenter::getCurrentRow() const
{
    QModelIndexList indexes = view->selectionModel()->selection().indexes();
    for(int i=0; i<indexes.count(); i++) {
        return indexes.at(i).row();
    }
    // IMPROVE constant w/ a name
    return -1;
}

Note* OutlineTreePresenter::getCurrentNote() const
{
    int row = getCurrentRow();
    // IMPROVE constant w/ a name
    if(row != -1) {
        return model->item(row)->data().value<Note*>();
    } else {
        return nullptr;
    }
}

void OutlineTreePresenter::slotSelectNextRow()
{
    int row = getCurrentRow();
    if(row < model->rowCount()-1) {
        QModelIndex nextIndex = model->index(row+1, 0);
        view->setCurrentIndex(nextIndex);
    }
}

void OutlineTreePresenter::slotSelectLastRow()
{
    int row = getCurrentRow();
    if(row) {
        QModelIndex previousIndex = model->index(row-1, 0);
        view->setCurrentIndex(previousIndex);
    }
}

}
