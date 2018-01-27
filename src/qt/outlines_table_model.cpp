/*
 notes_table_model.cpp     MindForger thinking notebook

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
#include "outlines_table_model.h"

namespace m8r {

OutlinesTableModel::OutlinesTableModel(QObject* parent)
    : QStandardItemModel(parent)
{
    setColumnCount(5);
    setRowCount(0);
}

void OutlinesTableModel::removeAllRows()
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader
        << tr("Outline")
        << tr("Importance")
        << tr("Urgency")
        << tr("Done")
        << tr("Ns")
        << tr("Rs")
        << tr("Ws")
        << tr("Modified");
    // IMPROVE set tooltips: items w/ tooltips instead of just strings
    setHorizontalHeaderLabels(tableHeader);
}

void OutlinesTableModel::addRow(Outline* outline)
{
    QList<QStandardItem*> items;
    QStandardItem* item;

    QString html, tooltip;
    if(outline->getTitle().size()) {
        tooltip = QString(outline->getTitle().c_str());
        html += tooltip;
    } else {
        tooltip = QString::fromStdString(outline->getKey());
        html = html.right(html.size()-html.lastIndexOf(QRegularExpression("/"))-1);
    }
    tagsToHtml(outline->getTags(), html);
    // IMPROVE make showing of type  configurable
    outlineTypeToHtml(outline->getType(), html);
    // item
    item = new QStandardItem(html);
    item->setToolTip(tooltip);
    // TODO under which ROLE this is > I should declare CUSTOM role (user+1 as constant)
    item->setData(QVariant::fromValue(outline));
    items.append(item);

    // IMPROVE refactor to methods
    QString s;

    s.clear();
    if(outline->getImportance() > 0) {
        for(int i=0; i<=4; i++) {
            if(outline->getImportance()>i) {
                s += QChar(9733);
            } else {
                s += QChar(9734);
            }
        }
    }
    items.append(new QStandardItem(s));

    s.clear();
    if(outline->getUrgency()>0) {
        for(int i=0; i<=4; i++) {
            if(outline->getUrgency()>i) {
                s += QChar(0x29D7);
            } else {
                s += QChar(0x29D6);
            }
        }
    }
    items.append(new QStandardItem(s));

    s.clear();
    if(outline->getProgress() > 0) {
        s += QString::number(outline->getProgress());
        s += "%";
    }
    items.append(new QStandardItem(s));

    item = new QStandardItem();
    item->setData(QVariant::fromValue((unsigned)(outline->getNotesCount())), Qt::DisplayRole);
    items.append(item);

    item = new QStandardItem();
    item->setData(QVariant(outline->getReads()), Qt::DisplayRole);
    items.append(item);

    item = new QStandardItem();
    item->setData(QVariant(outline->getRevision()), Qt::DisplayRole);
    items.append(item);

    s.clear();
    s += outline->getModifiedPretty().c_str();
    items.append(new QStandardItem(s));

    appendRow(items);
}

}
