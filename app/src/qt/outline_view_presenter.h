/*
 outline_view_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_VIEW_PRESENTER_H
#define M8RUI_OUTLINE_VIEW_PRESENTER_H

#include "../../lib/src/model/outline.h"
#include "../../lib/src/mind/mind.h"

#include <QtWidgets>

#include "outline_view_splitter.h"
#include "orloj_presenter.h"
#include "outline_tree_presenter.h"
#include "assoc_leaderboard_presenter.h"

namespace m8r {

class OrlojPresenter;
class OutlineTreePresenter;
class AssocLeaderboardPresenter;

class OutlineViewPresenter : public QObject
{
    Q_OBJECT

private:
    Outline* currentOutline;

    OutlineViewSplitter* view;
    OutlineTreePresenter* outlineTreePresenter;
    AssocLeaderboardPresenter* assocLeaderboardPresenter;

public:
    explicit OutlineViewPresenter(OutlineViewSplitter* view, OrlojPresenter* orloj);
    OutlineViewPresenter(const OutlineViewPresenter&) = delete;
    OutlineViewPresenter(const OutlineViewPresenter&&) = delete;
    OutlineViewPresenter &operator=(const OutlineViewPresenter&) = delete;
    OutlineViewPresenter &operator=(const OutlineViewPresenter&&) = delete;

    Outline* getCurrentOutline() const { return currentOutline; }
    void refresh(std::string outlineName) { view->refreshHeader(outlineName); }
    void refresh(Outline* outline);
    void refresh(Note* note);
    void insertAndSelect(Note* note);
    void selectRowByNote(const Note* note);
    OutlineTreePresenter* getOutlineTree() const { return outlineTreePresenter; }
    AssocLeaderboardPresenter* getAssocLeaderboard() const { return assocLeaderboardPresenter; }

    ~OutlineViewPresenter();
};

}
#endif // M8RUI_OUTLINE_VIEW_PRESENTER_H
