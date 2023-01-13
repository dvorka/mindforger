/*
 kanban_view.h     MindForger thinking notebook

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
#ifndef M8RUI_KANBAN_VIEW_H
#define M8RUI_KANBAN_VIEW_H

#include <iostream>

#include <vector>

#include <QtWidgets>

#include "kanban_column_view.h"

namespace m8r {

/**
 * @brief Kanban view on tags.
 *
 * Kanban view with 2 to 4 columns and drag & drop allowing to move Os/Ns between columns.
 * New / edit organizer dialog shared with Eisenhower Matrix on tags. If tag is ".", then
 * the column is not shown in the view thus user can use from none up to four columns.
 *
 * ```
 * -------+-------+-------+-------
 *  tag-1 | tag-2 | tag-1 | tag-2
 * -------+-------+-------+-------
 * ```
 */
class KanbanView : public QSplitter
{
    Q_OBJECT

    // 2 to 4 columns (UX)
    int columnsCount;

    // rendered from left to right with the growing index
    std::vector<KanbanColumnView*> columns;

public:
    explicit KanbanView(int columnsCount, QWidget* parent);
    KanbanView(const KanbanView&) = delete;
    KanbanView(const KanbanView&&) = delete;
    KanbanView& operator=(const KanbanView&) = delete;
    KanbanView& operator=(const KanbanView&&) = delete;
    ~KanbanView();

    KanbanColumnView* getColumn(int index) {
        if(index >= 0 && index < this->columnsCount) {
            return this->columns[index];
        }

        std::string errorMessage{
            "Kanban view column index "
            + std::to_string(index)
            + " is out of range: "
            + std::to_string(this->columnsCount)
        };
        std::cerr << errorMessage << std::endl;

        throw errorMessage;
    }
};

}
#endif // M8RUI_KANBAN_VIEW_H
