/*
 notebook_trees_table_model.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "notebook_trees_table_model.h"

namespace m8r {

using namespace std;

NotebookTreesTableModel::NotebookTreesTableModel(
    QObject* parent, HtmlOutlineRepresentation* htmlRepresentation
)
    : QStandardItemModel(parent), htmlRepresentation(htmlRepresentation)
{
    setColumnCount(1);
    setRowCount(0);
}

NotebookTreesTableModel::~NotebookTreesTableModel()
{
}

void NotebookTreesTableModel::removeAllRows()
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader
        << tr("Notebook Trees");
    setHorizontalHeaderLabels(tableHeader);
}

void NotebookTreesTableModel::addRow(NotebookTree* notebookTree)
{
    QList<QStandardItem*> items;
    QStandardItem* item;

    string html{}, tooltip{};
    html.reserve(500);
    tooltip.reserve(500);

    tooltip = notebookTree->getName();
    html = tooltip;

    item = new QStandardItem(QString::fromStdString(html));
    item->setToolTip(QString::fromStdString(tooltip));
    item->setData(QVariant::fromValue(notebookTree));
    items += item;

    appendRow(items);
}

} // m8r namespace
