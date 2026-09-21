/*
 grid_navigator_test.cpp     MindForger application test

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
#include <iostream>
#include <vector>

#include <gtest/gtest.h>

#include "../../../src/gear/grid_navigator.h"

using namespace std;
using namespace m8r;

/**
 * @brief Sections of the emojis dialog - 11 columns, 90 + 49 + 1 + 1 items.
 *
 * Emoji section has rows 0-7 full and row 8 w/ items 88 and 89 only, Greek
 * alphabet section has rows 0-3 full and row 4 w/ items 134-138 only.
 */
static GridNavigator createEmojisNavigator()
{
    GridNavigator navigator{11};
    navigator.setSections(vector<int>{90, 49, 1, 1});

    return navigator;
}

TEST(GridNavigatorTestCase, EmptyGrid)
{
    // GIVEN a grid w/o any item
    GridNavigator navigator{11};
    navigator.setSections(vector<int>{0, 0});

    // WHEN the grid is navigated
    const int count = navigator.getItemsCount();
    const int above = navigator.getItemAbove(0);
    const int below = navigator.getItemBelow(0);

    // THEN there is nothing to navigate to
    cout << "Empty grid: count=" << count << " above=" << above
         << " below=" << below << endl;
    ASSERT_EQ(0, count);
    ASSERT_EQ(GridNavigator::NO_ITEM, above);
    ASSERT_EQ(GridNavigator::NO_ITEM, below);
}

TEST(GridNavigatorTestCase, SectionsWithoutItemsAreSkipped)
{
    // GIVEN a grid whose 1st and 3rd sections are filtered out
    GridNavigator navigator{11};
    navigator.setSections(vector<int>{0, 3, 0, 2});

    // WHEN the sections of the items are resolved
    const int firstSection = navigator.getSection(0);
    const int secondSection = navigator.getSection(3);

    // THEN only the sections w/ items are laid out
    cout << "Sections of items 0 and 3: " << firstSection << ", "
         << secondSection << endl;
    ASSERT_EQ(5, navigator.getItemsCount());
    ASSERT_EQ(0, firstSection);
    ASSERT_EQ(1, secondSection);
    ASSERT_EQ(GridNavigator::NO_ITEM, navigator.getSection(5));
}

TEST(GridNavigatorTestCase, MoveWithinSection)
{
    // GIVEN the sections of the emojis dialog
    GridNavigator navigator = createEmojisNavigator();

    // WHEN the item of the 3rd row and 5th column is left
    const int above = navigator.getItemAbove(26);
    const int below = navigator.getItemBelow(26);

    // THEN the column is kept and the row changes by one
    cout << "Item 26: above=" << above << " below=" << below << endl;
    ASSERT_EQ(15, above);
    ASSERT_EQ(37, below);
}

TEST(GridNavigatorTestCase, MoveToShorterLastRowOfSection)
{
    // GIVEN the sections of the emojis dialog
    GridNavigator navigator = createEmojisNavigator();

    // WHEN the last full row of the 1st section is left downwards
    const int belowFirstColumn = navigator.getItemBelow(77);
    const int belowLastColumn = navigator.getItemBelow(82);

    // THEN the move stops at the last item of the shorter row below
    cout << "Below items 77 and 82: " << belowFirstColumn << ", "
         << belowLastColumn << endl;
    ASSERT_EQ(88, belowFirstColumn);
    // the row below has 2 items only - the 6th column is clamped to the 2nd
    ASSERT_EQ(89, belowLastColumn);
}

TEST(GridNavigatorTestCase, MoveDownToNextSection)
{
    // GIVEN the sections of the emojis dialog
    GridNavigator navigator = createEmojisNavigator();

    // WHEN the last row of the 1st section is left downwards
    const int belowFirstColumn = navigator.getItemBelow(88);
    const int belowSecondColumn = navigator.getItemBelow(89);

    // THEN the 1st row of the next section is entered w/ the column kept
    cout << "Below items 88 and 89: " << belowFirstColumn << ", "
         << belowSecondColumn << endl;
    ASSERT_EQ(90, belowFirstColumn);
    ASSERT_EQ(91, belowSecondColumn);
}

TEST(GridNavigatorTestCase, MoveUpToPreviousSection)
{
    // GIVEN the sections of the emojis dialog
    GridNavigator navigator = createEmojisNavigator();

    // WHEN the 1st row of the 2nd section is left upwards
    const int aboveFirstColumn = navigator.getItemAbove(90);
    const int aboveSixthColumn = navigator.getItemAbove(95);

    // THEN the shorter last row of the previous section is entered
    cout << "Above items 90 and 95: " << aboveFirstColumn << ", "
         << aboveSixthColumn << endl;
    ASSERT_EQ(88, aboveFirstColumn);
    // the row above has 2 items only - the 6th column is clamped to the 2nd
    ASSERT_EQ(89, aboveSixthColumn);
}

TEST(GridNavigatorTestCase, MoveToShorterSection)
{
    // GIVEN the sections of the emojis dialog
    GridNavigator navigator = createEmojisNavigator();

    // WHEN the sections w/ a single item are entered
    const int below = navigator.getItemBelow(138);
    const int above = navigator.getItemAbove(139);

    // THEN the column is clamped to the only item of the section
    cout << "Below item 138: " << below << " above item 139: " << above << endl;
    ASSERT_EQ(139, below);
    // 139 is the 1st item of the 3rd section > the last row of the 2nd one
    ASSERT_EQ(134, above);
}

TEST(GridNavigatorTestCase, MoveOutOfGrid)
{
    // GIVEN the sections of the emojis dialog
    GridNavigator navigator = createEmojisNavigator();

    // WHEN the 1st row of the 1st section and the very last row are left
    const int aboveFirstRow = navigator.getItemAbove(0);
    const int aboveFirstRowColumn = navigator.getItemAbove(10);
    const int belowLastRow = navigator.getItemBelow(140);

    // THEN there is nothing above, while the last item keeps the position
    cout << "Above items 0 and 10: " << aboveFirstRow << ", "
         << aboveFirstRowColumn << " below item 140: " << belowLastRow << endl;
    ASSERT_EQ(GridNavigator::NO_ITEM, aboveFirstRow);
    ASSERT_EQ(GridNavigator::NO_ITEM, aboveFirstRowColumn);
    ASSERT_EQ(140, belowLastRow);
}

TEST(GridNavigatorTestCase, InvalidIndex)
{
    // GIVEN the sections of the emojis dialog
    GridNavigator navigator = createEmojisNavigator();

    // WHEN an index which is out of the grid is navigated
    const int aboveNegative = navigator.getItemAbove(-1);
    const int belowTooBig = navigator.getItemBelow(141);

    // THEN there is no such item
    cout << "Above item -1: " << aboveNegative << " below item 141: "
         << belowTooBig << endl;
    ASSERT_EQ(GridNavigator::NO_ITEM, aboveNegative);
    ASSERT_EQ(GridNavigator::NO_ITEM, belowTooBig);
}
