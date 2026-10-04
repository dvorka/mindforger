/*
 recent_notes_presenter.cpp     MindForger thinking notebook

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
#include "recent_notes_presenter.h"

namespace m8r {

using namespace std;

RecentNotesPresenter::RecentNotesPresenter(RecentNotesView* view, HtmlOutlineRepresentation* htmlRepresentation)
{
    this->view = view;
    this->model = new RecentNotesModel(this, htmlRepresentation);
    this->view->getTree()->setModel(this->model);

    // ensure HTML cells rendering
    HtmlDelegate* delegate = new HtmlDelegate();
    this->view->getTree()->setItemDelegate(delegate);
}

RecentNotesPresenter::~RecentNotesPresenter()
{
}

void RecentNotesPresenter::refresh(const vector<Note*>& notes, bool editedOnly)
{
    vector<RecentNotes::HistoryEntry> history{};
    RecentNotes::toHistory(
        notes,
        editedOnly?RecentNotes::Mode::EDITED:RecentNotes::Mode::VIEWED_OR_EDITED,
        static_cast<size_t>(Configuration::getInstance().getRecentNotesUiLimit()),
        history);
    vector<RecentNotes::Group> groups{};
    RecentNotes::groupConsecutive(history, groups);

    model->removeAllRows();
    for(const RecentNotes::Group& g:groups) {
        model->addGroup(g);
    }

    view->setEditedOnly(editedOnly);

    RecentNotesTreeView* tree = view->getTree();
    tree->expandAll();
    tree->setCurrentIndex(model->index(0, 0));
    tree->setFocus();
}

Outline* RecentNotesPresenter::getSelectedOutline() const
{
    return RecentNotesModel::getOutline(view->getTree()->currentIndex());
}

Note* RecentNotesPresenter::getSelectedNote() const
{
    return RecentNotesModel::getNote(view->getTree()->currentIndex());
}

} // m8r namespace
