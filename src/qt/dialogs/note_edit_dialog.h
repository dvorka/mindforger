/*
 note_edit_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_NOTE_EDIT_DIALOG_H
#define M8RUI_NOTE_EDIT_DIALOG_H

#include <QtWidgets>

#include "lib/src/mind/ontology/ontology_vocabulary.h"
#include "lib/src/mind/ontology/ontology.h"

#include "src/qt/model_meta_definitions.h"

namespace m8r {

class NoteEditDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel* typeLabel;
    QComboBox* typeCombo;
    QLabel* tagLabel;
    QComboBox* tagCombo;
    QLabel* progressLabel;
    QSpinBox* progressSpin;
    // TODO deadline - via QCalendar + my own serialization/ts creation
    QLabel* deadlineLabel;

    QLabel* createdLabel;
    QLineEdit* createdLine;
    QLabel* modifiedLabel;
    QLineEdit* modifiedLine;
    QLabel* readLabel;
    QLineEdit* readLine;
    QLabel* readsLabel;
    QLineEdit* readsLine;
    QLabel* writesLabel;
    QLineEdit* writesLine;
    QLabel* locationLabel;
    QLineEdit* locationLine;

    QDialogButtonBox *buttonBox;

    Ontology& ontology;

    Note* currentNote;

public:
    explicit NoteEditDialog(Ontology& ontology, QWidget* parent);
    NoteEditDialog(const NoteEditDialog&) = delete;
    NoteEditDialog(const NoteEditDialog&&) = delete;
    NoteEditDialog &operator=(const NoteEditDialog&) = delete;
    NoteEditDialog &operator=(const NoteEditDialog&&) = delete;
    ~NoteEditDialog();

    void setCurrentNote(Note* note) { currentNote = note; }

    void show();
};

}
#endif // M8RUI_NOTE_EDIT_DIALOG_H
