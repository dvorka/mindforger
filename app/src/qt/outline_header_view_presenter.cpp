/*
 outline_header_view_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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

OutlineHeaderViewPresenter::OutlineHeaderViewPresenter(OutlineHeaderView *view, OrlojPresenter* orloj)
{
    this->view = view;
    this->orloj = orloj;

    this->htmlRepresentation
        = orloj->getMainWindow()->getHtmlRepresentation();

    // IMPORTANT: pre-allocate string using reserve() to ensure good append performance
    html = string{};
    html.reserve(10000);

#ifdef MF_QT_WEB_ENGINE
    QObject::connect(view->getPage(), SIGNAL(signalLinkClicked(QUrl)), this, SLOT(slotLinkClicked(QUrl)));
#else
    QObject::connect(view, SIGNAL(linkClicked(QUrl)), this, SLOT(slotLinkClicked(QUrl)));
#endif
    QObject::connect(view, SIGNAL(signalMouseDoubleClickEvent()), this, SLOT(slotEditOutlineHeader()));
}

OutlineHeaderViewPresenter::~OutlineHeaderViewPresenter()
{
}

void OutlineHeaderViewPresenter::refresh(Outline* outline)
{
    currentOutline = outline;

    htmlRepresentation->toHeader(outline,&html);

    view->setHtml(QString::fromStdString(html));

    // leaderboard
    std::vector<std::pair<Note*,float>> associatedNotesLeaderboard;
    shared_future<bool> f = orloj->getMind()->getAssociatedNotes(currentOutline, associatedNotesLeaderboard);
    if(f.wait_for(chrono::microseconds(0)) == future_status::ready) {
        if(f.get()) {
            orloj->getOutlineView()->getAssocLeaderboard()->refresh(associatedNotesLeaderboard);
        } else {
            orloj->getOutlineView()->getAssocLeaderboard()->getView()->setVisible(false);
        }
    } else {
        orloj->getOutlineView()->getAssocLeaderboard()->getView()->setVisible(false);
        // ask notifications distributor to repaint leaderboard later
        AsyncTaskNotificationsDistributor::Task* task = new AsyncTaskNotificationsDistributor::Task{f,AsyncTaskNotificationsDistributor::TaskType::NOTE_ASSOCIATIONS};
        task->setOutline(currentOutline);
        orloj->getMainWindow()->getDistributor()->add(task);
    }
}

void OutlineHeaderViewPresenter::slotLinkClicked(const QUrl& url)
{
    orloj->getMainWindow()->handleNoteViewLinkClicked(url);
}

void OutlineHeaderViewPresenter::slotEditOutlineHeader()
{
    orloj->showFacetOutlineHeaderEdit(currentOutline);
}

void OutlineHeaderViewPresenter::slotRefreshHeaderLeaderboardByValue(vector<pair<Note*,float>>* associations)
{
    if(orloj->isFacetActive(OrlojPresenterFacets::FACET_EDIT_OUTLINE_HEADER)) {
        orloj->getOutlineView()->getAssocLeaderboard()->refresh(*associations);
    }
    delete associations;
}

} // m8r namespace
