/*
 outlines_table_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_OUTLINE_TABLE_PRESENTER_H
#define M8RUI_OUTLINE_TABLE_PRESENTER_H

#include <QtWidgets>

#include "../../lib/src/representations/html/html_outline_representation.h"

#include "outlines_table_view.h"
#include "outlines_table_model.h"
#include "html_delegate.h"

namespace m8r {

class OutlinesTablePresenter : public QObject
{
    Q_OBJECT

    OutlinesTableView* view;
    OutlinesTableModel* model;

public:
    static const int NO_ROW = -1;

public:
    OutlinesTablePresenter(OutlinesTableView* view, HtmlOutlineRepresentation* htmlRepresentation);
    OutlinesTablePresenter(const OutlinesTablePresenter&) = delete;
    OutlinesTablePresenter(const OutlinesTablePresenter&&) = delete;
    OutlinesTablePresenter &operator=(const OutlinesTablePresenter&) = delete;
    OutlinesTablePresenter &operator=(const OutlinesTablePresenter&&) = delete;

    OutlinesTableModel* getModel() const { return model; }
    OutlinesTableView* getView() const { return view; }

    void refresh(const std::vector<Outline*>& outlines);
    int getCurrentRow() const;
};

}
#endif // M8RUI_OUTLINE_TABLE_PRESENTER_H
