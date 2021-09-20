/*
 organizer_quadrant_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "organizer_quadrant_presenter.h"

namespace m8r {

using namespace std;

OrganizerQuadrantPresenter::OrganizerQuadrantPresenter(
        OrganizerQuadrantView* view,
        OrlojPresenter* orloj,
        QString title)
{
    this->view = view;
    this->model = new OrganizerQuadrantModel(title, this, orloj->getMainPresenter()->getHtmlRepresentation());
    this->view->setModel(this->model);

    this->orloj = orloj;

    // ensure HTML cells rendering
    HtmlDelegate* delegate = new HtmlDelegate();
    this->view->setItemDelegate(delegate);

    // hit ENTER to open selected O
    QObject::connect(
        view,
        SIGNAL(signalShowSelectedNote()),
        this,
        SLOT(slotShowSelectedNote()));
    QObject::connect(
        this->view->horizontalHeader(), SIGNAL(sectionClicked(int)),
        this, SLOT(slotHeaderClicked(int))
    );
    /* click O to open O
    QObject::connect(
        view->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this,
        SLOT(slotShowOutline(const QItemSelection&, const QItemSelection&)));
    */
}

OrganizerQuadrantPresenter::~OrganizerQuadrantPresenter()
{
}

int OrganizerQuadrantPresenter::getCurrentRow() const
{
    QModelIndexList indexes = view->selectionModel()->selection().indexes();
    for(int i=0; i<indexes.count(); i++) {
        return indexes.at(i).row();
    }
    return NO_ROW;
}

void OrganizerQuadrantPresenter::slotShowSelectedNote()
{
    int row = getCurrentRow();
    if(row != NO_ROW) {
        QStandardItem* item = model->item(row);
        if(item) {
            // IMPROVE make my role constant
            Note* note = item->data(Qt::UserRole + 1).value<Note*>();

            note->incReads();
            note->makeDirty();

            orloj->showFacetNoteView(note);
        } else {
            orloj->getMainPresenter()->getStatusBar()->showInfo(QString(tr("Selected Notebook not found!")));
        }
    } else {
        orloj->getMainPresenter()->getStatusBar()->showInfo(QString(tr("No Notebook selected!")));
    }
}

void OrganizerQuadrantPresenter::slotShowNote(const QItemSelection& selected, const QItemSelection& deselected)
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


void OrganizerQuadrantPresenter::slotHeaderClicked(int section)
{
    Q_UNUSED(section);

    MF_DEBUG("Organizer quadrant presenter: O/N table header clicked..." << endl);
    orloj->getMainPresenter()->doActionOrganizerEdit();
}

void OrganizerQuadrantPresenter::refresh(const std::vector<Note*>& ts, bool urgency, bool importance)
{
    model->removeAllRows();
    if(ts.size()) {
        view->setVisible(true);
        for(auto& t:ts) {
            model->addRow(t, urgency, importance);
        }

        this->view->setCurrentIndex(this->model->index(0, 0));
        this->view->setFocus();
    }
}

} // m8r namespace
