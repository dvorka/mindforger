/*
 organizer_presenter.cpp     MindForger thinking notebook

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
#include "organizer_presenter.h"

namespace m8r {

using namespace std;

OrganizerPresenter::OrganizerPresenter(OrganizerView* view, OrlojPresenter* orloj)
{
    this->view = view;

    // TODO Notes/Notebooks tagged with "tag"
    doFirstPresenter = new OrganizerQuadrantPresenter(view->getDoFirst(), orloj, tr("Do first"));
    doSoonPresenter = new OrganizerQuadrantPresenter(view->getDoSoon(), orloj, tr("Do soon"));
    doSometimePresenter = new OrganizerQuadrantPresenter(view->getDoSometime(), orloj, tr("Do sometime"));
    planDedicatedTimePresenter = new OrganizerQuadrantPresenter(view->getPlanDedicatedTime(), orloj, tr("Plan dedicated time"));
}

OrganizerPresenter::~OrganizerPresenter()
{
}

void OrganizerPresenter::refresh(
    Organizer* organizer,
    const vector<Note*>& ons,
    const vector<Outline*>& os,
    const vector<Note*>& ns
) {
    MF_DEBUG("Rendering organizer: " << organizer->getName() << "..." << endl);

    QString title{};

    // upper right / do first
    vector<Note*> upperRightNs;
    // upper left / do soon
    vector<Note*> upperLeftNs;
    // lower right / plan dedicated time
    vector<Note*> lowerRightNs;
    // lower left / do sometimes
    vector<Note*> lowerLeftNs;

    if(os.size()) {
        // fill quadrants based on organizer
        if(!organizer || organizer->getKey()==Organizer::KEY_EISENHOWER_MATRIX) {
            // organizer type: Eisenhower matrix
            for(Outline* o:os) {
                if(o->getUrgency()>2) {
                    if(o->getImportance()>2) {
                        upperRightNs.push_back(o->getOutlineDescriptorAsNote());
                    } else {
                        upperLeftNs.push_back(o->getOutlineDescriptorAsNote());
                    }
                } else {
                    if(o->getImportance()>2) {
                        lowerRightNs.push_back(o->getOutlineDescriptorAsNote());
                    } else {
                        if(o->getImportance()>0) {
                            lowerLeftNs.push_back(o->getOutlineDescriptorAsNote());
                        }
                    }
                }
            }

            // set quadrant titles
            title = tr("Do first");
            doFirstPresenter->setTitle(title);
            title = tr("Do soon");
            doSoonPresenter->setTitle(title);
            title = tr("Plan dedicated time");
            planDedicatedTimePresenter->setTitle(title);
            title = tr("Do sometimes");
            doSometimePresenter->setTitle(title);

            doFirstPresenter->refresh(upperRightNs, true, true);
            doSoonPresenter->refresh(upperLeftNs, true, false);
            doSometimePresenter->refresh(lowerLeftNs, false, false);
            planDedicatedTimePresenter->refresh(lowerRightNs, false, true);
        } else {
            // organizer type: custom

            if(Organizer::FilterBy::NOTES == organizer->getFilterBy()) {
                for(Note* n:ns) {
                    if(n->hasTagStrings(organizer->getUpperRightTags())) {
                        upperRightNs.push_back(n);
                    }
                    if(n->hasTagStrings(organizer->getLowerRightTags())) {
                        lowerRightNs.push_back(n);
                    }
                    if(n->hasTagStrings(organizer->getUpperLeftTags())) {
                        upperLeftNs.push_back(n);
                    }
                    if(n->hasTagStrings(organizer->getLowerLeftTags())) {
                        lowerLeftNs.push_back(n);
                    }
                }
            } else if(Organizer::FilterBy::OUTLINES == organizer->getFilterBy()) {
                for(Outline* o:os) {
                    if(o->hasTagStrings(organizer->getUpperRightTags())) {
                        upperRightNs.push_back(o->getOutlineDescriptorAsNote());
                    }
                    if(o->hasTagStrings(organizer->getLowerRightTags())) {
                        lowerRightNs.push_back(o->getOutlineDescriptorAsNote());
                    }
                    if(o->hasTagStrings(organizer->getUpperLeftTags())) {
                        upperLeftNs.push_back(o->getOutlineDescriptorAsNote());
                    }
                    if(o->hasTagStrings(organizer->getLowerLeftTags())) {
                        lowerLeftNs.push_back(o->getOutlineDescriptorAsNote());
                    }
                }
            } else if(Organizer::FilterBy::OUTLINES_NOTES == organizer->getFilterBy()) {
                for(Note* n:ons) {
                    if(n->hasTagStrings(organizer->getUpperRightTags())) {
                        upperRightNs.push_back(n);
                    }
                    if(n->hasTagStrings(organizer->getLowerRightTags())) {
                        lowerRightNs.push_back(n);
                    }
                    if(n->hasTagStrings(organizer->getUpperLeftTags())) {
                        upperLeftNs.push_back(n);
                    }
                    if(n->hasTagStrings(organizer->getLowerLeftTags())) {
                        lowerLeftNs.push_back(n);
                    }
                }
            }

            // set quadrant titles
            title = QString::fromStdString(Organizer::tagsToString(organizer->getUpperRightTags()));
            doFirstPresenter->setTitle(title);
            title = QString::fromStdString(Organizer::tagsToString(organizer->getUpperLeftTags()));
            doSoonPresenter->setTitle(title);
            title = QString::fromStdString(Organizer::tagsToString(organizer->getLowerRightTags()));
            planDedicatedTimePresenter->setTitle(title);
            title = QString::fromStdString(Organizer::tagsToString(organizer->getLowerLeftTags()));
            doSometimePresenter->setTitle(title);

            doFirstPresenter->refresh(upperRightNs, true, true);
            doSoonPresenter->refresh(upperLeftNs, true, false);
            doSometimePresenter->refresh(lowerLeftNs, false, false);
            planDedicatedTimePresenter->refresh(lowerRightNs, false, true);
        }
    }

    view->getDoFirst()->setFocus();
}

} // m8r namespace
