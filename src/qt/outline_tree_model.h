/*
 outline_tree_model.h     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_TREE_MODEL_H
#define M8RUI_OUTLINE_TREE_MODEL_H

#include <QtWidgets>

#include "lib/src/model/note.h"
#include "notes_table_model.h"

#include "gear/qutils.h"

namespace m8r {

class OutlineTreeModel : public QStandardItemModel
{
    Q_OBJECT

private:
    QList<QModelIndex> noselection;
public:
    OutlineTreeModel(QObject *parent);
    OutlineTreeModel(const OutlineTreeModel&) = delete;
    OutlineTreeModel(const OutlineTreeModel&&) = delete;
    OutlineTreeModel &operator=(const OutlineTreeModel&) = delete;
    OutlineTreeModel &operator=(const OutlineTreeModel&&) = delete;

    void removeAllRows(void);
    void addRow(Note* note);
    void refresh(Note* note) { refresh(note, noselection); }
    void refresh(Note* note, QModelIndexList selection);
    void createRowText(QString& title, Note* note);
};

}

#endif // M8RUI_OUTLINE_TREE_MODEL_H
