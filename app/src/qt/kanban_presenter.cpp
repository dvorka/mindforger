/*
 kanban_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
    const vector<Note*>& ns,
    bool setFocus
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

    columns[0]->refresh(upperLeftNs, true, false, !kanban->getOutlineScope().size());
    columns[1]->refresh(upperRightNs, true, true, !kanban->getOutlineScope().size());
    columns[2]->refresh(lowerLeftNs, false, false, !kanban->getOutlineScope().size());
    columns[3]->refresh(lowerRightNs, false, true, !kanban->getOutlineScope().size());

    // IF at least one column with a tag exists,
    // THEN show only column(s) with tags,
    // ELSE show all EMPTY columns
    for(auto c:columns) {
        c->getView()->setVisible(true);
    }
    if(kanban->getUpperLeftTags().size()
       || kanban->getUpperRightTags().size()
       || kanban->getLowerLeftTags().size()
       || kanban->getLowerRightTags().size())
    {
        vector<KanbanColumnPresenter*> visibleColumns{};

        if(!kanban->getUpperLeftTags().size()) {
            columns[0]->getView()->setVisible(false);
        } else {
            visibleColumns.push_back(columns[0]);
        }
        if(!kanban->getUpperRightTags().size()) {
            columns[1]->getView()->setVisible(false);
        } else {
            visibleColumns.push_back(columns[1]);
        }
        if(!kanban->getLowerLeftTags().size()) {
            columns[2]->getView()->setVisible(false);
        } else {
            visibleColumns.push_back(columns[2]);
        }
        if(!kanban->getLowerRightTags().size()) {
            columns[3]->getView()->setVisible(false);
        } else {
            visibleColumns.push_back(columns[3]);
        }

        if(setFocus && visibleColumns.size()) {
            visibleColumns[0]->getView()->setFocus();
        }
    } else {
        if(setFocus) {
            columns[0]->getView()->setFocus();
        }
    }
}

void KanbanPresenter::getVisibleColumns(vector<KanbanColumnPresenter*>& visible, vector<int>& offsets)
{
    for(unsigned c=0; c<columns.size(); c++) {
        if(columns[c]->getView()->isVisible()) {
            visible.push_back(columns[c]);
            offsets.push_back(c);
        }
    }
}

KanbanColumnPresenter* KanbanPresenter::getNextVisibleColumn()
{
    vector<KanbanColumnPresenter*> visible{};
    vector<int> offsets{};
    this->getVisibleColumns(visible, offsets);

    for(unsigned i=0; i<visible.size(); i++) {
        if(visible[i]->getView()->hasFocus()) {
            return visible[(i+1)%visible.size()];
        }
    }

    return nullptr;
}

KanbanColumnPresenter* KanbanPresenter::getPreviousVisibleColumn()
{
    vector<KanbanColumnPresenter*> visible{};
    vector<int> offsets{};
    this->getVisibleColumns(visible, offsets);

    for(int i=0; ((unsigned)i)<visible.size(); i++) {
        if(visible[i]->getView()->hasFocus()) {
            int previousIdx = pythonModulo(i-1, visible.size());
            MF_DEBUG("Kanban: previous visible " << visible.size() << " column " << i << " >> " << previousIdx << endl);
            return visible[previousIdx];
        }
    }

    return nullptr;
}

KanbanColumnPresenter* KanbanPresenter::focusToNextVisibleColumn()
{
    if(KanbanColumnPresenter* next=this->getNextVisibleColumn()) {
        next->getView()->setFocus();
        return next;
    }
    return nullptr;
}

KanbanColumnPresenter* KanbanPresenter::focusToPreviousVisibleColumn()
{
    if(KanbanColumnPresenter* previous=this->getPreviousVisibleColumn()) {
        previous->getView()->setFocus();
        return previous;
    }
    return nullptr;
}

KanbanColumnPresenter* KanbanPresenter::moveToVisibleColumn(Note* n, int nextPrevious)
{
    vector<KanbanColumnPresenter*> visible{};
    vector<int> columnsOffsets{};
    this->getVisibleColumns(visible, columnsOffsets);
    if(visible.size()>1 && n->getTags()->size()) {
        for(unsigned i=0; i<visible.size(); i++) {
            if(visible[i]->getView()->hasFocus()) {
                int currentColumnsOffset = columnsOffsets[i];
                // next
                KanbanColumnPresenter* nextVisibleColumnPresenter =
                        nextPrevious > 0
                        ? this->getNextVisibleColumn()
                        : this->getPreviousVisibleColumn();
                int nextColumnsOffset = (currentColumnsOffset+nextPrevious)%columns.size();
                for(unsigned c=0; c<visible.size(); c++) {
                    if(nextVisibleColumnPresenter == visible[c]) {
                        nextColumnsOffset = columnsOffsets[c];
                    }
                }
                MF_DEBUG("Moving N from column "  << currentColumnsOffset << " to " << nextColumnsOffset << endl);

                Tags tags{*n->getTags()};
                vector<const Tag*> currentColumnTags = this->getTagsForColumn(
                    currentColumnsOffset
                );
                tags.removeTags(currentColumnTags);
                vector<const Tag*> nextColumnTags = this->getTagsForColumn(
                    nextColumnsOffset
                );
                tags.addTags(nextColumnTags);

                if(Outline::isOutlineDescriptorNote(n)) {
                    n->getOutline()->setTags(tags.getTagsPtr());
                } else {
                    n->setTags(tags.getTagsPtr());
                }

                // caller to persist N and refresh of source and target view columns
                n->makeModified();

                return columns[nextColumnsOffset];
            }
        }
    }

    return nullptr;
}

KanbanColumnPresenter* KanbanPresenter::moveToNextVisibleColumn(Note* n)
{
    return this->moveToVisibleColumn(n, 1);
}

KanbanColumnPresenter* KanbanPresenter::moveToPreviousVisibleColumn(Note* n)
{
    return this->moveToVisibleColumn(n, -1);
}

} // m8r namespace
