/*
 recent_notes_table_model.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "recent_notes_table_model.h"

namespace m8r {

using namespace std;

RecentNotesTableModel::RecentNotesTableModel(QObject* parent, HtmlOutlineRepresentation* htmlRepresentation)
    : QStandardItemModel(parent), htmlRepresentation(htmlRepresentation)
{
    setColumnCount(5);
    setRowCount(0);
}

RecentNotesTableModel::~RecentNotesTableModel()
{
}

void RecentNotesTableModel::removeAllRows()
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader
        << tr("Recent Notes")
        << tr("Notebook")
        << tr("Rs")
        << tr("Ws")
        << tr("Read")
        << tr("Modified");
    // IMPROVE set tooltips: items w/ tooltips instead of just strings
    setHorizontalHeaderLabels(tableHeader);
}

void RecentNotesTableModel::addRow(const Note* n)
{
    QList<QStandardItem*> items;
    QStandardItem* item;

    string html{}, tooltip{};
    html.reserve(500);
    tooltip.reserve(500);

    if(n->getName().size()) {
        tooltip = n->getName();
        html = tooltip;
    } else {
        tooltip = n->getMangledName();
        // IMPROVE parse out file name
        string dir{};
        pathToDirectoryAndFile(tooltip, dir, html);
    }
    htmlRepresentation->tagsToHtml(n->getTags(), html);
    // IMPROVE make showing of type  configurable
    htmlRepresentation->noteTypeToHtml(n->getType(), html);

    // item
    item = new QStandardItem(QString::fromStdString(html));
    item->setToolTip(QString::fromStdString(tooltip));
    // TODO under which ROLE this is > I should declare CUSTOM role (user+1 as constant)
    item->setData(QVariant::fromValue(n));
    items += item;

    // IMPROVE refactor to methods
    QString s;

    s.clear();
    s += n->getOutline()->getName().c_str();
    items += new QStandardItem(s);

    item = new QStandardItem();
    item->setData(QVariant(n->getReads()), Qt::DisplayRole);
    items += item;

    item = new QStandardItem();
    item->setData(QVariant(n->getRevision()), Qt::DisplayRole);
    items += item;

    s.clear();
    s += n->getReadPretty().c_str();
    items += new QStandardItem(s);

    s.clear();
    s += n->getModifiedPretty().c_str();
    items += new QStandardItem(s);

    appendRow(items);
}

} // m8r namespace
