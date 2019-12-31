/*
 note_edit_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "note_edit_view.h"

namespace m8r {

using namespace std;

NoteEditView::NoteEditView(QWidget* parent)
    : QWidget(parent)
{
    // widgets
    topNamePanel = new EditNameAndButtonsPanel{EditNameAndButtonsPanel::Mode::NOTE_MODE, this};
    noteEditor = new NoteEditorView{this};
    bottomButtonsPanel = new EditButtonsPanel{EditButtonsPanel::Mode::NOTE_MODE, this};

    // assembly
    QVBoxLayout* layout = new QVBoxLayout{this};
    // ensure that wont be extra space around member widgets
    layout->setContentsMargins(QMargins(0,0,0,0));
    layout->addWidget(topNamePanel);
    layout->addWidget(noteEditor);
    layout->addWidget(bottomButtonsPanel);
    setLayout(layout);

    // signals
    new QShortcut(
        QKeySequence(QKeySequence(Qt::ALT+Qt::Key_Left)),
        this, SLOT(slotSaveAndCloseEditor()));
    new QShortcut(
        QKeySequence(QKeySequence(Qt::CTRL+Qt::Key_G)),
        this, SLOT(slotCloseEditor()));
    new QShortcut(
        QKeySequence(QKeySequence(Qt::ALT+Qt::Key_Return)),
        this, SLOT(slotOpenNotePropertiesEditor()));
    new QShortcut(
        QKeySequence(QKeySequence(Qt::CTRL+Qt::Key_S)),
        this, SLOT(slotSaveNote()));
    QObject::connect(
        bottomButtonsPanel->getRememberButton(), SIGNAL(clicked()),
        this, SLOT(slotSaveAndCloseEditor()));
    QObject::connect(
        bottomButtonsPanel->getCancelButton(), SIGNAL(clicked()),
        this, SLOT(slotCloseEditor()));
}

NoteEditView::~NoteEditView()
{
}

void NoteEditView::slotOpenNotePropertiesEditor()
{
    bottomButtonsPanel->handleShowNoteEditDialog();
}

void NoteEditView::slotSaveAndCloseEditor()
{
    emit signalSaveAndCloseEditor();
}

void NoteEditView::slotCloseEditor()
{
    emit signalCloseEditor();
}

void NoteEditView::slotSaveNote()
{
    emit signalSaveNote();
}

} // m8r namespace
