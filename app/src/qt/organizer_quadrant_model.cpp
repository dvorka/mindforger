/*
 organizer_quadrant_model.cpp     MindForger thinking notebook

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
#include "organizer_quadrant_model.h"

namespace m8r {

using namespace std;

OrganizerQuadrantModel::OrganizerQuadrantModel(QString& title, QObject* parent, HtmlOutlineRepresentation* htmlRepresentation)
    : QStandardItemModel(parent), htmlRepresentation(htmlRepresentation)
{
    setColumnCount(1);
    setRowCount(0);

    this->title = title;
}

OrganizerQuadrantModel::~OrganizerQuadrantModel()
{
}

void OrganizerQuadrantModel::removeAllRows()
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader << title;
    // IMPROVE set tooltips: items w/ tooltips instead of just strings
    setHorizontalHeaderLabels(tableHeader);
}

void OrganizerQuadrantModel::addRow(Outline* outline, bool urgency, bool importance)
{
    QList<QStandardItem*> items;
    QStandardItem* item;

    // IMPROVE consider moving this to HTML representation
    string h{outline->getName().c_str()};

    htmlRepresentation->tagsToHtml(outline->getTags(), h);

    h += "<span style='color: ";
    h += Color::DARK_GRAY().asHtml();
    h += ";'>";

    QString html{};
    html += QString::fromStdString(h);

    if(urgency) {
        if(outline->getUrgency()) {
            html += " ";
            for(int i=1; i<=outline->getUrgency(); i++) {
                html += QChar(0x25D5); // timer clock
                //html += QChar(0x29D7); // sand clocks - not in fonts on macOS and Fedora
            }
        }
    }
    if(importance) {
        if(outline->getImportance()) {
            html += " ";
            for(int i=1; i<=outline->getImportance(); i++) {
                html += QChar(9733);
            }
        }
    }

    html += " </span>";

    // item
    item = new QStandardItem(html);
    item->setToolTip(html);
    // TODO under which ROLE this is > I should declare CUSTOM role (user+1 as constant)
    item->setData(QVariant::fromValue(outline));
    items += item;

    appendRow(items);
}

} // m8r namespace
