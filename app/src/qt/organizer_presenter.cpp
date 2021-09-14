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

void OrganizerPresenter::refresh(Organizer* organizer, const vector<Outline*>& os)
{
    MF_DEBUG("Rendering organizer: " << organizer->getName() << "..." << endl);

    // upper right / do first
    vector<Outline*> upperRightOs;
    // upper left / do soon
    vector<Outline*> upperLeftOs;
    // lower right / plan dedicated time
    vector<Outline*> lowerRightOs;
    // lower left / do sometimes
    vector<Outline*> lowerLeftOs;

    if(os.size()) {
        // fill quadrants based on organizer
        if(!organizer || organizer->getKey()==Organizer::KEY_EISENHOWER_MATRIX) {
            // organizer type: Eisenhower matrix
            for(Outline* o:os) {
                if(o->getUrgency()>2) {
                    if(o->getImportance()>2) {
                        upperRightOs.push_back(o);
                    } else {
                        upperLeftOs.push_back(o);
                    }
                } else {
                    if(o->getImportance()>2) {
                        lowerRightOs.push_back(o);
                    } else {
                        if(o->getImportance()>0) {
                            lowerLeftOs.push_back(o);
                        }
                    }
                }
            }
        } else {
            // organizer type: custom
            for(Outline* o:os) {
                if(o->hasTagStrings(organizer->getUpperRightTags())) {
                    upperRightOs.push_back(o);
                }
                if(o->hasTagStrings(organizer->getLowerRightTags())) {
                    lowerRightOs.push_back(o);
                }
                if(o->hasTagStrings(organizer->getUpperLeftTags())) {
                    upperLeftOs.push_back(o);
                }
                if(o->hasTagStrings(organizer->getLowerLeftTags())) {
                    lowerLeftOs.push_back(o);
                }
            }
        }
    }

    doFirstPresenter->refresh(upperRightOs, true, true);
    doSoonPresenter->refresh(upperLeftOs, true, false);
    doSometimePresenter->refresh(lowerLeftOs, false, false);
    planDedicatedTimePresenter->refresh(lowerRightOs, false, true);

    view->getDoFirst()->setFocus();
}

} // m8r namespace
