/*
 outlines_table_presenter.cpp     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_TABLE_PRESENTER_H
#define M8RUI_OUTLINE_TABLE_PRESENTER_H

#include <QtWidgets>

#include "outlines_table_view.h"
#include "outlines_table_model.h"
#include "html_delegate.h"

namespace m8r {

class OutlinesTablePresenter : public QObject
{
    Q_OBJECT

private:
    OutlinesTableView* view;
    OutlinesTableModel* model;

public:
    OutlinesTablePresenter(OutlinesTableView* view);
    OutlinesTablePresenter(const OutlinesTablePresenter&) = delete;
    OutlinesTablePresenter(const OutlinesTablePresenter&&) = delete;
    OutlinesTablePresenter &operator=(const OutlinesTablePresenter&) = delete;
    OutlinesTablePresenter &operator=(const OutlinesTablePresenter&&) = delete;
    OutlinesTableModel* getModel(void) const { return model; }

    void refresh(const std::vector<Outline*>& outlines);
};

}
#endif // M8RUI_OUTLINE_TABLE_PRESENTER_H
