/*
 outline_view_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "outline_view_presenter.h"

namespace m8r {

OutlineViewPresenter::OutlineViewPresenter(OutlineView *view, OrlojPresenter* orloj) : QObject(orloj), currentOutline{nullptr}
{
    this->view = view;
    // IMPROVE stack object
    this->outlineTreePresenter = new OutlineTreePresenter(view->getOutlineTree(), this);

    QObject::connect(view->getTitleLabel(), SIGNAL(clicked()), orloj, SLOT(slotShowOutlineHeader()));
    QObject::connect(view->getOutlineTree(), SIGNAL(signalFromOutlineTreeToOutlines()), orloj, SLOT(slotShowOutlines()));
}

void OutlineViewPresenter::refresh(Outline* outline)
{
    currentOutline = outline;
    view->refreshHeader(outline->getTitle());
    outlineTreePresenter->refresh(outline);
}

void OutlineViewPresenter::refresh(Note* note)
{
    outlineTreePresenter->refresh(note);
}

OutlineViewPresenter::~OutlineViewPresenter()
{
    if(outlineTreePresenter) delete outlineTreePresenter;
}

}
