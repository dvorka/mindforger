/*
 notes_table_model.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_NOTES_TABLE_MODEL_H
#define M8RUI_NOTES_TABLE_MODEL_H

#include <QtWidgets>

#include "model_meta_definitions.h"

namespace m8r {

class NotesTableModel : public QStandardItemModel
{
    Q_OBJECT

public:
    NotesTableModel(QObject *parent = 0);

    void removeAllRows();
    void addRow(Note* note);
};

}
#endif // M8RUI_NOTES_TABLE_MODEL_H
