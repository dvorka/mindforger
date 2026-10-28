/*
 recent_notes_model.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "recent_notes_model.h"

namespace m8r {

using namespace std;

RecentNotesModel::RecentNotesModel(QObject* parent, HtmlOutlineRepresentation* htmlRepresentation)
    : QStandardItemModel(parent), htmlRepresentation(htmlRepresentation)
{
    setColumnCount(COLUMN_COUNT);
    setRowCount(0);
}

RecentNotesModel::~RecentNotesModel()
{
}

void RecentNotesModel::removeAllRows()
{
    QStandardItemModel::clear();

    QStringList tableHeader;
    tableHeader
        << tr("Recent Notes")
        << tr("Rs")
        << tr("Ws")
        << tr("Read")
        << tr("Modified");
    // IMPROVE set tooltips: items w/ tooltips instead of just strings
    setHorizontalHeaderLabels(tableHeader);
}

QStandardItem* RecentNotesModel::createNameItem(
    const string& name,
    const string& fallbackName,
    const vector<const Tag*>* tags
) {
    string html{}, tooltip{};
    html.reserve(500);
    tooltip.reserve(500);

    if(name.size()) {
        tooltip = name;
        html = tooltip;
    } else {
        tooltip = fallbackName;
        string dir{};
        pathToDirectoryAndFile(tooltip, dir, html);
    }
    htmlRepresentation->tagsToHtml(tags, html);

    QStandardItem* item = new QStandardItem(QString::fromStdString(html));
    item->setToolTip(QString::fromStdString(tooltip));
    return item;
}

QStandardItem* RecentNotesModel::createNumberItem(u_int32_t number)
{
    QStandardItem* item = new QStandardItem();
    item->setData(QVariant(number), Qt::DisplayRole);
    return item;
}

void RecentNotesModel::addGroup(const RecentNotes::Group& group)
{
    Outline* o = group.outline;

    // O row
    QList<QStandardItem*> outlineItems;
    QStandardItem* outlineItem = createNameItem(o->getName(), o->getKey(), o->getTags());
    outlineItem->setText("<b>" + outlineItem->text() + "</b>");
    outlineItem->setData(QVariant::fromValue(o), ROLE_OUTLINE);
    outlineItems += outlineItem;
    outlineItems += createNumberItem(o->getReads());
    outlineItems += createNumberItem(o->getRevision());
    outlineItems += new QStandardItem();
    outlineItems += new QStandardItem(QString::fromStdString(o->getModifiedPretty()));

    // N rows
    for(Note* n:group.notes) {
        QList<QStandardItem*> noteItems;
        QStandardItem* noteItem = createNameItem(n->getName(), n->getMangledName(), n->getTags());
        // IMPROVE make showing of type configurable
        string typeHtml{};
        htmlRepresentation->noteTypeToHtml(n->getType(), typeHtml);
        noteItem->setText(noteItem->text() + QString::fromStdString(typeHtml));
        noteItem->setData(QVariant::fromValue(o), ROLE_OUTLINE);
        noteItem->setData(QVariant::fromValue(n), ROLE_NOTE);
        noteItems += noteItem;
        noteItems += createNumberItem(n->getReads());
        noteItems += createNumberItem(n->getRevision());
        noteItems += new QStandardItem(QString::fromStdString(n->getReadPretty()));
        noteItems += new QStandardItem(QString::fromStdString(n->getModifiedPretty()));

        outlineItem->appendRow(noteItems);
    }

    appendRow(outlineItems);
}

Outline* RecentNotesModel::getOutline(const QModelIndex& index)
{
    if(index.isValid()) {
        // O/N is stored in the name column of the row
        return index.sibling(index.row(), COLUMN_NAME).data(ROLE_OUTLINE).value<Outline*>();
    }
    return nullptr;
}

Note* RecentNotesModel::getNote(const QModelIndex& index)
{
    if(index.isValid()) {
        return index.sibling(index.row(), COLUMN_NAME).data(ROLE_NOTE).value<Note*>();
    }
    return nullptr;
}

} // m8r namespace
