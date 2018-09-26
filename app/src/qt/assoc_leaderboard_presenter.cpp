/*
 assoc_leaderboard_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "assoc_leaderboard_presenter.h"

namespace m8r {

AssocLeaderboardPresenter::AssocLeaderboardPresenter(AssocLeaderboardView* view, OrlojPresenter* orloj)
{
    this->view = view;
    this->model = new AssocLeaderboardModel(this);
    this->view->setModel(this->model);

    this->orloj = orloj;

    // ensure HTML cells rendering
    HtmlDelegate* delegate = new HtmlDelegate();
    this->view->setItemDelegate(delegate);

    QObject::connect(
        view->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this,
        SLOT(slotShowNote(const QItemSelection&, const QItemSelection&)));

    lastAssociations = nullptr;
}

AssocLeaderboardPresenter::~AssocLeaderboardPresenter()
{
    if(lastAssociations) delete lastAssociations;
}

void AssocLeaderboardPresenter::slotShowNote(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    QModelIndexList indices = selected.indexes();
    if(indices.size()) {
        const QModelIndex& index = indices.at(0);
        QStandardItem* item
            = model->itemFromIndex(index);
        // IMPROVE make my role constant
        Note* note = item->data(Qt::UserRole + 1).value<Note*>();

        note->incReads();
        note->makeDirty();

        orloj->showFacetNoteView(note);
    } // else do nothing
}

void AssocLeaderboardPresenter::refresh(AssociatedNotes* associations)
{
    if(associations->getAssociations()->size()) {

        lastAssociations = associations;

        switch(associations->getSourceType()) {
        case OUTLINE:
            model->setTitle(QString::fromStdString(associations->getOutline()->getName()));
            break;
        case NOTE:
            model->setTitle(QString::fromStdString(associations->getNote()->getName()));
            break;
        case WORD:
            model->setTitle(QString::fromStdString(associations->getWord()));
            break;
        default:
            model->setTitle(tr("Associations"));
        }

        model->removeAllRows();
        for(auto& i:*lastAssociations->getAssociations()) {
            model->addRow(i.first, i.second);
        }

        view->setVisible(true);
    } else {
        view->setVisible(false);
    }
}

} // m8r namespace
