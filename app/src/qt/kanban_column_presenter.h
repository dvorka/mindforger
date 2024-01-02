/*
 kanban_column_presenter.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_KANBAN_COLUMN_PRESENTER_H
#define M8RUI_KANBAN_COLUMN_PRESENTER_H

#include <QtWidgets>

#include "orloj_presenter.h"
#include "kanban_column_model.h"
#include "kanban_column_view.h"

namespace m8r {

class KanbanColumnPresenter : public QObject
{
    Q_OBJECT

private:
    static const int NO_ROW = -1;

    KanbanColumnView* view;
    KanbanColumnModel* model;

    OrlojPresenter* orloj;

public:
    explicit KanbanColumnPresenter(
        KanbanColumnView* view,
        OrlojPresenter* orloj,
        QString title
    );
    KanbanColumnPresenter(const KanbanColumnPresenter&) = delete;
    KanbanColumnPresenter(const KanbanColumnPresenter&&) = delete;
    KanbanColumnPresenter& operator=(const KanbanColumnPresenter&) = delete;
    KanbanColumnPresenter& operator=(const KanbanColumnPresenter&&) = delete;
    ~KanbanColumnPresenter();

    void setTitle(QString& title) { model->setTitle(title); }

    int getCurrentRow() const;
    Note* getSelectedNote();
    void focusAndSelectRow();
    void refresh(
        const std::vector<Note*>& os,
        bool urgency,
        bool importance,
        bool showOutline
    );
    OrganizerQuadrantView* getView() const { return view; }


public slots:
    void slotShowSelectedNote();
    void slotShowNote(const QItemSelection& selected, const QItemSelection& deselected);
    void slotHeaderClicked(int section);

    void slotOrganizerEdit();

    void slotFocusToNextVisibleQuadrant();
    void slotFocusToPreviousVisibleQuadrant();

    void slotMoveNoteToNextQuadrant();
    void slotMoveNoteToPreviousQuadrant();
};

}
#endif // M8RUI_KANBAN_COLUMN_PRESENTER_H
