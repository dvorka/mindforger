/*
 note_edit_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
    this->mwp = mwp;

    view->setEditorShowLineNumbers(Configuration::getInstance().isUiEditorShowLineNumbers());
    view->setStatusBar(mwp->getStatusBar()->getView());

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
    mwp->getOrloj()->getOutlineView()->getAssocLeaderboard()->getView()->setVisible(false);

    this->currentNote = note;
    string mdDescription{};
    mwp->getMarkdownRepresentation()->toDescription(note, &mdDescription);

    view->setNote(note, mdDescription);    
}

void NoteEditPresenter::slotKeyPressed()
{
    mwp->getMind()->associate();
}

void NoteEditPresenter::slotCloseEditor()
{
    mwp->getOrloj()->fromNoteEditBackToView(currentNote);
}

void NoteEditPresenter::slotSaveAndCloseEditor()
{
    slotSaveNote();
    mwp->getOrloj()->fromNoteEditBackToView(currentNote);
}

void NoteEditPresenter::slotSaveNote()
{
    // set UI data to current note
    if(currentNote) {
        string name{"Note"};
        if(!view->getName().isEmpty()) {
            name.assign(view->getName().toStdString());
        }

        // ensure autolinking indices are updated on N rename
        mwp->getMind()->autolinkUpdate(currentNote->getName(), view->getName().toStdString());

        currentNote->setName(name);

        if(!view->isDescriptionEmpty()) {
            string s{view->getDescription().toStdString()};
            //MF_DEBUG("- BEGIN N description -" << endl << s << "- END N description -" << endl);
            vector<string*> d{};
            mwp->getMarkdownRepresentation()->description(&s, d);
            currentNote->setDescription(d);
        } else {
            currentNote->clearDescription();
        }

        // Note metada (type, tags, progress, deadline) are set by Note edit dialog on it's close
        // (if user doesn't open dialog, nothing is blindly saved there & here)

        currentNote->makeModified();

        // remember
        mwp->getMind()->remember(currentNote->getOutlineKey());
        mwp->getStatusBar()->showInfo(tr("Note '%1' saved").arg(QString::fromStdString(currentNote->getName())));
    } else {
        mwp->getStatusBar()->showError(tr("Attempt to save data from UI to Note, but no Note is set."));
    }
}

} // namespace m8r
