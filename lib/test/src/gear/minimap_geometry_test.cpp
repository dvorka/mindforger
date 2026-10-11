/*
 minimap_geometry_test.cpp     MindForger application test

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

#include <gtest/gtest.h>

#include "../../../src/gear/minimap_geometry.h"

using namespace std;
using namespace m8r;

/*
 * Long text: 1000 lines, editor shows 20 lines, 200px strip w/ 2px lines
 * shows 100 lines i.e. the strip scrolls 900 lines while the editor 980 lines.
 */
static const int LONG_TOTAL = 1000;
static const int LONG_VISIBLE = 20;
static const int STRIP_HEIGHT = 200;

TEST(MinimapGeometryTestCase, EmptyText)
{
    // GIVEN a minimap of an empty text
    MinimapGeometry geometry{};

    // WHEN the geometry is computed
    geometry.update(0, 20, 0, STRIP_HEIGHT);

    // THEN there is nothing to show, but the geometry is valid
    cout << "Empty text: first=" << geometry.getFirstLine()
         << " top=" << geometry.getViewportTop()
         << " height=" << geometry.getViewportHeight() << endl;
    ASSERT_EQ(0, geometry.getFirstLine());
    ASSERT_EQ(0, geometry.getViewportTop());
    ASSERT_EQ(0, geometry.getViewportHeight());
    ASSERT_EQ(0, geometry.getLineAt(50));
    ASSERT_EQ(0, geometry.getFirstVisibleLineFor(50));
}

TEST(MinimapGeometryTestCase, ZeroStripHeight)
{
    // GIVEN a minimap which is collapsed
    MinimapGeometry geometry{};

    // WHEN the geometry is computed
    geometry.update(LONG_TOTAL, LONG_VISIBLE, 500, 0);

    // THEN the viewport has no area and nothing is divided by zero
    ASSERT_EQ(0, geometry.getStripLines());
    ASSERT_EQ(0, geometry.getViewportHeight());
    ASSERT_EQ(0, geometry.getFirstVisibleLineFor(10));
}

TEST(MinimapGeometryTestCase, TextFitsTheStrip)
{
    // GIVEN a text which fits the strip, but not the editor
    MinimapGeometry geometry{};

    // WHEN the editor is scrolled down by 10 lines
    geometry.update(50, LONG_VISIBLE, 10, STRIP_HEIGHT);

    // THEN the strip does not scroll and the viewport follows the editor
    ASSERT_EQ(0, geometry.getFirstLine());
    ASSERT_EQ(20, geometry.getViewportTop());
    ASSERT_EQ(LONG_VISIBLE*MinimapGeometry::DEFAULT_LINE_HEIGHT, geometry.getViewportHeight());
    ASSERT_EQ(10, geometry.getFirstVisibleLineFor(geometry.getViewportTop()));
    ASSERT_EQ(25, geometry.getLineAt(50));
    ASSERT_EQ(49, geometry.getLineAt(150));
}

TEST(MinimapGeometryTestCase, TextFitsTheEditor)
{
    // GIVEN a text which is shorter than the editor
    MinimapGeometry geometry{};

    // WHEN the editor tries to scroll
    geometry.update(5, LONG_VISIBLE, 3, STRIP_HEIGHT);

    // THEN the viewport covers just the text and there is nowhere to scroll
    ASSERT_EQ(0, geometry.getFirstLine());
    ASSERT_EQ(0, geometry.getViewportTop());
    ASSERT_EQ(5*MinimapGeometry::DEFAULT_LINE_HEIGHT, geometry.getViewportHeight());
    ASSERT_EQ(0, geometry.getFirstVisibleLineFor(100));
}

