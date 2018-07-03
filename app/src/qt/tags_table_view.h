/*
 tags_table_view.h     MindForger thinking notebook

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
#ifndef M8RUI_TAGS_TABLE_VIEW_H
#define M8RUI_TAGS_TABLE_VIEW_H

#include <QtWidgets>

namespace m8r {

class TagsTableView : public QTableView
{
    Q_OBJECT

public:
    static const int COLUMN_COUNT = 2;
public:
    explicit TagsTableView(QWidget* parent);
    TagsTableView(const TagsTableView&) = delete;
    TagsTableView(const TagsTableView&&) = delete;
    TagsTableView &operator=(const TagsTableView&) = delete;
    TagsTableView &operator=(const TagsTableView&&) = delete;
    ~TagsTableView();

    int getColumnCount() { return COLUMN_COUNT; }

    virtual void paintEvent(QPaintEvent* event) override;
};

}
#endif // M8RUI_TAGS_TABLE_VIEW_H
