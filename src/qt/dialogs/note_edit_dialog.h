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

#include "../model_meta_definitions.h"
#include "../widgets/edit_tags_panel.h"
#include "../widgets/labeled_edit_line_panel.h"
#include "../gear/qutils.h"

namespace m8r {

class NoteEditDialog : public QDialog
{
    Q_OBJECT

private:
    Note* currentNote;

    QLabel* typeLabel;
    QComboBox* typeCombo;
    QLabel* progressLabel;
    QSpinBox* progressSpin;
    QCheckBox* deadlineCheck;
    QDateEdit* deadlineEdit;
    QLabel* parentRelLabel;
    QComboBox* parentRelCombo;

    QLabel* createdLabel;
    QLineEdit* createdLine;
    LabeledEditLinePanel* modifiedPanel;
    LabeledEditLinePanel* readPanel;
    LabeledEditLinePanel* readsPanel;
    LabeledEditLinePanel* writesPanel;
    QLineEdit* writesLine;
    QLabel* locationLabel;
    QLineEdit* locationLine;

    EditTagsPanel* editTagsGroup;
    QDialogButtonBox *buttonBox;

    Ontology& ontology;

public:
    explicit NoteEditDialog(Ontology& ontology, QWidget* parent);
    NoteEditDialog(const NoteEditDialog&) = delete;
    NoteEditDialog(const NoteEditDialog&&) = delete;
    NoteEditDialog &operator=(const NoteEditDialog&) = delete;
    NoteEditDialog &operator=(const NoteEditDialog&&) = delete;
    ~NoteEditDialog();

    /**
     * @brief Set current Note.
     */
    void setNote(Note* note) { currentNote = note; }
    /**
     * @brief Set dialog's data to Note.
     */
    void toNote();

    void show();

signals:
    void acceptedSignal();
    void rejectedSignal();

private slots:
    void handleDeadlineCheck(int state);
    void handleAccepted();
    void handleRejected();
};

}
#endif // M8RUI_NOTE_EDIT_DIALOG_H
