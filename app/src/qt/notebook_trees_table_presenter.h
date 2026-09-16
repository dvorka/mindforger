/*
 notebook_trees_table_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_NOTEBOOK_TREES_TABLE_PRESENTER_H
#define M8RUI_NOTEBOOK_TREES_TABLE_PRESENTER_H

#include <QtWidgets>

#include "../../lib/src/representations/html/html_outline_representation.h"
#include "../../lib/src/debug.h"

#include "notebook_trees_table_view.h"
#include "notebook_trees_table_model.h"
#include "html_delegate.h"

namespace m8r {

class NotebookTreesTablePresenter : public QObject
{
    Q_OBJECT

    NotebookTreesTableView* view;
    NotebookTreesTableModel* model;

public:
    static const int NO_ROW = -1;

public:
    explicit NotebookTreesTablePresenter(
        NotebookTreesTableView* view, HtmlOutlineRepresentation* htmlRepresentation
    );
    NotebookTreesTablePresenter(const NotebookTreesTablePresenter&) = delete;
    NotebookTreesTablePresenter(const NotebookTreesTablePresenter&&) = delete;
    NotebookTreesTablePresenter &operator=(const NotebookTreesTablePresenter&) = delete;
    NotebookTreesTablePresenter &operator=(const NotebookTreesTablePresenter&&) = delete;
    ~NotebookTreesTablePresenter();

    NotebookTreesTableModel* getModel() const { return model; }
    NotebookTreesTableView* getView() const { return view; }

    void refresh(const std::vector<NotebookTree*>& notebookTrees);
    int getCurrentRow() const;
};

}
#endif // M8RUI_NOTEBOOK_TREES_TABLE_PRESENTER_H
