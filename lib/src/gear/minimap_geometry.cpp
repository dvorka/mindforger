/*
 minimap_geometry.cpp     MindForger thinking notebook

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
#include "minimap_geometry.h"

#include <algorithm>

namespace m8r {

using std::max;
using std::min;

const int MinimapGeometry::DEFAULT_LINE_HEIGHT;

MinimapGeometry::MinimapGeometry(int lineHeightPx)
    : lineHeightPx{lineHeightPx>0?lineHeightPx:DEFAULT_LINE_HEIGHT},
      totalLines{0},
      visibleLines{0},
      firstVisibleLine{0},
      stripHeightPx{0}
{
}

MinimapGeometry::~MinimapGeometry()
{
}

void MinimapGeometry::update(int totalLines, int visibleLines, int firstVisibleLine, int stripHeightPx)
{
    this->totalLines = max(0, totalLines);
    this->visibleLines = max(0, visibleLines);
    this->stripHeightPx = max(0, stripHeightPx);
    this->firstVisibleLine = max(0, min(firstVisibleLine, getMaxFirstVisibleLine()));
}

int MinimapGeometry::getMaxFirstVisibleLine() const
{
    return max(0, totalLines - visibleLines);
}

int MinimapGeometry::getStripLines() const
{
    return stripHeightPx / lineHeightPx;
}

int MinimapGeometry::getFirstLine() const
{
    const int maxFirstLine = totalLines - getStripLines();
    const int maxFirstVisibleLine = getMaxFirstVisibleLine();
    if(maxFirstLine <= 0 || maxFirstVisibleLine <= 0) {
        // the whole text fits the strip or the editor
        return 0;
    }

    // long long - no overflow for long texts
    const long long firstLine
        = static_cast<long long>(firstVisibleLine) * maxFirstLine / maxFirstVisibleLine;
    return static_cast<int>(min(static_cast<long long>(maxFirstLine), max(0LL, firstLine)));
}

int MinimapGeometry::getViewportTop() const
{
    const int top = (firstVisibleLine - getFirstLine()) * lineHeightPx;
    return max(0, min(top, stripHeightPx));
}

int MinimapGeometry::getViewportHeight() const
{
    const int height = min(visibleLines, totalLines) * lineHeightPx;
    return max(0, min(height, stripHeightPx - getViewportTop()));
}

int MinimapGeometry::getLineAt(int y) const
{
    if(totalLines <= 0) {
        return 0;
    }

    const int line = getFirstLine() + max(0, y) / lineHeightPx;
    return min(line, totalLines - 1);
}

int MinimapGeometry::getFirstVisibleLineFor(int viewportTopPx) const
{
    const int maxFirstVisibleLine = getMaxFirstVisibleLine();
    // the viewport rectangle travels from the top of the shown lines to their bottom
    const int travelPx = (min(totalLines, getStripLines()) - visibleLines) * lineHeightPx;
    if(maxFirstVisibleLine <= 0 || travelPx <= 0) {
        return 0;
    }

    const long long line
        = static_cast<long long>(max(0, viewportTopPx)) * maxFirstVisibleLine / travelPx;
    return static_cast<int>(min(static_cast<long long>(maxFirstVisibleLine), line));
}

} // m8r namespace
