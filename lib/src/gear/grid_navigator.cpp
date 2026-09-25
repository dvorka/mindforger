/*
 grid_navigator.cpp     MindForger thinking notebook

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
#include "grid_navigator.h"

#include <algorithm>

namespace m8r {

using std::vector;

const int GridNavigator::NO_ITEM;

GridNavigator::GridNavigator(int columns)
    : columns{columns>0?columns:1}
{
    setSections(vector<int>{});
}

GridNavigator::~GridNavigator()
{
}

void GridNavigator::setSections(const vector<int>& sectionSizes)
{
    sectionStarts.clear();

    int start{0};
    for(int size:sectionSizes) {
        // a section w/o items is not shown - it must not break the navigation
        if(size > 0) {
            sectionStarts.push_back(start);
            start += size;
        }
    }

    // sentinel which closes the last section
    sectionStarts.push_back(start);
}

int GridNavigator::getItemsCount() const
{
    return sectionStarts.empty()?0:sectionStarts.back();
}

int GridNavigator::getSectionsCount() const
{
    // the last item of sectionStarts is the sentinel - it is not a section
    return sectionStarts.empty()?0:static_cast<int>(sectionStarts.size())-1;
}

int GridNavigator::getSectionStart(int section) const
{
    return sectionStarts[static_cast<size_t>(section)];
}

int GridNavigator::getSectionEnd(int section) const
{
    return sectionStarts[static_cast<size_t>(section)+1];
}

int GridNavigator::getSection(int index) const
{
    if(index < 0 || index >= getItemsCount()) {
        return NO_ITEM;
    }

    for(int section=0; section<getSectionsCount(); section++) {
        if(index < getSectionEnd(section)) {
            return section;
        }
    }

    return NO_ITEM;
}

int GridNavigator::getItemAbove(int index) const
{
    const int section = getSection(index);
    if(section == NO_ITEM) {
        return NO_ITEM;
    }

    const int start = getSectionStart(section);
    const int column = (index-start) % columns;

    if(index-column-columns >= start) {
        // the row above is a full row of the same section
        return index-columns;
    }
    if(section == 0) {
        // there is nothing above the first row of the first section
        return NO_ITEM;
    }

    // the last row of the previous section may be shorter than the columns
    const int previousStart = getSectionStart(section-1);
    const int lastRowStart
        = previousStart + ((start-previousStart-1)/columns)*columns;

    return std::min(lastRowStart+column, start-1);
}

int GridNavigator::getItemBelow(int index) const
{
    const int section = getSection(index);
    if(section == NO_ITEM) {
        return NO_ITEM;
    }

    const int start = getSectionStart(section);
    const int end = getSectionEnd(section);
    const int column = (index-start) % columns;
    const int rowBelowStart = index-column+columns;

    if(rowBelowStart < end) {
        // the row below is in the same section, but it may be shorter
        return std::min(rowBelowStart+column, end-1);
    }
    if(section+1 >= getSectionsCount()) {
        // there is nothing below the last row of the last section
        return index;
    }

    // the next section starts a new row which may be shorter than the columns
    return std::min(end+column, getSectionEnd(section+1)-1);
}

} // m8r namespace
