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

// IMPROVE md propagation is not nice (create it here (deepest presenter point)?)
void NoteEditPresenter::setNote(Note* note, string* md)
{
    this->currentNote = note;
    view->setNote(note);
    view->setDescription(*md);
}

void NoteEditPresenter::slotCloseEditor(void)
{
    mainPresenter->getOrloj()->fromNoteEditBackToView(currentNote);
}

void NoteEditPresenter::slotSaveAndCloseEditor(void)
{
    view->toNote();
    slotSaveNote();

    if(!view->isNoteDescriptionEmpty()) {
        mainPresenter->getOrloj()->fromNoteEditBackToView(currentNote);
    }
}

void NoteEditPresenter::slotSaveNote(void)
{
    QString text = view->getDescription();

    if(!text.isEmpty()) {
        // IMPROVE try to find a more efficient conversion
        string s = text.toStdString();
        Note* note = mdRepresentation->note(&s);

        // IMPROVE move this code to note - it updates existing note OR mind.learn()
        currentNote->setTitle(note->getTitle());
        currentNote->setDescription(note->getDescription());
        //currentNote->setDeadline();
        //currentNote->setProgress();
        //currentNote->setTags();
        //currentNote->setType();
        // set on remembering (save) of note
        currentNote->setModified();
        currentNote->setModifiedPretty();
        currentNote->setRevision(currentNote->getRevision()+1);
        if(currentNote->getReads()<currentNote->getRevision()) currentNote->setReads(currentNote->getRevision());
        // TODO delete note (vectors to be kept)

        // Note's outline metadata must be updated as well
        currentNote->getOutline()->setModified();
        currentNote->getOutline()->setModifiedPretty();
        currentNote->getOutline()->setRevision(currentNote->getOutline()->getRevision()+1);

        mainPresenter->getMind()->remind().remember(currentNote->getOutlineKey());
        mainPresenter->getStatusBar()->showInfo("Note saved!");
    } else {
        mainPresenter->getStatusBar()->showError("Note text is empty - it was NOT saved!");
    }
}

NoteEditPresenter::~NoteEditPresenter()
{
    if(mdRepresentation) delete mdRepresentation;
}

}
