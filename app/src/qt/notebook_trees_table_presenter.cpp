/*
 notebook_trees_table_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "notebook_trees_table_presenter.h"

namespace m8r {

using namespace std;

NotebookTreesTablePresenter::NotebookTreesTablePresenter(
    NotebookTreesTableView* view, HtmlOutlineRepresentation* htmlRepresentation
) {
    this->view = view;
    this->model = new NotebookTreesTableModel(this, htmlRepresentation);
    this->view->setModel(this->model);

    HtmlDelegate* delegate = new HtmlDelegate();
    this->view->setItemDelegate(delegate);
}

NotebookTreesTablePresenter::~NotebookTreesTablePresenter()
{
}

void NotebookTreesTablePresenter::refresh(const vector<NotebookTree*>& notebookTrees)
{
    vector<NotebookTree*> sortedNotebookTrees{notebookTrees};
    std::sort(
        sortedNotebookTrees.begin(),
        sortedNotebookTrees.end(),
        [](NotebookTree* a, NotebookTree* b){ return a->getModified() > b->getModified(); }
    );

#ifdef DO_MF_DEBUG
    MF_DEBUG("Refreshing notebook trees table:");
    for(NotebookTree* t:sortedNotebookTrees) {
        MF_DEBUG("  [" << t->getModified() << "]  " << t->getName() << endl);
    }
#endif

    model->removeAllRows();
    if(sortedNotebookTrees.size()) {
        for(auto& t:sortedNotebookTrees) {
            model->addRow(t);
        }
    }

    this->view->setCurrentIndex(this->model->index(0, 0));
    this->view->setFocus();
}

int NotebookTreesTablePresenter::getCurrentRow() const
{
    QModelIndexList indexes = view->selectionModel()->selection().indexes();
    for(int i=0; i<indexes.count(); i++) {
        return indexes.at(i).row();
    }
    return NO_ROW;
}

} // m8r namespace
