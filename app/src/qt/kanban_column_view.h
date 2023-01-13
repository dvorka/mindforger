/*
 kanban_column_view.h     MindForger thinking notebook

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
#ifndef M8RUI_KANBAN_COLUMN_VIEW_H
#define M8RUI_KANBAN_COLUMN_VIEW_H

#include "../../../lib/src/debug.h"

#include <QtWidgets>

# include "organizer_quadrant_view.h"

namespace m8r {

class KanbanColumnView : public OrganizerQuadrantView
{
    Q_OBJECT

public:
    explicit KanbanColumnView(QWidget* parent);
    KanbanColumnView(const KanbanColumnView&) = delete;
    KanbanColumnView(const KanbanColumnView&&) = delete;
    KanbanColumnView& operator=(const KanbanColumnView&) = delete;
    KanbanColumnView& operator=(const KanbanColumnView&&) = delete;
    ~KanbanColumnView();
};

}

#endif // M8RUI_KANBAN_COLUMN_VIEW_H
