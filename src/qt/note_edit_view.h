/*
 note_edit_view.h     MindForger thinking notebook

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
#ifndef M8R_NOTE_EDIT_VIEW_H
#define M8R_NOTE_EDIT_VIEW_H

#include <QtWidgets>

#include "../../lib/src/model/note.h"

#include "note_editor_view.h"
#include "widgets/edit_name_and_buttons_panel.h"

namespace m8r {

class NoteEditView : public QWidget
{
    Q_OBJECT

private:
    Note* currentNote;

    EditNameAndButtonsPanel* editNameAndButtonsPanel;
    NoteEditorView* noteEditor;

public:
    explicit NoteEditView(QWidget* parent);
    NoteEditView(const NoteEditView&) = delete;
    NoteEditView(const NoteEditView&&) = delete;
    NoteEditView &operator=(const NoteEditView&) = delete;
    NoteEditView &operator=(const NoteEditView&&) = delete;
    ~NoteEditView();

    void setNoteEditDialog(NoteEditDialog* noteEditDialog) {
        editNameAndButtonsPanel->setNoteEditDialog(noteEditDialog);
    }
    void setNote(Note* note, std::string mdDescription) {
        currentNote = note;
        editNameAndButtonsPanel->setNote(note);
        noteEditor->setPlainText(QString::fromStdString(mdDescription));
    }
    void setEditorShowLineNumbers(bool show) { noteEditor->setShowLineNumbers(show); }
    void setEditorEnableSyntaxHighlighting(bool enable) { noteEditor->setEnableSyntaxHighlighting(enable); }

    QString getName() const { return editNameAndButtonsPanel->getName(); }
    QString getDescription() const { return noteEditor->toPlainText(); }
    bool isDescriptionEmpty() const { return noteEditor->toPlainText().isEmpty(); }

private slots:
    void slotOpenNotePropertiesEditor();
    void slotSaveNote();
    void slotCloseEditor();
    void slotSaveAndCloseEditor();

signals:
    void signalSaveAndCloseEditor();
    void signalCloseEditor();
    void signalSaveNote();
};

}
#endif // M8R_NOTE_EDIT_VIEW_H
