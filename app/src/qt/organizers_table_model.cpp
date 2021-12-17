/*
 organizers_table_model.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "organizers_table_model.h"


namespace m8r {

using namespace std;

OrganizersTableModel::OrganizersTableModel(QObject* parent, HtmlOutlineRepresentation* htmlRepresentation)
    : QStandardItemModel(parent), htmlRepresentation(htmlRepresentation)
{
    setColumnCount(1);
    setRowCount(0);
}

OrganizersTableModel::~OrganizersTableModel()
{
}

void OrganizersTableModel::removeAllRows()
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader
        << tr("Organizers");
    // IMPROVE set tooltips: items w/ tooltips instead of just strings
    setHorizontalHeaderLabels(tableHeader);
}

void OrganizersTableModel::addRow(Organizer* organizer)
{
    QList<QStandardItem*> items;
    QStandardItem* item;

    // row as HTML
    string html{}, tooltip{};
    html.reserve(500);
    tooltip.reserve(500);

    if(organizer->getName().size()) {
        tooltip = organizer->getName();
        html = tooltip;
    } else {
        tooltip = "";
        // IMPROVE parse out file name
        string dir{};
        pathToDirectoryAndFile(tooltip, dir, html);
    }
    htmlRepresentation->organizerTypeToHtml(organizer, html);

    // item
    item = new QStandardItem(QString::fromStdString(html));
    item->setToolTip(QString::fromStdString(tooltip));
    // TODO under which ROLE this is > I should declare CUSTOM role (user+1 as constant)
    item->setData(QVariant::fromValue(organizer));
    items += item;

    appendRow(items);
}

} // m8r namespace
