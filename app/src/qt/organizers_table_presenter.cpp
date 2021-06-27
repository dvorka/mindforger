/*
 organizers_table_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "organizers_table_presenter.h"

namespace m8r {

using namespace std;

OrganizersTablePresenter::OrganizersTablePresenter(OrganizersTableView* view, HtmlOutlineRepresentation* htmlRepresentation)
{
    this->view = view;
    this->model = new OrganizersTableModel(this, htmlRepresentation);
    this->view->setModel(this->model);

    // ensure HTML cells rendering
    HtmlDelegate* delegate = new HtmlDelegate();
    // IMPROVE implement delegates by type e.g. timestamp an reuse them across views
    //this->view->setItemDelegateForColumn(delegate);
    this->view->setItemDelegate(delegate);
}

OrganizersTablePresenter::~OrganizersTablePresenter()
{
}

void OrganizersTablePresenter::refresh(const map<const Tag*, int>& tags)
{
    model->removeAllRows();
    if(tags.size()) {
        for(const auto& t:tags) {
            model->addRow(t.first, t.second);
        }
    }

    view->sortByColumn(1, Qt::SortOrder::DescendingOrder);

    this->view->setCurrentIndex(this->model->index(0, 0));
    this->view->setFocus();
}

int OrganizersTablePresenter::getCurrentRow() const
{
    QModelIndexList indexes = view->selectionModel()->selection().indexes();
    for(int i=0; i<indexes.count(); i++) {
        return indexes.at(i).row();
    }
    return NO_ROW;
}

} // m8r namespace
