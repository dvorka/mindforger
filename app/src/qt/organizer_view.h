/*
 organizer_view.h     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_ORGANIZER_VIEW_H
#define M8RUI_ORGANIZER_VIEW_H

#include <QtWidgets>

#include "organizer_quadrant_view.h"

namespace m8r {

/**
 * @brief Eisenhower matrix aka organizer view.
 *
 * Do soon     | Do first
 * ------------+---------------------
 * Do sometime | Plan dedicated time
 */
class OrganizerView : public QSplitter
{
    Q_OBJECT

private:
    QSplitter* left;
    QSplitter* right;

    OrganizerQuadrantView* doFirst;
    OrganizerQuadrantView* doSoon;
    OrganizerQuadrantView* planDedicatedTime;
    OrganizerQuadrantView* doSometime;

public:
    explicit OrganizerView(QWidget* parent);
    OrganizerView(const OrganizerView&) = delete;
    OrganizerView(const OrganizerView&&) = delete;
    OrganizerView &operator=(const OrganizerView&) = delete;
    OrganizerView &operator=(const OrganizerView&&) = delete;
    ~OrganizerView();

    OrganizerQuadrantView* getDoFirst() { return doFirst; }
    OrganizerQuadrantView* getDoSoon() { return doSoon; }
    OrganizerQuadrantView* getDoSometime() { return doSometime; }
    OrganizerQuadrantView* getPlanDedicatedTime() { return planDedicatedTime; }
};

}
#endif // M8RUI_ORGANIZER_VIEW_H
