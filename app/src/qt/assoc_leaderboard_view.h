/*
 assoc_leaderboard_view.h     MindForger thinking notebook

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
#ifndef M8RUI_ASSOC_LEADERBOARD_VIEW_H
#define M8RUI_ASSOC_LEADERBOARD_VIEW_H

#include <QtWidgets>

#include "../../../lib/src/debug.h"

namespace m8r {

class AssocLeaderboardView : public QTableView
{
    Q_OBJECT

public:
    explicit AssocLeaderboardView(QWidget* parent);
    AssocLeaderboardView(const AssocLeaderboardView&) = delete;
    AssocLeaderboardView(const AssocLeaderboardView&&) = delete;
    AssocLeaderboardView &operator=(const AssocLeaderboardView&) = delete;
    AssocLeaderboardView &operator=(const AssocLeaderboardView&&) = delete;
    virtual ~AssocLeaderboardView() override {}

    virtual void resizeEvent(QResizeEvent* event) override;
};

}
#endif // M8RUI_ASSOC_LEADERBOARD_VIEW_H
