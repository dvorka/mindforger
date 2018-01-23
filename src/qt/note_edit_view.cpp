/*
 note_edit_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "note_edit_view.h"

namespace m8r {

using namespace std;

NoteEditView::NoteEditView(QWidget* parent)
    : QWidget(parent)
{
    // TODO remove: CHECK OUTLINE VIEW: QWidget w/ vertical layout, sizing done there

    // widgets
    editTitleAndButtonsPanel = new EditTitleAndButtonsPanel{this};
    noteEditor = new NoteEditorView{this};

    // assembly
    QVBoxLayout* layout = new QVBoxLayout{this};
    // ensure that wont be extra space around member widgets
    layout->setContentsMargins(QMargins(0,0,0,0));
    layout->addWidget(editTitleAndButtonsPanel);
    layout->addWidget(noteEditor);
    setLayout(layout);

    // signals
    new QShortcut(
        QKeySequence(QKeySequence(Qt::ALT+Qt::Key_Left)),
        this, SLOT(slotSaveAndCloseEditor()));
    new QShortcut(
        QKeySequence(QKeySequence(Qt::CTRL+Qt::Key_S)),
        this, SLOT(slotSaveNote()));
    QObject::connect(
        editTitleAndButtonsPanel->getRememberButton(), SIGNAL(clicked()),
        this, SLOT(slotSaveAndCloseEditor()));
    QObject::connect(
        editTitleAndButtonsPanel->getCancelButton(), SIGNAL(clicked()),
        this, SLOT(slotCloseEditor()));
}

// IMPROVE perhaps this code belong to presenter
void NoteEditView::toNote(void)
{
    if(currentNote) {
        string title{"Note"};
        if(!editTitleAndButtonsPanel->getTitle().isEmpty()) {
            title.assign(editTitleAndButtonsPanel->getTitle().toStdString());
        }
        currentNote->setTitle(title);

        if(!noteEditor->toPlainText().isEmpty()) {
            string s{noteEditor->toPlainText().toStdString()};
            vector<string*> d{};
            mdRepresentation->description(&s, d);
            currentNote->setDescription(d);
        } else {
            currentNote->clearDescription();
        }

        // Note metada (type, tags, progress, deadline) are set by Note edit dialog on it's close
        // (if user doesn't open dialog, nothing is blindly saved there & here)

        // IMPROVE if fields below are set on remembering (save) of Note, then delete code below
        currentNote->setModified();
        currentNote->setModifiedPretty();
        currentNote->setRevision(currentNote->getRevision()+1);
        if(currentNote->getReads()<currentNote->getRevision()) {
            currentNote->setReads(currentNote->getRevision());
        }
        // Note's outline metadata must be updated as well
        currentNote->getOutline()->setModified();
        currentNote->getOutline()->setModifiedPretty();
        currentNote->getOutline()->setRevision(currentNote->getOutline()->getRevision()+1);
    } else {
        QMessageBox::critical(this, tr("Unable to Save Note"), tr("Attempt to save data from UI to Note, but no Note is set."));
    }
}

void NoteEditView::slotSaveAndCloseEditor(void)
{
    emit signalSaveAndCloseEditor();
}

void NoteEditView::slotCloseEditor(void)
{
    emit signalCloseEditor();
}

void NoteEditView::slotSaveNote(void)
{
    emit signalSaveNote();
}

NoteEditView::~NoteEditView()
{
}

}
