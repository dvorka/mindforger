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
    : view{view},
      orloj{orloj}
{
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

vector<const Tag*> KanbanPresenter::getTagsForColumn(int columnNumber)
{
    static vector<const Tag*> empty{};

    if(this->kanban) {
        return kanban->getTagsForColumn(
            columnNumber, orloj->getMind()->getOntology()
        );
    }

    return empty;
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
    title = QString::fromStdString(
        Tags::tagsToString(kanban->getUpperLeftTags(), false)
    );
    columns[0]->setTitle(title);
    title = QString::fromStdString(
        Tags::tagsToString(kanban->getUpperRightTags(), false)
    );
    columns[1]->setTitle(title);
    title = QString::fromStdString(
        Tags::tagsToString(kanban->getLowerLeftTags(), false)
    );
    columns[2]->setTitle(title);
    title = QString::fromStdString(
        Tags::tagsToString(kanban->getLowerRightTags(), false)
    );
    columns[3]->setTitle(title);

    columns[0]->refresh(upperLeftNs, true, false);
    columns[1]->refresh(upperRightNs, true, true);
    columns[2]->refresh(lowerLeftNs, false, false);
    columns[3]->refresh(lowerRightNs, false, true);

    // IF at least one column with a tag exists,
    // THEN show only columns with tags,
    // ELSE show all EMPTY columns
    for(auto c:columns) {
        c->getView()->setVisible(true);
    }
    if(upperLeftNs.size()
       || upperRightNs.size()
       || lowerLeftNs.size()
       || lowerRightNs.size())
    {
        if(!upperLeftNs.size()) {
            columns[0]->getView()->setVisible(false);
        }
        if(!upperRightNs.size()) {
            columns[1]->getView()->setVisible(false);
        }
        if(!lowerLeftNs.size()) {
            columns[2]->getView()->setVisible(false);
        }
        if(!lowerRightNs.size()) {
            columns[3]->getView()->setVisible(false);
        }
    }

    view->getColumn(0)->setFocus();
}

void KanbanPresenter::getVisibleColumns(vector<KanbanColumnPresenter*>& visible)
{
    for(auto c:columns) {
        if(c->getView()->isVisible()) {
            visible.push_back(c);
        }
    }
}

KanbanColumnPresenter* KanbanPresenter::getNextVisibleColumn()
{
    vector<KanbanColumnPresenter*> visible{};
    this->getVisibleColumns(visible);

    for(unsigned i=0; i<visible.size(); i++) {
        if(visible[i]->getView()->hasFocus()) {
            return visible[(i+1)%visible.size()];
        }
    }

    return nullptr;
}

KanbanColumnPresenter* KanbanPresenter::getLastVisibleColumn()
{
    vector<KanbanColumnPresenter*> visible{};
    this->getVisibleColumns(visible);

    for(unsigned i=0; i<visible.size(); i++) {
        if(visible[i]->getView()->hasFocus()) {
            return visible[(i-1)%visible.size()];
        }
    }

    return nullptr;
}

void KanbanPresenter::focusToNextVisibleColumn()
{
    if(KanbanColumnPresenter* next=this->getNextVisibleColumn()) {
        next->getView()->setFocus();
    }
}

void KanbanPresenter::focusToLastVisibleColumn()
{
    if(KanbanColumnPresenter* last=this->getLastVisibleColumn()) {
        last->getView()->setFocus();
    }
}

bool KanbanPresenter::moveToVisibleColumn(Note* n, int nextLast)
{
    vector<KanbanColumnPresenter*> visible{};
    this->getVisibleColumns(visible);
    if(visible.size()>1 && n->getTags()->size()) {
        for(unsigned i=0; i<visible.size(); i++) {
            if(visible[i]->getView()->hasFocus()) {
                Tags tags{*n->getTags()};
                vector<const Tag*> currentColumnTags = this->getTagsForColumn(i);
                tags.removeTags(currentColumnTags);
                vector<const Tag*> nextColumnTags = this->getTagsForColumn((i+nextLast)%visible.size());
                tags.addTags(nextColumnTags);

                n->setTags(tags.getTagsPtr());

                // caller to perform refresh
                return true;
            }
        }
    }

    return false;
}

bool KanbanPresenter::moveToNextVisibleColumn(Note* n)
{
    return this->moveToVisibleColumn(n, 1);
}

bool KanbanPresenter::moveToLastVisibleColumn(Note* n)
{
    return this->moveToVisibleColumn(n, -1);
}

} // m8r namespace
