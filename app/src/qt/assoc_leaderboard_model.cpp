/*
 assoc_leaderboard_model.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "assoc_leaderboard_model.h"

namespace m8r {

AssocLeaderboardModel::AssocLeaderboardModel(QObject* parent)
    : QStandardItemModel(parent)
{
    setColumnCount(2);
    setRowCount(0);
}

AssocLeaderboardModel::~AssocLeaderboardModel()
{
}

void AssocLeaderboardModel::removeAllRows()
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader
        << tr("Associations")
        << tr("%");
    // IMPROVE set tooltips: items w/ tooltips instead of just strings
    setHorizontalHeaderLabels(tableHeader);
}

void AssocLeaderboardModel::addRow(Note* note, float associativity)
{
    QStringList tableHeader;
    tableHeader
        << (tr("Associations")+(title.size()?tr(" for '")+title+tr("'"):tr("")))
        << tr("%");
    // IMPROVE set tooltips: items w/ tooltips instead of just strings
    setHorizontalHeaderLabels(tableHeader);

    QList<QStandardItem*> items;
    QStandardItem* item;

    QString html;
    html = QString(note->getName().c_str());
    html += " (";
    html += QString::fromStdString(note->getOutline()->getName());
    html += ")";

    // item
    item = new QStandardItem(html);
    item->setToolTip(html);
    // TODO under which ROLE this is > I should declare CUSTOM role (user+1 as constant)
    item->setData(QVariant::fromValue(note));
    items += item;

    html.clear();
    if(associativity>0.29) {
        html += "<span style='color: #00";
        if(associativity>0.69) {
            html += "CC";
        } else if(associativity>0.49) {
            html += "AA";
        } else if(associativity>0.39) {
            html += "66";
        } else if(associativity>0.29) {
            html += "44";
        }
        html += "00'>";
    }
    html += QString::number(associativity*100.);
    html += "%";
    if(associativity>0.29) {
        html += "</span>";
    }
    items += new QStandardItem(html);

    appendRow(items);
}

} // m8r namespace
