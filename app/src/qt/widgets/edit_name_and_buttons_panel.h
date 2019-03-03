/*
 edit_name_and_buttons_panel.h     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_EDIT_NAME_AND_BUTTONS_PANEL_H
#define M8R_EDIT_NAME_AND_BUTTONS_PANEL_H

#include <QtWidgets>

#include "../dialogs/note_edit_dialog.h"
#include "../dialogs/outline_header_edit_dialog.h"

namespace m8r {

class EditNameAndButtonsPanel : public QWidget
{
    Q_OBJECT

public:
    enum Mode {
        OUTLINE_MODE,
        NOTE_MODE
    };

private:
    Mode mode;

    QHBoxLayout* layout;
    QLabel* label;
    QLineEdit* lineEdit;
    QPushButton* moreButton;
    QPushButton* rememberButton;
    QPushButton* cancelButton;

    OutlineHeaderEditDialog* outlineHeaderEditDialog;
    NoteEditDialog* noteEditDialog;

public:
    explicit EditNameAndButtonsPanel(Mode mode, QWidget* parent);
    EditNameAndButtonsPanel(const EditNameAndButtonsPanel&) = delete;
    EditNameAndButtonsPanel(const EditNameAndButtonsPanel&&) = delete;
    EditNameAndButtonsPanel &operator=(const EditNameAndButtonsPanel&) = delete;
    EditNameAndButtonsPanel &operator=(const EditNameAndButtonsPanel&&) = delete;
    ~EditNameAndButtonsPanel();

/*
 * NOTE mode
 */

    void setNoteEditDialog(NoteEditDialog* dialog) {
        this->noteEditDialog = dialog;
        // signals can be set after dialog instance is available
        // IMPROVE wiring to QDialog::accept doesn't from some reason :-/
        QObject::connect(dialog, SIGNAL(acceptedSignal()), this, SLOT(handleCloseNoteEditDialog()));
    }
    void setNote(Note* note) {
        noteEditDialog->setNote(note);
        lineEdit->setText(QString::fromStdString(note->getName()));
    }

public slots:
    void handleShowNoteEditDialog();
    void handleCloseNoteEditDialog();

/*
 * OUTLINE mode
 */

    void setOutlineHeaderEditDialog(OutlineHeaderEditDialog* dialog) {
        this->outlineHeaderEditDialog = dialog;
        // signals can be set after dialog instance is available
        // IMPROVE wiring to QDialog::accept doesn't from some reason :-/
        QObject::connect(dialog, SIGNAL(acceptedSignal()), this, SLOT(handleCloseOutlineHeaderEditDialog()));
    }
    void setOutline(Outline* outline) {
        outlineHeaderEditDialog->setOutline(outline);
        lineEdit->setText(QString::fromStdString(outline->getName()));
    }

public slots:
    void handleShowOutlineHeaderEditDialog();
    void handleCloseOutlineHeaderEditDialog();

public:
    QString getName() const { return lineEdit->text(); }
    QPushButton* getRememberButton() const { return rememberButton; }
    QPushButton* getCancelButton() const { return cancelButton; }

};

}
#endif // M8R_EDIT_NAME_AND_BUTTONS_PANEL_H
