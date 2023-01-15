/*
 organizers_table_model.h     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_ORGANIZERS_TABLE_MODEL_H
#define M8RUI_ORGANIZERS_TABLE_MODEL_H

#include <QtWidgets>

#include "model_meta_definitions.h"
#include "../../lib/src/representations/html/html_outline_representation.h"

namespace m8r {

class OrganizersTableModel : public QStandardItemModel
{
    Q_OBJECT

public:
    HtmlOutlineRepresentation* htmlRepresentation;

    explicit OrganizersTableModel(
        QObject* parent, HtmlOutlineRepresentation* htmlRepresentation
    );
    OrganizersTableModel(const OrganizersTableModel&) = delete;
    OrganizersTableModel(const OrganizersTableModel&&) = delete;
    OrganizersTableModel &operator=(const OrganizersTableModel&) = delete;
    OrganizersTableModel &operator=(const OrganizersTableModel&&) = delete;
    ~OrganizersTableModel();

    void removeAllRows();
    void addRow(Organizer* organizer);
};

}
#endif // M8RUI_ORGANIZERS_TABLE_MODEL_H
