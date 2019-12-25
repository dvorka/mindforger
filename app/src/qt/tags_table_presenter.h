/*
 tags_table_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_TAGS_TABLE_PRESENTER_H
#define M8RUI_TAGS_TABLE_PRESENTER_H

#include <QtWidgets>

#include "../../lib/src/representations/html/html_outline_representation.h"

#include "tags_table_view.h"
#include "tags_table_model.h"
#include "html_delegate.h"

namespace m8r {

class TagsTablePresenter : public QObject
{
    Q_OBJECT

    TagsTableView* view;
    TagsTableModel* model;

public:
    explicit TagsTablePresenter(TagsTableView* view, HtmlOutlineRepresentation* htmlRepresentation);
    TagsTablePresenter(const TagsTablePresenter&) = delete;
    TagsTablePresenter(const TagsTablePresenter&&) = delete;
    TagsTablePresenter &operator=(const TagsTablePresenter&) = delete;
    TagsTablePresenter &operator=(const TagsTablePresenter&&) = delete;
    ~TagsTablePresenter();

    TagsTableModel* getModel() const { return model; }
    TagsTableView* getView() const { return view; }

    void refresh(const std::map<const Tag*, int>& tags);
};

}
#endif // M8RUI_TAGS_TABLE_PRESENTER_H
