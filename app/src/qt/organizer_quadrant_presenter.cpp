/*
 organizer_quadrant_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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

    QObject::connect(
        view->selectionModel(),
        SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
        this,
        SLOT(slotShowOutline(const QItemSelection&, const QItemSelection&)));
}

OrganizerQuadrantPresenter::~OrganizerQuadrantPresenter()
{
}

void OrganizerQuadrantPresenter::slotShowOutline(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    QModelIndexList indices = selected.indexes();
    if(indices.size()) {
        const QModelIndex& index = indices.at(0);
        QStandardItem* item
            = model->itemFromIndex(index);
        // IMPROVE make my role constant
        Outline* outline = item->data(Qt::UserRole + 1).value<Outline*>();

        outline->incReads();
        outline->makeDirty();

        orloj->showFacetOutline(outline);
    } // else do nothing
}

void OrganizerQuadrantPresenter::refresh(const std::vector<Outline*>& os, bool urgency, bool importance)
{
    model->removeAllRows();
    if(os.size()) {
        view->setVisible(true);
        for(auto& o:os) {
            model->addRow(o, urgency, importance);
        }
    }
}

} // m8r namespace
