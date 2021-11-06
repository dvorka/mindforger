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

#include "../../lib/src/gear/math_utils.h"
#include "../../lib/src/model/kanban.h"
#include "../../lib/src/mind/ontology/ontology.h"

#include <QtWidgets>

#include "kanban_view.h"
#include "kanban_column_presenter.h"

namespace m8r {

class KanbanColumnPresenter;

/**
 * @brief Kanban view presenter.
 *
 * Code structure (Model/View/Presenter + signals/slots):
 *
 * - KanbanPresenter
 *   -> Kanban (lib)
 *   -> KanbanView
 *      -> KanbanColumnView
 *   -> KanbanColumnPresenter[]
 *      ->>> SLOTS >>> MainWindowPresenter (app wide orchestration)
 *      -> KanbanColumnView
 *         <<<- SIGNALS
 *      -> KanbanColumnModel
 *
 * Remarks:
 *
 * - View sends signals to Model
 * - Kanban is specific as it does not need model
 * - Kanban is specific as it inheriths column view from quadrant view
 *
 */
class KanbanPresenter : public QObject
{
    Q_OBJECT

    KanbanView* view;

    // rendered from left to right with the growing index
    std::vector<KanbanColumnPresenter*> columns;

    Kanban* kanban;

    OrlojPresenter* orloj;

public:
    explicit KanbanPresenter(KanbanView* view, OrlojPresenter* orloj);
    KanbanPresenter(const KanbanPresenter&) = delete;
    KanbanPresenter(const KanbanPresenter&&) = delete;
    KanbanPresenter& operator=(const KanbanPresenter&) = delete;
    KanbanPresenter& operator=(const KanbanPresenter&&) = delete;
    ~KanbanPresenter();

    KanbanView* getView() const { return this->view; }
    Kanban* getKanban() const { return this->kanban; }

    std::vector<const Tag*> getTagsForColumn(int columnNumber);

    void refresh(
        Kanban* kanban,
        const std::vector<Note*>& ons,
        const std::vector<Outline*>& os,
        const std::vector<Note*>& ns,
        bool setFocus = true
    );

    void getVisibleColumns(std::vector<KanbanColumnPresenter*>& visible, std::vector<int>& offsets);
    KanbanColumnPresenter* getNextVisibleColumn();
    KanbanColumnPresenter* getPreviousVisibleColumn();

    /**
     * @brief Give focus to next visible column.
     *
     * @returns presenter with focus or `nullptr` if no focus given.
     */
    KanbanColumnPresenter* focusToNextVisibleColumn();
    /**
     * @brief Give focus to previous visible column.
     *
     * @returns presenter with focus or `nullptr` if no focus given.
     */
    KanbanColumnPresenter* focusToPreviousVisibleColumn();

    /**
     * @brief Move Note to the next visible Kanban column.
     *
     * Caller is responsible for refreshing column views.
     *
     * @param n     Note to be moved.
     * @return `true` if Note was moved, `false` otherwise.
     */
    KanbanColumnPresenter* moveToNextVisibleColumn(Note* n);
    /**
     * @brief Move Note to the previous visible Kanban column.
     *
     * Caller is responsible for refreshing column views.
     *
     * @param n     Note to be moved.
     * @return `true` if Note was moved, `false` otherwise.
     */
    KanbanColumnPresenter* moveToPreviousVisibleColumn(Note* n);

private:
    KanbanColumnPresenter* moveToVisibleColumn(Note* n, int nextPrevious);

};

}
#endif // M8RUI_KANBAN_PRESENTER_H
