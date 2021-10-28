/*
 kanban_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_KANBAN_PRESENTER_H
#define M8RUI_KANBAN_PRESENTER_H

#include "../../lib/src/model/kanban.h"

#include <QtWidgets>

#include "kanban_view.h"
#include "kanban_column_presenter.h"

namespace m8r {

class KanbanPresenter : public QObject
{
    Q_OBJECT

    KanbanView* view;

    // rendered from left to right with the growing index
    std::vector<KanbanColumnPresenter*> columns;

    Kanban* kanban;

public:
    explicit KanbanPresenter(KanbanView* view, OrlojPresenter* orloj);
    KanbanPresenter(const KanbanPresenter&) = delete;
    KanbanPresenter(const KanbanPresenter&&) = delete;
    KanbanPresenter& operator=(const KanbanPresenter&) = delete;
    KanbanPresenter& operator=(const KanbanPresenter&&) = delete;
    ~KanbanPresenter();

    void refresh(
        Kanban* kanban,
        const std::vector<Note*>& ons,
        const std::vector<Outline*>& os,
        const std::vector<Note*>& ns
    );

    KanbanView* getView() const { return this->view; }

    Kanban* getOrganizer() const { return this->kanban; }
};

}
#endif // M8RUI_KANBAN_PRESENTER_H
