/*
 edit_name_panel.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_EDIT_NAME_PANEL_H
#define M8R_EDIT_NAME_PANEL_H

#include <QtWidgets>

#include "mf_widgets.h"
#include "../dialogs/note_edit_dialog.h"
#include "../dialogs/outline_header_edit_dialog.h"

namespace m8r {

class EditNamePanel : public QWidget
{
    Q_OBJECT

private:
    MfWidgetMode mode;

    QHBoxLayout* layout;
    QLabel* label;
    QLineEdit* lineEdit;

    OutlineHeaderEditDialog* outlineHeaderEditDialog;
    NoteEditDialog* noteEditDialog;

public:
    explicit EditNamePanel(MfWidgetMode mode, QWidget* parent);
    EditNamePanel(const EditNamePanel&) = delete;
    EditNamePanel(const EditNamePanel&&) = delete;
    EditNamePanel &operator=(const EditNamePanel&) = delete;
    EditNamePanel &operator=(const EditNamePanel&&) = delete;
    ~EditNamePanel();

/*
 * NOTE mode
 */

    void setNote(Note* note) {
        lineEdit->setText(QString::fromStdString(note->getName()));
    }

/*
 * OUTLINE mode
 */

    void setOutline(Outline* outline) {
        lineEdit->setText(QString::fromStdString(outline->getName()));
    }

public:
    QString getName() const { return lineEdit->text(); }
};

}
#endif // M8R_EDIT_NAME_PANEL_H
