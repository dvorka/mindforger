/*
 ner_leaderboard_model.cpp     MindForger thinking notebook

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
#include "ner_leaderboard_model.h"

namespace m8r {

using namespace std;

NerLeaderboardModel::NerLeaderboardModel(QWidget* parent)
    : QStandardItemModel(parent)
{
    setColumnCount(3);
    setRowCount(0);
}

NerLeaderboardModel::~NerLeaderboardModel()
{
}

void NerLeaderboardModel::removeAllRows()
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader
        << tr("Name")
        << tr("Type")
        << tr("Score");

    // IMPROVE set tooltips: items w/ tooltips instead of just strings
    setHorizontalHeaderLabels(tableHeader);
}

void NerLeaderboardModel::addRow(string& entityName, NerNamedEntityType entityType, float score)
{
    QList<QStandardItem*> items;
    QStandardItem* item;

    QString html{};
    html += QString::fromStdString(entityName);

    // item
    item = new QStandardItem(html);
    item->setToolTip(html);
    items += item;

    html.clear();
    switch(entityType) {
    case NerNamedEntityType::PERSON:
        html += tr("person");
        break;
    case NerNamedEntityType::LOCATION:
        html += tr("location");
        break;
    case NerNamedEntityType::ORGANIZATION:
        html += tr("organization");
        break;
    case NerNamedEntityType::MISC:
        html += tr("misc");
        break;
    }

    item = new QStandardItem(html);
    item->setToolTip(html);
    items += item;

    html.clear();
    if(score>0.29) {
        html += "<span style='color: #00";
        if(score>0.69) {
            html += "CC";
        } else if(score>0.49) {
            html += "AA";
        } else if(score>0.39) {
            html += "66";
        } else if(score>0.29) {
            html += "44";
        }
        html += "00'>";
    }
    score = ROUND_FLOAT(score, 1000);
    html += QString::number(score);
    if(score>0.29) {
        html += "</span>";
    }
    item = new QStandardItem(html);
    item->setData(QVariant::fromValue(score));
    items += item;

    appendRow(items);
}

} // m8r namespace
