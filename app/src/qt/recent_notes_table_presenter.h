/*
 recent_notes_table_presenter.h     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_RECENT_NOTES_TABLE_PRESENTER_H
#define M8RUI_RECENT_NOTES_TABLE_PRESENTER_H

#include <QtWidgets>

#include "../../lib/src/representations/html/html_outline_representation.h"

#include "recent_notes_table_view.h"
#include "recent_notes_table_model.h"
#include "html_delegate.h"

namespace m8r {

class RecentNotesTablePresenter : public QObject
{
    Q_OBJECT

    bool isDashboardlet;

    RecentNotesTableView* view;
    RecentNotesTableModel* model;

public:
    explicit RecentNotesTablePresenter(RecentNotesTableView* view, HtmlOutlineRepresentation* htmlRepresentation);
    RecentNotesTablePresenter(const RecentNotesTablePresenter&) = delete;
    RecentNotesTablePresenter(const RecentNotesTablePresenter&&) = delete;
    RecentNotesTablePresenter &operator=(const RecentNotesTablePresenter&) = delete;
    RecentNotesTablePresenter &operator=(const RecentNotesTablePresenter&&) = delete;
    ~RecentNotesTablePresenter();

    RecentNotesTableModel* getModel() const { return model; }
    RecentNotesTableView* getView() const { return view; }

    void refresh(const std::vector<Note*>& notes);
};

}
#endif // M8RUI_RECENT_NOTES_TABLE_PRESENTER_H
