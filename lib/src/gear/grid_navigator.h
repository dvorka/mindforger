/*
 grid_navigator.h     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_GRID_NAVIGATOR_H
#define M8R_GRID_NAVIGATOR_H

#include <vector>

namespace m8r {

/**
 * @brief Keyboard navigation in a grid of items which is split to sections.
 *
 * Items are indexed as a single flat list, but they are laid out section
 * by section - every section starts a new row and the last row of a section
 * may be shorter than the others. Navigator maps the flat index to the
 * visual position and gets the item which is above/below the given item.
 */
class GridNavigator
{
public:
    /**
     * @brief There is no such item - the index is invalid or out of the grid.
     */
    static const int NO_ITEM = -1;

    explicit GridNavigator(int columns=1);
    GridNavigator(const GridNavigator&) = default;
    GridNavigator& operator=(const GridNavigator&) = default;
    ~GridNavigator();

    /**
     * @brief Lay out the items - sections w/o items are skipped.
     */
    void setSections(const std::vector<int>& sectionSizes);

    int getItemsCount() const;

    /**
     * @brief Get the section of the item.
     *
     * @return NO_ITEM if the index is out of the grid.
     */
    int getSection(int index) const;

    /**
     * @brief Get the item which is visually above the given item.
     *
     * @return NO_ITEM if the item is in the first row of the first section.
     */
    int getItemAbove(int index) const;

    /**
     * @brief Get the item which is visually below the given item.
     *
     * @return the given item if there is nothing below it.
     */
    int getItemBelow(int index) const;

private:
    int columns;
    /**
     * @brief Index of the first item of every section + sentinel w/ the count.
     */
    std::vector<int> sectionStarts;

    int getSectionStart(int section) const;
    int getSectionEnd(int section) const;
    int getSectionsCount() const;
};

}
#endif // M8R_GRID_NAVIGATOR_H
