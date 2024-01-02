/*
 organizer_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_ORGANIZER_PRESENTER_H
#define M8RUI_ORGANIZER_PRESENTER_H

#include <vector>

#include <QtWidgets>

#include "organizer_view.h"
#include "organizer_quadrant_presenter.h"

namespace m8r {

class OrganizerQuadrantPresenter;
class OrlojPresenter;

class OrganizerPresenter : public QObject
{
    Q_OBJECT

    static constexpr const auto TITLE_DO_FIRST = "Do First (Urgent and Important)";
    static constexpr const auto TITLE_DO_SOON = "Do Soon (Urgent)";
    static constexpr const auto TITLE_PLAN_DEDICATED_TIME = "Plan Dedicated Time (Important)";
    static constexpr const auto TITLE_DO_SOMETIMES= "Do Soon (Urgent)";

    OrganizerView* view;

    OrganizerQuadrantPresenter* doFirstPresenter;
    OrganizerQuadrantPresenter* doSoonPresenter;
    OrganizerQuadrantPresenter* doSometimePresenter;
    OrganizerQuadrantPresenter* planDedicatedTimePresenter;

    std::vector<OrganizerQuadrantPresenter*> orderedQuadrants;

    Organizer* organizer;

    OrlojPresenter* orloj;

public:
    explicit OrganizerPresenter(OrganizerView* view, OrlojPresenter* orloj);
    OrganizerPresenter(const OrganizerPresenter&) = delete;
    OrganizerPresenter(const OrganizerPresenter&&) = delete;
    OrganizerPresenter& operator=(const OrganizerPresenter&) = delete;
    OrganizerPresenter& operator=(const OrganizerPresenter&&) = delete;
    ~OrganizerPresenter();

    OrganizerView* getView() const { return this->view; }
    Organizer* getOrganizer() const { return this->organizer; }

    std::vector<const Tag*> getTagsForQuadrant(int columnNumber);

    void refresh(
        Organizer* organizer,
        const std::vector<Note*>& ons,
        const std::vector<Outline*>& os,
        const std::vector<Note*>& ns,
        bool setFocus = true
    );

    void focusAndSelectPreviouslySelectedRow(OrganizerQuadrantView* view);
    void focusToNextVisibleQuadrant();
    void focusToPreviousVisibleQuadrant();

    /**
     * @brief Move Note to the next visible Eisenhower Matrix quadrant.
     *
     * Caller is responsible for refreshing column views.
     *
     * @param n     Note to be moved.
     * @return `true` if Note was moved, `false` otherwise.
     */
    OrganizerQuadrantPresenter* moveToNextVisibleQuadrant(Note* n);
    /**
     * @brief Move Note to the previous visible Eisenhower Matrix quadrant.
     *
     * Caller is responsible for refreshing column views.
     *
     * @param n     Note to be moved.
     * @return `true` if Note was moved, `false` otherwise.
     */
    OrganizerQuadrantPresenter* moveToPreviousVisibleQuadrant(Note* n);

private:
    OrganizerQuadrantPresenter* moveToVisibleQuadrant(Note* n, int nextPrevious);

};

}
#endif // M8RUI_ORGANIZER_PRESENTER_H
