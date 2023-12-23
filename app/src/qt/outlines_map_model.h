/*
 outlines_map_model.h     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_OUTLINES_MAP_MODEL_H
#define M8RUI_OUTLINES_MAP_MODEL_H

#include <string>

#include <QtWidgets>

#include "../../lib/src/model/note.h"
#include "../../lib/src/representations/html/html_outline_representation.h"

#include "notes_table_model.h"
#include "gear/qutils.h"

namespace m8r {

class OutlinesMapModel : public QStandardItemModel
{
    Q_OBJECT

private:
    HtmlOutlineRepresentation* htmlRepresentation;
    QList<QModelIndex> noselection;

public:
    OutlinesMapModel(QObject *parent, HtmlOutlineRepresentation* htmlRepresentation);
    OutlinesMapModel(const OutlinesMapModel&) = delete;
    OutlinesMapModel(const OutlinesMapModel&&) = delete;
    OutlinesMapModel &operator=(const OutlinesMapModel&) = delete;
    OutlinesMapModel &operator=(const OutlinesMapModel&&) = delete;

    void removeAllRows();
    void addNote(Note* note);
    int insertNote(Note* note);
    int getRowByNote(const Note* note);
    void refresh(Note* note) { refresh(note, noselection); }
    void refresh(Note* note, int row, bool set=false);
    void refresh(Note* note, QModelIndexList selection);

private:
    void createNameText(std::string& name, Note* note);
    void createRowFor(Note* note, QList<QStandardItem*>& rowItems);
};

}

#endif // M8RUI_OUTLINES_MAP_MODEL_H
