/*
 outline_header_view_presenter.cpp     MindForger thinking notebook

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
#include "outline_header_view_presenter.h"

#include "look_n_feel.h"

namespace m8r {

using namespace std;

OutlineHeaderViewPresenter::OutlineHeaderViewPresenter(
        OutlineHeaderView* view,
        OrlojPresenter* orloj)
{
    this->view = view;
    this->orloj = orloj;

    this->htmlRepresentation
        = orloj->getMainPresenter()->getHtmlRepresentation();

    // IMPORTANT: pre-allocate string using reserve() to ensure good append performance
    html = string{};
    html.reserve(10000);

#ifdef MF_QT_WEB_ENGINE
    QObject::connect(
        view->getViever()->getPage(), SIGNAL(signalLinkClicked(QUrl)),
        this, SLOT(slotLinkClicked(QUrl)));
#else
    QObject::connect(
        view->getViever(), SIGNAL(linkClicked(QUrl)),
        this, SLOT(slotLinkClicked(QUrl)));
#endif
    QObject::connect(
        view->getViever(), SIGNAL(signalMouseDoubleClickEvent()),
        this, SLOT(slotEditOutlineHeader()));
    QObject::connect(
        view, SIGNAL(signalOpenEditor()),
        this, SLOT(slotEditOutlineHeader()));
    QObject::connect(
        view->getViever(), SIGNAL(signalFromViewOutlineHeaderToOutlines()),
        orloj, SLOT(slotShowOutlines()));
}

OutlineHeaderViewPresenter::~OutlineHeaderViewPresenter()
{
}

void OutlineHeaderViewPresenter::refresh(Outline* outline)
{
    currentOutline = outline;

    htmlRepresentation->toHeader(outline, &html, false, Configuration::getInstance().isAutolinking());
    view->setHtml(QString::fromStdString(html));

    // leaderboard
    orloj->getMind()->associate();
}

void OutlineHeaderViewPresenter::slotLinkClicked(const QUrl& url)
{
    orloj->getMainPresenter()->handleNoteViewLinkClicked(url);
}

void OutlineHeaderViewPresenter::slotEditOutlineHeader()
{
    orloj->showFacetOutlineHeaderEdit(currentOutline);
}

void OutlineHeaderViewPresenter::slotRefreshHeaderLeaderboardByValue(AssociatedNotes* associations)
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE_HEADER)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_VIEW_OUTLINE)
         ||
       orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) // leaderboard for edit @ view
    {
        orloj->getOutlineView()->getAssocLeaderboard()->refresh(associations);
        delete associations;
    }
}

} // m8r namespace
