/*
 outline_view_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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

OutlineViewPresenter::OutlineViewPresenter(OutlineViewSplitter* view, OrlojPresenter* orloj)
    : QObject(orloj), currentOutline{nullptr}
{
    this->view = view;
    this->outlineTreePresenter
        = new OutlineTreePresenter(view->getOutlineTree(), orloj->getMainPresenter(), this);
    this->assocLeaderboardPresenter
        = new AssocLeaderboardPresenter(view->getAssocLeaderboard(), orloj);

    QObject::connect(
        view->getNameLabel(), SIGNAL(clicked()),
        orloj, SLOT(slotShowOutlineHeader()));
    QObject::connect(
        view->getOutlineTree(), SIGNAL(signalFromOutlineTreeToOutlines()),
        orloj, SLOT(slotShowOutlines()));
}

OutlineViewPresenter::~OutlineViewPresenter()
{
    if(outlineTreePresenter) delete outlineTreePresenter;
}

void OutlineViewPresenter::refresh(Outline* outline)
{
    outline->makeRead();

    currentOutline = outline;
    view->refreshHeader(outline->getName());
    outlineTreePresenter->refresh(outline);
}

void OutlineViewPresenter::refresh(Note* note)
{
    outlineTreePresenter->refresh(note);
    selectRowByNote(note);
}

void OutlineViewPresenter::insertAndSelect(Note* note)
{
    outlineTreePresenter->insertAndSelect(note);
}

void OutlineViewPresenter::selectRowByNote(const Note* note)
{
    outlineTreePresenter->selectRowByNote(note);
}

} // m8r namespace
