/*
 organizer_presenter.cpp     MindForger thinking notebook

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

void OrganizerPresenter::refresh(const vector<Outline*>& os)
{
    vector<Outline*> doFirstOs;
    vector<Outline*> doSoonOs;
    vector<Outline*> doSometimeOs;
    vector<Outline*> planDedicatedTimeOs;

    if(os.size()) {
        for(Outline* o:os) {
            if(o->getUrgency()>2) {
                if(o->getImportance()>2) {
                    doFirstOs.push_back(o);
                } else {
                    doSoonOs.push_back(o);
                }
            } else {
                if(o->getImportance()>2) {
                    planDedicatedTimeOs.push_back(o);
                } else {
                    if(o->getImportance()>0) {
                        doSometimeOs.push_back(o);
                    }
                }
            }
        }
    }

    doFirstPresenter->refresh(doFirstOs, true, true);
    doSoonPresenter->refresh(doSoonOs, true, false);
    doSometimePresenter->refresh(doSometimeOs, false, false);
    planDedicatedTimePresenter->refresh(planDedicatedTimeOs, false, true);

    view->getDoFirst()->setFocus();
}

} // m8r namespace
