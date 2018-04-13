/*
 outline_header_view_presenter.h     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_OUTLINE_HEADER_VIEW_PRESENTER_H
#define M8RUI_OUTLINE_HEADER_VIEW_PRESENTER_H

#include "../../lib/src/model/outline.h"
#include "../../lib/src/representations/html/html_outline_representation.h"

#include <QtWidgets>

#include "orloj_presenter.h"
#include "outline_header_view.h"

namespace m8r {

class OrlojPresenter;

class OutlineHeaderViewPresenter : public QObject
{
    Q_OBJECT

private:
    // IMPROVE to model
    Outline* currentOutline;

    OutlineHeaderView* view;
    OrlojPresenter* orloj;
    HtmlOutlineRepresentation* htmlRepresentation;

public:
    explicit OutlineHeaderViewPresenter(OutlineHeaderView* view, OrlojPresenter* orloj);
    OutlineHeaderViewPresenter(const OutlineHeaderViewPresenter&) = delete;
    OutlineHeaderViewPresenter(const OutlineHeaderViewPresenter&&) = delete;
    OutlineHeaderViewPresenter &operator=(const OutlineHeaderViewPresenter&) = delete;
    OutlineHeaderViewPresenter &operator=(const OutlineHeaderViewPresenter&&) = delete;

    void refresh(Outline* outline);

    ~OutlineHeaderViewPresenter();

public slots:
    void slotEditOutlineHeader();
    void slotRefreshHeaderLeaderboardByValue(std::vector<std::pair<Note*,float>>* associations);
};

}
#endif // M8RUI_OUTLINE_HEADER_VIEW_PRESENTER_H
