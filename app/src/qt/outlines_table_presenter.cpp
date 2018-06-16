/*
 outlines_table_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "outlines_table_presenter.h"

using namespace std;

namespace m8r {

OutlinesTablePresenter::OutlinesTablePresenter(OutlinesTableView* view, HtmlOutlineRepresentation* htmlRepresentation)
{
    this->view = view;
    this->model = new OutlinesTableModel(this, htmlRepresentation);
    this->view->setModel(this->model);

    // ensure HTML cells rendering
    HtmlDelegate* delegate = new HtmlDelegate();
    // IMPROVE implement delegates by type e.g. timestamp an reuse them across views
    //this->view->setItemDelegateForColumn(delegate);
    this->view->setItemDelegate(delegate);
}

void OutlinesTablePresenter::refresh(const vector<Outline*>& outlines)
{
    model->removeAllRows();
    if(outlines.size()) {
        for(Outline* outline:outlines) {
            model->addRow(outline);
        }

        // IMPROVE create hidden column w/ long time used for sorting
        view->sortByColumn(7, Qt::SortOrder::DescendingOrder);
    }
}

} // m8r namespace