TEST(MinimapGeometryTestCase, LongTextScrolled)
{
    // GIVEN a minimap of a long text
    MinimapGeometry geometry{};

    // WHEN the editor is at the top, in the middle and at the bottom
    geometry.update(LONG_TOTAL, LONG_VISIBLE, 0, STRIP_HEIGHT);
    const int topFirst = geometry.getFirstLine();
    const int topViewport = geometry.getViewportTop();
    geometry.update(LONG_TOTAL, LONG_VISIBLE, 490, STRIP_HEIGHT);
    const int middleFirst = geometry.getFirstLine();
    const int middleViewport = geometry.getViewportTop();
    geometry.update(LONG_TOTAL, LONG_VISIBLE, 980, STRIP_HEIGHT);
    const int bottomFirst = geometry.getFirstLine();
    const int bottomViewport = geometry.getViewportTop();
    const int bottomHeight = geometry.getViewportHeight();

    // THEN the strip scrolls proportionally and the viewport stays in the strip
    cout << "Long text first lines: " << topFirst << " " << middleFirst << " " << bottomFirst << endl;
    cout << "Long text viewports: " << topViewport << " " << middleViewport << " " << bottomViewport << endl;
    ASSERT_EQ(0, topFirst);
    ASSERT_EQ(0, topViewport);
    ASSERT_EQ(450, middleFirst);
    ASSERT_EQ(80, middleViewport);
    ASSERT_EQ(LONG_TOTAL - 100, bottomFirst);
    ASSERT_EQ(STRIP_HEIGHT - LONG_VISIBLE*MinimapGeometry::DEFAULT_LINE_HEIGHT, bottomViewport);
    ASSERT_EQ(STRIP_HEIGHT, bottomViewport + bottomHeight);
}

TEST(MinimapGeometryTestCase, LongTextScrolledBeyondTheEnd)
{
    // GIVEN a minimap of a long text
    MinimapGeometry geometry{};

    // WHEN the editor reports an out of range first visible line
    geometry.update(LONG_TOTAL, LONG_VISIBLE, 5000, STRIP_HEIGHT);

    // THEN it is clamped to the bottom of the text
    ASSERT_EQ(LONG_TOTAL - 100, geometry.getFirstLine());
    ASSERT_EQ(STRIP_HEIGHT, geometry.getViewportTop() + geometry.getViewportHeight());
}

TEST(MinimapGeometryTestCase, LineAtY)
{
    // GIVEN a long text w/ the editor in the middle
    MinimapGeometry geometry{};
    geometry.update(LONG_TOTAL, LONG_VISIBLE, 490, STRIP_HEIGHT);

    // WHEN lines are resolved at the top, middle, bottom and outside the strip
    const int top = geometry.getLineAt(0);
    const int middle = geometry.getLineAt(101);
    const int above = geometry.getLineAt(-10);
    const int below = geometry.getLineAt(100000);

    // THEN lines are relative to the first line of the strip and clamped to the text
    ASSERT_EQ(450, top);
    ASSERT_EQ(500, middle);
    ASSERT_EQ(450, above);
    ASSERT_EQ(LONG_TOTAL - 1, below);
}

TEST(MinimapGeometryTestCase, FirstVisibleLineForViewportTop)
{
    // GIVEN a minimap of a long text
    MinimapGeometry geometry{};
    geometry.update(LONG_TOTAL, LONG_VISIBLE, 0, STRIP_HEIGHT);

    // WHEN the viewport rectangle is dragged to the top, middle, bottom and beyond
    const int top = geometry.getFirstVisibleLineFor(0);
    const int middle = geometry.getFirstVisibleLineFor(80);
    const int bottom = geometry.getFirstVisibleLineFor(160);
    const int above = geometry.getFirstVisibleLineFor(-50);
    const int below = geometry.getFirstVisibleLineFor(1000);

    // THEN it is the inverse of the viewport top clamped to the scrollable range
    ASSERT_EQ(0, top);
    ASSERT_EQ(490, middle);
    ASSERT_EQ(980, bottom);
    ASSERT_EQ(0, above);
    ASSERT_EQ(980, below);
}
