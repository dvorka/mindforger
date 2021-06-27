/*
 find_note_by_name_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_FIND_NOTE_BY_NAME_DIALOG_H
#define M8RUI_FIND_NOTE_BY_NAME_DIALOG_H

#include <vector>

#include <QtWidgets>

#include "../../lib/src/model/outline.h"

#include "find_outline_by_name_dialog.h"

namespace m8r {

class FindNoteByNameDialog : public FindOutlineByNameDialog
{
    Q_OBJECT

private:
    Outline* scope;

public:
    explicit FindNoteByNameDialog(QWidget* parent);
    FindNoteByNameDialog(const FindNoteByNameDialog&) = delete;
    FindNoteByNameDialog(const FindNoteByNameDialog&&) = delete;
    FindNoteByNameDialog &operator=(const FindNoteByNameDialog&) = delete;
    FindNoteByNameDialog &operator=(const FindNoteByNameDialog&&) = delete;
    ~FindNoteByNameDialog();

    void clearScope() { scope = nullptr; }
    void setScope(Outline* o) { scope = o; }
    Outline* getScope() { return scope; }

    void show(std::vector<Note*>&);
};

}
#endif // M8RUI_FIND_NOTE_BY_NAME_DIALOG_H
