/*
 notes_table_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "notes_table_presenter.h"

using namespace std;

namespace m8r {

NotesTablePresenter::NotesTablePresenter(NotesTableView* view)
{
    this->view = view;
    this->model = new NotesTableModel();
    this->view->setModel(this->model);
}

void NotesTablePresenter::refresh(vector<Note*>* result)
{
    model->removeAllRows();
    for(Note* note:*result) {
        model->addRow(note);
    }

    delete result;
}

} // m8r namespace
