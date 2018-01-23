/*
 note_edit_presenter.cpp     MindForger thinking notebook

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
#include "note_edit_presenter.h"

#include "look_n_feel.h"

using namespace std;

namespace m8r {

NoteEditPresenter::NoteEditPresenter(
        NoteEditView* view,
        MainWindowPresenter* mwp,
        QObject* parent) : QObject(parent)
{
    this->view = view;    
    this->mainPresenter = mwp;
    this->model = new NoteEditModel{};
    // IMPROVE View/Model to be used this->view->setModel(this->model);
    this->mdRepresentation
        = new MarkdownOutlineRepresentation{mwp->getMind()->ontology()};

    noteEditDialog
        = new NoteEditDialog{mwp->getMind()->remind().getOntology(), view};
    this->view->setNoteEditDialog(noteEditDialog);
    this->view->setMarkdownRepresentation(mdRepresentation);

    // signals
    QObject::connect(
        view, SIGNAL(signalSaveNote()),
        this, SLOT(slotSaveNote()));
    QObject::connect(
        view, SIGNAL(signalCloseEditor()),
        this, SLOT(slotCloseEditor()));
    QObject::connect(
        view, SIGNAL(signalSaveAndCloseEditor()),
        this, SLOT(slotSaveAndCloseEditor()));
}

void NoteEditPresenter::setNote(Note* note)
{
    this->currentNote = note;
    view->setNote(note);
}

void NoteEditPresenter::slotCloseEditor(void)
{
    mainPresenter->getOrloj()->fromNoteEditBackToView(currentNote);
}

void NoteEditPresenter::slotSaveAndCloseEditor(void)
{
    slotSaveNote();

    if(!view->isNoteDescriptionEmpty()) {
        mainPresenter->getOrloj()->fromNoteEditBackToView(currentNote);
    }
}

void NoteEditPresenter::slotSaveNote(void)
{
    // set UI data to current note
    view->toNote();

    // remember
    mainPresenter->getMind()->remind().remember(currentNote->getOutlineKey());
    mainPresenter->getStatusBar()->showInfo("Note saved!");
}

NoteEditPresenter::~NoteEditPresenter()
{
    if(mdRepresentation) delete mdRepresentation;
}

}
