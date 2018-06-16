/*
 outlines_table_view.h     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINES_TABLE_H_
#define M8RUI_OUTLINES_TABLE_H_

#include <vector>

#include "../../lib/src/mind/mind.h"

#include <QtWidgets>

namespace m8r {

class OutlinesTableView : public QTableView
{
    Q_OBJECT

private:
    // if view is width < threshold columns, then shows simplified view w/o Mind-related columns
    static constexpr int SIMPLIFIED_VIEW_THRESHOLD_WIDTH = 75*2;

public:
    static const int COLUMN_COUNT = 7;
public:
    explicit OutlinesTableView(QWidget* parent);
    OutlinesTableView(const OutlinesTableView&) = delete;
    OutlinesTableView(const OutlinesTableView&&) = delete;
    OutlinesTableView &operator=(const OutlinesTableView&) = delete;
    OutlinesTableView &operator=(const OutlinesTableView&&) = delete;

    int getColumnCount() { return COLUMN_COUNT; }

    virtual void paintEvent(QPaintEvent* event) override;
};


}

#endif // M8RUI_OUTLINES_TABLE_H_
