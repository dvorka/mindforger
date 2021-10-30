/*
 organizer_quadrant_view.h     MindForger thinking notebook

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
#ifndef M8RUI_ORGANIZER_QUADRANT_VIEW_H
#define M8RUI_ORGANIZER_QUADRANT_VIEW_H

#include "../../../lib/src/debug.h"

#include <QtWidgets>

namespace m8r {

class OrganizerQuadrantView : public QTableView
{
    Q_OBJECT

public:
    enum ViewType {
        ORGANIZER,
        KANBAN
    };

private:
    ViewType viewType;

public:
    explicit OrganizerQuadrantView(QWidget* parent, ViewType viewType = ViewType::ORGANIZER);
    OrganizerQuadrantView(const OrganizerQuadrantView&) = delete;
    OrganizerQuadrantView(const OrganizerQuadrantView&&) = delete;
    OrganizerQuadrantView& operator=(const OrganizerQuadrantView&) = delete;
    OrganizerQuadrantView& operator=(const OrganizerQuadrantView&&) = delete;
    virtual ~OrganizerQuadrantView() {}

    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
#ifdef WIP_DRAG_N_DROP
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

    Qt::DropActions supportedDropActions() const
    {
        return Qt::MoveAction;
    }
    Qt::DropActions supportedDragActions() const
    {
        return Qt::MoveAction;
    }
#endif

signals:
    void signalShowSelectedKanbanNote();
    void signalShowSelectedNote();
};

}
#endif // M8RUI_ORGANIZER_QUADRANT_VIEW_H
