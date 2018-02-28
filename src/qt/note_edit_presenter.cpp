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

    view->setEditorShowLineNumbers(Configuration::getInstance().isUiEditorShowLineNumbers());
    view->setEditorEnableSyntaxHighlighting(Configuration::getInstance().isUiEditorEnableSyntaxHighlighting());

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

NoteEditPresenter::~NoteEditPresenter()
{
}

void NoteEditPresenter::setNote(Note* note)
{
    this->currentNote = note;
    string mdDescription{};
    mainPresenter->getMarkdownRepresentation()->toDescription(note, &mdDescription);

    view->setNote(note, mdDescription);
}

void NoteEditPresenter::slotCloseEditor()
{
    mainPresenter->getOrloj()->fromNoteEditBackToView(currentNote);
}

void NoteEditPresenter::slotSaveAndCloseEditor()
{
    slotSaveNote();

    if(!view->isDescriptionEmpty()) {
        mainPresenter->getOrloj()->fromNoteEditBackToView(currentNote);
    }
}

void NoteEditPresenter::slotSaveNote()
{
    // set UI data to current note
    if(currentNote) {
        string name{"Note"};
        if(!view->getName().isEmpty()) {
            name.assign(view->getName().toStdString());
        }
        currentNote->setName(name);

        if(!view->isDescriptionEmpty()) {
            string s{view->getDescription().toStdString()};
            vector<string*> d{};
            mainPresenter->getMarkdownRepresentation()->description(&s, d);
            currentNote->setDescription(d);
        } else {
            currentNote->clearDescription();
        }

        // Note metada (type, tags, progress, deadline) are set by Note edit dialog on it's close
        // (if user doesn't open dialog, nothing is blindly saved there & here)

        // IMPROVE if fields below are set on remembering (save) of Note, then delete code below
        currentNote->makeModified();
        if(currentNote->getReads()<currentNote->getRevision()) {
            currentNote->setReads(currentNote->getRevision());
        }
        // Note's outline metadata must be updated as well
        currentNote->getOutline()->makeModified();

        // remember
        mainPresenter->getMind()->remind().remember(currentNote->getOutlineKey());
        mainPresenter->getStatusBar()->showInfo(tr("Note saved!"));
#ifdef DO_M8F_DEBUG
        qDebug() << "Note " << QString::fromStdString(currentNote->getName()) << " saved!";
#endif
    } else {
        mainPresenter->getStatusBar()->showError(tr("Attempt to save data from UI to Note, but no Note is set."));
    }
}

}
