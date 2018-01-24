/*
 outline_tree_model.cpp     MindForger thinking notebook

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
#include "outline_tree_model.h"

#include <iostream>

using namespace std;

namespace m8r {

OutlineTreeModel::OutlineTreeModel(QObject *parent)
    : QStandardItemModel(parent)
{
    setColumnCount(1);
    setRowCount(0);
}

void OutlineTreeModel::removeAllRows(void)
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader
            << tr("Notes")
            << tr("Done")
            << tr("Rs")
            << tr("Ws")
            << tr("Modified");
    setHorizontalHeaderLabels(tableHeader);
}

void OutlineTreeModel::addRow(Note* note)
{
    QList<QStandardItem*> items;

    QString s{};

    QString title{};
    createTitleText(title, note);
    QStandardItem* noteItem = new QStandardItem{title};
    // TODO set role
    noteItem->setData(QVariant::fromValue(note));
    items.append(noteItem);

    s.clear();
    s += QString::number(note->getProgress());
    s += "%";
    items.append(new QStandardItem{s});

    items.append(new QStandardItem{QString::number(note->getReads())});

    items.append(new QStandardItem{QString::number(note->getRevision())});

    s = note->getModifiedPretty().c_str();
    items.append(new QStandardItem{s});

    appendRow(items);
}

void OutlineTreeModel::refresh(Note* note, QModelIndexList selection)
{
    int row = -1;

    // determine row number by note attached to the row - selection or iteration
    if(selection.size()) {
        // TODO use role
        if(item(selection[0].row())->data().value<Note*>() == note) {
            row = selection[0].row();
        }
    }
    // iterate
    if(row<0) {
        // IMPROVE UI note that has both Note and QStandardItem refs
        for(row = 0; row<rowCount(); row++) {
            // TODO use role
            if(item(row)->data().value<Note*>() == note) {
                break;
            }
        }
    }

    // refresh
    if(row>=0) {
        QString s{};
        createTitleText(s, note);
        // refresh content
        item(row,0)->setText(s);

        s.clear();
        s += QString::number(note->getProgress());
        s += "%";
        item(row,1)->setText(s);

        item(row,2)->setText(QString::number(note->getReads()));
        item(row,3)->setText(QString::number(note->getRevision()));
        s = note->getModifiedPretty().c_str();
        item(row,4)->setText(s);

        QModelIndex from = createIndex(row, 0, item(row,0));
        QModelIndex to = createIndex(row, 3, item(row,3));
        // notify
        emit dataChanged(from,to);
        return;
    }
}

void OutlineTreeModel::createTitleText(QString& html, Note* note)
{
    for(auto depth=0; depth<note->getDepth(); depth++) {
        html += QString::fromUtf8("&nbsp;&nbsp;&nbsp;&nbsp;");
    }
    html += QString::fromStdString(note->getTitle());

    tagsToHtml(note->getTags(), html);
    // IMPROVE make showing of type configurable
    noteTypeToHtml(note->getType(), html);
}

}
