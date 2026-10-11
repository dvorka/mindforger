/*
 recent_notes_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_RECENT_NOTES_PRESENTER_H
#define M8RUI_RECENT_NOTES_PRESENTER_H

#include <QtWidgets>

#include "../../lib/src/model/recent_notes.h"
#include "../../lib/src/representations/html/html_outline_representation.h"

#include "recent_notes_view.h"
#include "recent_notes_model.h"
#include "html_delegate.h"

namespace m8r {

class RecentNotesPresenter : public QObject
{
    Q_OBJECT

    RecentNotesView* view;
    RecentNotesModel* model;

public:
    explicit RecentNotesPresenter(RecentNotesView* view, HtmlOutlineRepresentation* htmlRepresentation);
    RecentNotesPresenter(const RecentNotesPresenter&) = delete;
    RecentNotesPresenter(const RecentNotesPresenter&&) = delete;
    RecentNotesPresenter &operator=(const RecentNotesPresenter&) = delete;
    RecentNotesPresenter &operator=(const RecentNotesPresenter&&) = delete;
    ~RecentNotesPresenter();

    RecentNotesModel* getModel() const { return model; }
    RecentNotesView* getView() const { return view; }

    /**
     * @brief Show Notes grouped by Notebook.
     *
     * @param notes         all Notes (any order).
     * @param editedOnly    show edited Notes only, else viewed or edited Notes.
     */
    void refresh(const std::vector<Note*>& notes, bool editedOnly);

    /**
     * @brief Get selected Notebook - Notebook of the selected Note, if Note is selected.
     */
    Outline* getSelectedOutline() const;
    /**
     * @brief Get selected Note - nullptr if Notebook row is selected.
     */
    Note* getSelectedNote() const;
};

}
#endif // M8RUI_RECENT_NOTES_PRESENTER_H
