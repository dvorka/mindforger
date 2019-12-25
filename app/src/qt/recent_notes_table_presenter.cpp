/*
 recent_notes_table_presenter.cpp     MindForger thinking notebook

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
#include "recent_notes_table_presenter.h"

namespace m8r {

using namespace std;

RecentNotesTablePresenter::RecentNotesTablePresenter(RecentNotesTableView* view, HtmlOutlineRepresentation* htmlRepresentation)
{       
    this->view = view;
    this->model = new RecentNotesTableModel(this, htmlRepresentation);
    this->view->setModel(this->model);

    // ensure HTML cells rendering
    HtmlDelegate* delegate = new HtmlDelegate();
    // IMPROVE implement delegates by type e.g. timestamp an reuse them across views
    //this->view->setItemDelegateForColumn(delegate);
    this->view->setItemDelegate(delegate);
}

RecentNotesTablePresenter::~RecentNotesTablePresenter()
{
}

void RecentNotesTablePresenter::refresh(const vector<Note*>& notes)
{
    model->removeAllRows();
    if(notes.size()) {
        int uiLimit = Configuration::getInstance().getRecentNotesUiLimit();
        for(Note* n:notes) {
            if(uiLimit) uiLimit--; else break;
            model->addRow(n);            
        }
    }

    // order by read timestamp
    view->sortByColumn(4, Qt::SortOrder::DescendingOrder);
}

} // m8r namespace
