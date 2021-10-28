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
    MF_DEBUG("Rendering organizer: " << kanban->getName() << "..." << endl);
    MF_DEBUG("  filter (notes==" << Organizer::FilterBy::NOTES << "): " << kanban->getFilterBy() << endl);

    this->kanban = kanban;




    // TODO this code must go to lib/
    // TODO - unit test to be created
    // app/ to just call the library

    kanban->makeModified();

    QString title{};

    // upper right / do first
    vector<Note*> upperRightNs{};
    // upper left / do soon
    vector<Note*> upperLeftNs{};
    // lower right / plan dedicated time
    vector<Note*> lowerRightNs{};
    // lower left / do sometimes
    vector<Note*> lowerLeftNs{};

    if(os.size()) {
        // organizer type: custom
        if(Organizer::FilterBy::NOTES == kanban->getFilterBy()) {
            Outline* scopeKanban{nullptr};

            if(kanban->getOutlineScope().size()) {
                for(auto* o:os) {
                    if(o->getKey() == kanban->getOutlineScope()) {
                        scopeKanban= o;
                        break;
                    }
                }
            }

            // scoped vs. all
            const vector<Note*>& notes{scopeKanban?scopeKanban->getNotes():ns};

            for(Note* n:notes) {
                if(n->hasTagStrings(kanban->getUpperRightTags())) {
                    upperRightNs.push_back(n);
                }
                if(n->hasTagStrings(kanban->getLowerRightTags())) {
                    lowerRightNs.push_back(n);
                }
                if(n->hasTagStrings(kanban->getUpperLeftTags())) {
                    upperLeftNs.push_back(n);
                }
                if(n->hasTagStrings(kanban->getLowerLeftTags())) {
                    lowerLeftNs.push_back(n);
                }
            }
        } else if(Organizer::FilterBy::OUTLINES == kanban->getFilterBy()) {
            for(Outline* o:os) {
                if(o->hasTagStrings(kanban->getUpperRightTags())) {
                    upperRightNs.push_back(o->getOutlineDescriptorAsNote());
                }
                if(o->hasTagStrings(kanban->getLowerRightTags())) {
                    lowerRightNs.push_back(o->getOutlineDescriptorAsNote());
                }
                if(o->hasTagStrings(kanban->getUpperLeftTags())) {
                    upperLeftNs.push_back(o->getOutlineDescriptorAsNote());
                }
                if(o->hasTagStrings(kanban->getLowerLeftTags())) {
                    lowerLeftNs.push_back(o->getOutlineDescriptorAsNote());
                }
            }
        } else if(Organizer::FilterBy::OUTLINES_NOTES == kanban->getFilterBy()) {
            for(Note* n:ons) {
                if(n->hasTagStrings(kanban->getUpperRightTags())) {
                    upperRightNs.push_back(n);
                }
                if(n->hasTagStrings(kanban->getLowerRightTags())) {
                    lowerRightNs.push_back(n);
                }
                if(n->hasTagStrings(kanban->getUpperLeftTags())) {
                    upperLeftNs.push_back(n);
                }
                if(n->hasTagStrings(kanban->getLowerLeftTags())) {
                    lowerLeftNs.push_back(n);
                }
            }
        }

        // set quadrant titles
        title = QString::fromStdString(Organizer::tagsToString(kanban->getUpperRightTags(), false));
        columns[0]->setTitle(title);
        title = QString::fromStdString(Organizer::tagsToString(kanban->getUpperLeftTags(), false));
        columns[1]->setTitle(title);
        title = QString::fromStdString(Organizer::tagsToString(kanban->getLowerRightTags(), false));
        columns[2]->setTitle(title);
        title = QString::fromStdString(Organizer::tagsToString(kanban->getLowerLeftTags(), false));
        columns[3]->setTitle(title);

        Memory::sortByRead(upperRightNs);
        Memory::sortByRead(upperLeftNs);
        Memory::sortByRead(lowerLeftNs);
        Memory::sortByRead(lowerRightNs);

        columns[0]->refresh(upperRightNs, true, true);
        columns[1]->refresh(upperLeftNs, true, false);
        columns[2]->refresh(lowerLeftNs, false, false);
        columns[3]->refresh(lowerRightNs, false, true);
    }

    view->getColumn(0)->setFocus();
}

} // m8r namespace
