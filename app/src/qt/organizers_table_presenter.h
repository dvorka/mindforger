/*
 organizers_table_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_ORGANIZERS_TABLE_PRESENTER_H
#define M8RUI_ORGANIZERS_TABLE_PRESENTER_H

#include <QtWidgets>

#include "../../lib/src/representations/html/html_outline_representation.h"
#include "../../lib/src/debug.h"

#include "organizers_table_view.h"
#include "organizers_table_model.h"
#include "html_delegate.h"

namespace m8r {

class OrganizersTablePresenter : public QObject
{
    Q_OBJECT

    OrganizersTableView* view;
    OrganizersTableModel* model;

public:
    static const int NO_ROW = -1;

public:
    explicit OrganizersTablePresenter(OrganizersTableView* view, HtmlOutlineRepresentation* htmlRepresentation);
    OrganizersTablePresenter(const OrganizersTablePresenter&) = delete;
    OrganizersTablePresenter(const OrganizersTablePresenter&&) = delete;
    OrganizersTablePresenter &operator=(const OrganizersTablePresenter&) = delete;
    OrganizersTablePresenter &operator=(const OrganizersTablePresenter&&) = delete;
    ~OrganizersTablePresenter();

    OrganizersTableModel* getModel() const { return model; }
    OrganizersTableView* getView() const { return view; }

    void refresh(const std::vector<Organizer*>& organizers);
    int getCurrentRow() const;
};

}
#endif // M8RUI_ORGANIZERS_TABLE_PRESENTER_H
