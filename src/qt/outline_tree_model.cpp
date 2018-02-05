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

#define NO_INDEX -1

using namespace std;

namespace m8r {

OutlineTreeModel::OutlineTreeModel(QObject *parent)
    : QStandardItemModel(parent)
{
    setColumnCount(5);
    setRowCount(0);
}

void OutlineTreeModel::removeAllRows()
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

void OutlineTreeModel::addRow(Note* note)
{
    QList<QStandardItem*> items;
    QString s{};

    // title
    QString title{};
    createTitleText(title, note);
    QStandardItem* noteItem = new QStandardItem{title};
    // TODO declare custom role
    noteItem->setData(QVariant::fromValue(note), Qt::UserRole + 1);
    items.append(noteItem);
    // %
    if(note->getProgress()) {
        s.clear();
        s += QString::number(note->getProgress());
        s += "%";
        items.append(new QStandardItem{s});
    } else {
        items.append(new QStandardItem{""});
    }
    // rd/wr
    items.append(new QStandardItem{QString::number(note->getReads())});
    items.append(new QStandardItem{QString::number(note->getRevision())});
    // pretty
    s = note->getModifiedPretty().c_str();
    items.append(new QStandardItem{s});

    appendRow(items);
}

int OutlineTreeModel::getRowByNote(const Note* note)
{
    for(int row = 0; row<rowCount(); row++) {
        if(item(row)->data().value<Note*>() == note) {
            return row;
        }
    }
    return NO_INDEX;
}

void OutlineTreeModel::refresh(Note* note, int row, bool set)
{
    if(row > NO_INDEX) {
        if(set) {
            // TODO declare custom role
            item(row,0)->setData(QVariant::fromValue(note), Qt::UserRole + 1);
        }

        QString s{};
        createTitleText(s, note);
        // refresh content
        item(row,0)->setText(s);

        if(note->getProgress()) {
            s.clear();
            s += QString::number(note->getProgress());
            s += "%";
            item(row,1)->setText(s);
        } else {
            item(row,1)->setText("");
        }

        item(row,2)->setText(QString::number(note->getReads()));
        item(row,3)->setText(QString::number(note->getRevision()));
        s = note->getModifiedPretty().c_str();
        item(row,4)->setText(s);

        QModelIndex from = createIndex(row, 0, item(row,0));
        QModelIndex to = createIndex(row, 3, item(row,3));

        // notify widget about changes
        emit dataChanged(from,to);
    }
}


void OutlineTreeModel::refresh(Note* note, QModelIndexList selection)
{
    int row = NO_INDEX;

    // determine row number by note attached to the row - selection or iteration
    if(selection.size()) {
        // TODO declare custom role
        if(item(selection[0].row())->data(Qt::UserRole + 1).value<Note*>() == note) {
            row = selection[0].row();
        }
    }
    if(row <= NO_INDEX) {
        // IMPROVE UI note that has both Note and QStandardItem refs
        row = getRowByNote(note);
    }

    refresh(note, row);
}

}
