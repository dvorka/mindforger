/*
 tags_table_model.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "tags_table_model.h"

namespace m8r {

using namespace std;

TagsTableModel::TagsTableModel(QObject* parent, HtmlOutlineRepresentation* htmlRepresentation)
    : QStandardItemModel(parent), htmlRepresentation(htmlRepresentation)
{
    setColumnCount(2);
    setRowCount(0);
}

TagsTableModel::~TagsTableModel()
{
}

void TagsTableModel::removeAllRows()
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader
        << tr("Tags")
        << tr("Ts");
    // IMPROVE set tooltips: items w/ tooltips instead of just strings
    setHorizontalHeaderLabels(tableHeader);
}

void TagsTableModel::addRow(const Tag* tag, int cardinality)
{
    QList<QStandardItem*> items;
    QStandardItem* item;

    // tag name
    string html{}, tooltip{};
    tooltip = tag->getName();
    vector<const Tag*> tags;
    tags.push_back(tag);
    htmlRepresentation->tagsToHtml(&tags, html);
    item = new QStandardItem(QString::fromStdString(html));
    item->setToolTip(QString::fromStdString(tooltip));
    // TODO under which ROLE this is > I should declare CUSTOM role (user+1 as constant)
    item->setData(QVariant::fromValue(tag));
    items += item;

    // cardinality
    item = new QStandardItem();
    item->setData(QVariant::fromValue((int)(cardinality)), Qt::DisplayRole);
    items += item;

    appendRow(items);
}

} // m8r namespace
