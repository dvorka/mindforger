/*
 kanban_presenter.cpp     MindForger thinking notebook

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
#include "kanban_presenter.h"

namespace m8r {

using namespace std;

KanbanPresenter::KanbanPresenter(KanbanView* view, OrlojPresenter* orloj)
{
    this->view = view;

    for(int column=0; column<4; column++) {
        this->columns.push_back(
            new KanbanColumnPresenter(
                view->getColumn(column), orloj, tr("")
            )
        );
    }
}

KanbanPresenter::~KanbanPresenter()
{
}

void KanbanPresenter::refresh(
    Kanban* kanban,
    const vector<Note*>& ons,
    const vector<Outline*>& os,
    const vector<Note*>& ns
) {
    MF_DEBUG("Rendering Kanban: " << kanban->getName() << "..." << endl);

    this->kanban = kanban;

    vector<Note*> upperLeftNs{};
    vector<Note*> upperRightNs{};
    vector<Note*> lowerLeftNs{};
    vector<Note*> lowerRightNs{};

    Outline::organizeToKanbanColumns(
        this->kanban, ons, os, ns, upperLeftNs, upperRightNs, lowerLeftNs, lowerRightNs
    );

    // set quadrant titles
    QString title{};
    title = QString::fromStdString(Organizer::tagsToString(kanban->getUpperLeftTags(), false));
    columns[0]->setTitle(title);
    title = QString::fromStdString(Organizer::tagsToString(kanban->getUpperRightTags(), false));
    columns[1]->setTitle(title);
    title = QString::fromStdString(Organizer::tagsToString(kanban->getLowerLeftTags(), false));
    columns[2]->setTitle(title);
    title = QString::fromStdString(Organizer::tagsToString(kanban->getLowerRightTags(), false));
    columns[3]->setTitle(title);

    columns[0]->refresh(upperLeftNs, true, false);
    columns[1]->refresh(upperRightNs, true, true);
    columns[2]->refresh(lowerLeftNs, false, false);
    columns[3]->refresh(lowerRightNs, false, true);

    view->getColumn(0)->setFocus();
}

} // m8r namespace
