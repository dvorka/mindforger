/*
 organizer_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "organizer_presenter.h"

namespace m8r {

using namespace std;

OrganizerPresenter::OrganizerPresenter(OrganizerView* view, OrlojPresenter* orloj)
    : view{view},
      orloj{orloj}
{
    doFirstPresenter = new OrganizerQuadrantPresenter(
        view->getDoFirst(), orloj, tr(TITLE_DO_FIRST)
    );
    doSoonPresenter = new OrganizerQuadrantPresenter(
        view->getDoSoon(), orloj, tr(TITLE_DO_SOON)
    );
    doSometimePresenter = new OrganizerQuadrantPresenter(
        view->getDoSometime(), orloj, tr(TITLE_DO_SOMETIMES)
    );
    planDedicatedTimePresenter = new OrganizerQuadrantPresenter(
        view->getPlanDedicatedTime(), orloj, tr(TITLE_PLAN_DEDICATED_TIME)
    );

    orderedQuadrants = vector<OrganizerQuadrantPresenter*>{
        doSoonPresenter,
        doFirstPresenter,
        doSometimePresenter,
        planDedicatedTimePresenter
    };
}

OrganizerPresenter::~OrganizerPresenter()
{
}

vector<const Tag*> OrganizerPresenter::getTagsForQuadrant(int columnNumber)
{
    static vector<const Tag*> empty{};

    if(this->organizer) {
        return organizer->getTagsForQuadrant(
            columnNumber, orloj->getMind()->getOntology()
        );
    }

    return empty;
}

void OrganizerPresenter::refresh(
    Organizer* organizer,
    const vector<Note*>& ons,
    const vector<Outline*>& os,
    const vector<Note*>& ns,
    bool setFocus
) {
    MF_DEBUG("Rendering organizer: " << organizer->getName() << "..." << endl);

    this->organizer = organizer;

    // upper right / do first
    vector<Note*> upperRightNs{};
    // upper left / do soon
    vector<Note*> upperLeftNs{};
    // lower right / plan dedicated time
    vector<Note*> lowerRightNs{};
    // lower left / do sometimes
    vector<Note*> lowerLeftNs{};

    Outline::organizeToEisenhowerMatrix(
        this->organizer, ons, os, ns, upperLeftNs, upperRightNs, lowerLeftNs, lowerRightNs
    );

    // set quadrant titles
    QString title{};
    if(!organizer || organizer->getKey()==EisenhowerMatrix::KEY_EISENHOWER_MATRIX) {
        title = tr(TITLE_DO_FIRST);
        doFirstPresenter->setTitle(title);
        title = tr(TITLE_DO_SOON);
        doSoonPresenter->setTitle(title);
        title = tr(TITLE_PLAN_DEDICATED_TIME);
        planDedicatedTimePresenter->setTitle(title);
        title = tr(TITLE_DO_SOMETIMES);
        doSometimePresenter->setTitle(title);
    } else {
        title = QString::fromStdString(
            Tags::tagsToString(organizer->getUpperRightTags(), false)
        );
        doFirstPresenter->setTitle(title);
        title = QString::fromStdString(
            Tags::tagsToString(organizer->getUpperLeftTags(), false)
        );
        doSoonPresenter->setTitle(title);
        title = QString::fromStdString(
            Tags::tagsToString(organizer->getLowerRightTags(), false)
        );
        planDedicatedTimePresenter->setTitle(title);
        title = QString::fromStdString(
            Tags::tagsToString(organizer->getLowerLeftTags(), false)
        );
        doSometimePresenter->setTitle(title);
    }

    doFirstPresenter->refresh(upperRightNs, true, true, organizer->getOutlineScope().size());
    doSoonPresenter->refresh(upperLeftNs, true, false, organizer->getOutlineScope().size());
    doSometimePresenter->refresh(lowerLeftNs, false, false, organizer->getOutlineScope().size());
    planDedicatedTimePresenter->refresh(lowerRightNs, false, true, organizer->getOutlineScope().size());

    if(setFocus) {
        for(auto quadrantPresenter: this->orderedQuadrants) {
            if(quadrantPresenter->getModel()->rowCount() > 0) {
                quadrantPresenter->getView()->setFocus();
                quadrantPresenter->getView()->setCurrentIndex(
                    quadrantPresenter->getModel()->index(0, 0)
                );
                break;
            }
        }
    }
}

void OrganizerPresenter::focusAndSelectPreviouslySelectedRow(
    OrganizerQuadrantView* view
) {
    view->setFocus();
    view->setCurrentIndex(
        view->model()->index(
            view->currentIndex().row(), 0
        )
    );
}

void OrganizerPresenter::focusToNextVisibleQuadrant()
{
    for(unsigned i=0; i<orderedQuadrants.size(); i++) {
        if(orderedQuadrants[i]->getView()->hasFocus()) {
            if(orderedQuadrants[i]->getView()->hasFocus()) {
                // find next VISIBLE quadrant (stop @ same i.e. <=> 1 quadrant visible)
                unsigned v=i;
                do {
                    int next = (v+1)%orderedQuadrants.size();
                    if(orderedQuadrants[next]->getModel()->rowCount() > 0) {
                        orderedQuadrants[i]->getView()->clearSelection();
                        focusAndSelectPreviouslySelectedRow(orderedQuadrants[next]->getView());
                        return;
                    }
                    v++;
                } while(orderedQuadrants[i] != orderedQuadrants[v]);
                // ^ there is only 1 quadrant > focus can stay where it is
            }
        }
    }
 }

void OrganizerPresenter::focusToPreviousVisibleQuadrant()
{
    for(unsigned i=0; i<orderedQuadrants.size(); i++) {
        if(orderedQuadrants[i]->getView()->hasFocus()) {
            // find previous VISIBLE quadrant (stop @ same i.e. <=> 1 quadrant visible)
            unsigned v=i;
            do {
                int next = (v-1)%orderedQuadrants.size();
                if(orderedQuadrants[next]->getModel()->rowCount() > 0) {
                    orderedQuadrants[i]->getView()->clearSelection();
                    focusAndSelectPreviouslySelectedRow(orderedQuadrants[next]->getView());
                    return;
                }
                v--;
            } while(orderedQuadrants[i] != orderedQuadrants[v]);
            // ^ there is only 1 quadrant > focus can stay where it is
        }
    }
}

OrganizerQuadrantPresenter* OrganizerPresenter::moveToVisibleQuadrant(Note *n, int nextPrevious)
{
    if(n->getTags()->size()) {
        for(unsigned i=0; i<orderedQuadrants.size(); i++) {
            if(orderedQuadrants[i]->getView()->hasFocus()) {
                int currentColumnsOffset = i;
                int nextColumnsOffset = (currentColumnsOffset+nextPrevious)%orderedQuadrants.size();

#ifdef DO_MF_DEBUG
                MF_DEBUG("Moving N from quadrant "  << currentColumnsOffset << " to " << nextColumnsOffset << endl);
                MF_DEBUG("Note tags:\n");
                for(auto nt:*n->getTags()) {
                    MF_DEBUG("  " << nt->getName() << endl);
                }
                MF_DEBUG("Current column tags:\n");
                for(auto nt:this->getTagsForQuadrant(currentColumnsOffset)) {
                    MF_DEBUG("  " << nt->getName() << endl);
                }
                MF_DEBUG("Next column tags:\n");
                for(auto nt:this->getTagsForQuadrant(nextColumnsOffset)) {
                    MF_DEBUG("  " << nt->getName() << endl);
                }
#endif

                Tags tags{*n->getTags()};
                vector<const Tag*> currentColumnTags = this->getTagsForQuadrant(
                    currentColumnsOffset
                );
                tags.removeTags(currentColumnTags);
                vector<const Tag*> nextColumnTags = this->getTagsForQuadrant(
                    nextColumnsOffset
                );
                tags.addTags(nextColumnTags);

#ifdef DO_MF_DEBUG
                MF_DEBUG("NEW Note tags:\n");
                for(auto nt:*tags.getTagsPtr()) {
                    MF_DEBUG("  " << nt->getName() << endl);
                }
#endif

                if(Outline::isOutlineDescriptorNote(n)) {
                    n->getOutline()->setTags(tags.getTagsPtr());
                } else {
                    n->setTags(tags.getTagsPtr());
                }

                // caller to persist N and refresh of source and target view columns
                n->makeModified();

                return orderedQuadrants[nextColumnsOffset];
            }
        }
    }

    return nullptr;
}

OrganizerQuadrantPresenter* OrganizerPresenter::moveToNextVisibleQuadrant(Note* n)
{
    return this->moveToVisibleQuadrant(n, 1);
}

OrganizerQuadrantPresenter* OrganizerPresenter::moveToPreviousVisibleQuadrant(Note* n)
{
    return this->moveToVisibleQuadrant(n, -1);
}

} // m8r namespace
