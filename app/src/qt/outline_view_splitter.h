/*
 outline_view_splitter.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_OUTLINE_VIEW_SPLITTER_H
#define M8RUI_OUTLINE_VIEW_SPLITTER_H

#include <QtWidgets>

#include "outline_view.h"
#include "outline_tree_view.h"
#include "assoc_leaderboard_view.h"

namespace m8r {

/**
 * @brief Outline view splitter.
 *
 * - Splitter ----------
 * | OutlineView       |
 * |   OutlineLabel    |
 * |   OutlineTreeView |
 * ---------------------
 * | AALeaderboardView |
 * ---------------------
 *
 */
class OutlineViewSplitter : public QSplitter
{
    Q_OBJECT

private:
    OutlineView* outlineView;
    AssocLeaderboardView* assocLeaderboardView;

public:
    explicit OutlineViewSplitter(QWidget* parent);
    OutlineViewSplitter(const OutlineViewSplitter&) = delete;
    OutlineViewSplitter(const OutlineViewSplitter&&) = delete;
    OutlineViewSplitter &operator=(const OutlineViewSplitter&) = delete;
    OutlineViewSplitter &operator=(const OutlineViewSplitter&&) = delete;
    ~OutlineViewSplitter();

    void refreshHeader(const std::string& name) { outlineView->refreshHeader(name); }
    const QPushButton* getNameLabel() const { return outlineView->getNameLabel(); }
    OutlineTreeView* getOutlineTree() const { return outlineView->getOutlineTree(); }
    AssocLeaderboardView* getAssocLeaderboard() const { return assocLeaderboardView; }
};

}
#endif // M8RUI_OUTLINE_VIEW_SPLITTER_H
