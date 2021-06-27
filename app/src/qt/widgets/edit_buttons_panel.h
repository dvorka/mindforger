/*
 edit_buttons_panel.h     MindForger thinking notebook

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
#ifndef M8R_EDIT_BUTTONS_PANEL_H
#define M8R_EDIT_BUTTONS_PANEL_H

#include <QtWidgets>

#include "mf_widgets.h"
#include "../dialogs/note_edit_dialog.h"
#include "../dialogs/outline_header_edit_dialog.h"

namespace m8r {

class EditButtonsPanel : public QWidget
{
    Q_OBJECT

private:
    MfWidgetMode mode;

    QHBoxLayout* layout;
    QPushButton* previewButton;
    QPushButton* moreButton;
    QPushButton* rememberButton;
    QPushButton* cancelButton;

    OutlineHeaderEditDialog* outlineHeaderEditDialog;
    NoteEditDialog* noteEditDialog;

public:
    explicit EditButtonsPanel(MfWidgetMode mode, QWidget* parent);
    EditButtonsPanel(const EditButtonsPanel&) = delete;
    EditButtonsPanel(const EditButtonsPanel&&) = delete;
    EditButtonsPanel &operator=(const EditButtonsPanel&) = delete;
    EditButtonsPanel &operator=(const EditButtonsPanel&&) = delete;
    ~EditButtonsPanel();

signals:
    void signalShowLivePreview();

public:
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
    }

public slots:
    void handleShowNoteEditDialog();
    void handleCloseNoteEditDialog();

/*
 * OUTLINE mode
 */

public:
    void setOutlineHeaderEditDialog(OutlineHeaderEditDialog* dialog) {
        this->outlineHeaderEditDialog = dialog;
        // signals can be set after dialog instance is available
        // IMPROVE wiring to QDialog::accept doesn't from some reason :-/
        QObject::connect(dialog, SIGNAL(acceptedSignal()), this, SLOT(handleCloseOutlineHeaderEditDialog()));
    }
    void setOutline(Outline* outline) {
        outlineHeaderEditDialog->setOutline(outline);
    }

public slots:
    void handleShowLivePreview();
    void handleShowOutlineHeaderEditDialog();
    void handleCloseOutlineHeaderEditDialog();

public:
    QPushButton* getRememberButton() const { return rememberButton; }
    QPushButton* getCancelButton() const { return cancelButton; }

};

}
#endif // M8R_EDIT_BUTTONS_PANEL_H
