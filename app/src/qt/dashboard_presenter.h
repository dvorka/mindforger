/*
 dashboard_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_DASHBOARD_PRESENTER_H
#define M8RUI_DASHBOARD_PRESENTER_H

#include <vector>

#include <QtWidgets>

#include "gear/qutils.h"
#include "orloj_presenter.h"
#include "dashboard_view.h"
#include "organizer_quadrant_presenter.h"
#include "recent_notes_table_presenter.h"
#include "navigator_presenter.h"
#include "tags_table_presenter.h"
#include "outlines_table_view.h"

namespace m8r {

class OrganizerQuadrantPresenter;
class OrlojPresenter;
class OutlinesTablePresenter;

class DashboardPresenter : public QObject
{
    Q_OBJECT

private:
    DashboardView* view;

    Configuration& config;

    OrganizerQuadrantPresenter* doFirstDashboardletPresenter;
    OrganizerQuadrantPresenter* doSoonDashboardletPresenter;
    RecentNotesTablePresenter* recentDashboardletPresenter;
    NavigatorPresenter* navigatorDashboardletPresenter;
    TagsTablePresenter* tagsDashboardletPresenter;
    OutlinesTablePresenter* outlinesDashboardletPresenter;

public:
    explicit DashboardPresenter(DashboardView* view, OrlojPresenter* orloj);
    DashboardPresenter(const DashboardPresenter&) = delete;
    DashboardPresenter(const DashboardPresenter&&) = delete;
    DashboardPresenter &operator=(const DashboardPresenter&) = delete;
    DashboardPresenter &operator=(const DashboardPresenter&&) = delete;
    ~DashboardPresenter();

    DashboardView* getView() { return view; }

    void refresh(
        const std::vector<Outline*>& os,
        const std::vector<Note*>& ns,
        const std::map<const Tag*,int>& ts,
        int bytes,
        MindStatistics* stats
    );

    RecentNotesTablePresenter* getRecentNotesPresenter() { return recentDashboardletPresenter; }
    TagsTablePresenter* getTagsPresenter() { return tagsDashboardletPresenter; }
    OutlinesTablePresenter* getOutlinesPresenter() { return outlinesDashboardletPresenter; }
};

}
#endif // M8RUI_DASHBOARD_PRESENTER_H
