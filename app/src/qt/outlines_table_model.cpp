/*
 notes_table_model.cpp     MindForger thinking notebook

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
#include "outlines_table_model.h"

namespace m8r {

using namespace std;

OutlinesTableModel::OutlinesTableModel(QObject* parent, HtmlOutlineRepresentation* htmlRepresentation)
    : QStandardItemModel(parent), htmlRepresentation(htmlRepresentation)
{
    setColumnCount(5);
    setRowCount(0);
}

void OutlinesTableModel::removeAllRows()
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader
        << tr("Notebooks")
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

    string html{}, tooltip{};
    html.reserve(500);
    tooltip.reserve(500);

    if(outline->getName().size()) {
        tooltip = outline->getName();
        html = tooltip;
    } else {
        tooltip = outline->getKey();
        // IMPROVE parse out file name
        string dir{};
        pathToDirectoryAndFile(tooltip, dir, html);
    }
    htmlRepresentation->tagsToHtml(outline->getTags(), html);
    // IMPROVE make showing of type  configurable
    htmlRepresentation->outlineTypeToHtml(outline->getType(), html);
    // item
    item = new QStandardItem(QString::fromStdString(html));
    item->setToolTip(QString::fromStdString(tooltip));
    // TODO under which ROLE this is > I should declare CUSTOM role (user+1 as constant)
    item->setData(QVariant::fromValue(outline));
    items += item;

    // IMPROVE refactor to methods
    QString s;

    s.clear();
    // stupid and ugly: correct sorting is ensured by making appropriate HTML (alpha sort), don't know how to sort using data role
    s += "<div title='";
    s += outline->getImportance();
    s += "'>";
    if(outline->getImportance() > 0) {
        for(int i=0; i<=4; i++) {
            if(outline->getImportance()>i) {
                s += QChar(U_CODE_IMPORTANCE_ON);
            } else {
                s += QChar(U_CODE_IMPORTANCE_OFF);
            }
        }
    }
    s += "</div>";
    item = new QStandardItem(s);
    item->setData(QVariant::fromValue((int8_t)(outline->getImportance())), Qt::UserRole);
    items += item;

    s.clear();
    if(outline->getUrgency()>0) {
        for(int i=0; i<=4; i++) {
            if(outline->getUrgency()>i) {
                s += QChar(U_CODE_URGENCY_ON);
            } else {
                s += QChar(U_CODE_URGENCY_OFF);
            }
        }
    }
    item = new QStandardItem(s);
    item->setData(QVariant::fromValue((int8_t)(outline->getUrgency())), Qt::UserRole);
    items += item;

    s.clear();
    if(outline->getProgress() > 0) {
        s += QString::number(outline->getProgress());
        s += "%";
    }
    items += new QStandardItem(s);

    item = new QStandardItem();
    item->setData(QVariant::fromValue((unsigned)(outline->getNotesCount())), Qt::DisplayRole);
    items += item;

    item = new QStandardItem();
    item->setData(QVariant(outline->getReads()), Qt::DisplayRole);
    items += item;

    item = new QStandardItem();
    item->setData(QVariant(outline->getRevision()), Qt::DisplayRole);
    items += item;

    s.clear();
    s += outline->getModifiedPretty().c_str();
    items += new QStandardItem(s);

    appendRow(items);
}

} // m8r namespace
