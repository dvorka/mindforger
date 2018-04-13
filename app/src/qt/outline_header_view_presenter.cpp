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
        = new HtmlOutlineRepresentation{orloj->getMind()->ontology()};

    QObject::connect(
        view, SIGNAL(signalMouseDoubleClickEvent()),
        this, SLOT(slotEditOutlineHeader()));
}

OutlineHeaderViewPresenter::~OutlineHeaderViewPresenter()
{
}

void OutlineHeaderViewPresenter::refresh(Outline* outline)
{
    currentOutline = outline;

    // TODO build HTML using Outline > QString build documented in QtDoc for QString
    QString html(MF_HTML_HEAD);
    html += QString::fromUtf8(" style='color: ");
    html += LookAndFeels::getInstance().getTextColor();
    html += QString::fromUtf8("; background-color: ");
    html += LookAndFeels::getInstance().getEditorBackgroundColor();
    html += QString::fromUtf8("; font-size: 90%;'>");
    // table
    html += QString::fromUtf8("<table width='100%'><tr>");
    html += QString::fromUtf8("<td>");
    html += QString::fromUtf8("<h2>");
    html += QString::fromStdString(outline->getName());
    html += QString::fromUtf8("</h2>");
    outlineTypeToHtml(outline->getType(), html);
    html += QString::fromUtf8("&nbsp;");
    // IMPROVE show rs/ws/... only if it's MF repository (hide it otherwise) + configuration allows to hide it in all cases
    outlineMetadataToHtml(outline, html);
    html += QString::fromUtf8("</td>");
    html += QString::fromUtf8("<td style='width: 50px;'>");
    if(outline->getProgress()) {
        html += QString::fromUtf8("<h1>");
        html += QString::number(outline->getProgress());
        html += QString::fromUtf8("%&nbsp;&nbsp;</h1>");
    }
    html += QString::fromUtf8("</td>");
    html += QString::fromUtf8("<td style='width: 50px;'><table style='font-size: 100%;'><tr>");
    if(outline->getImportance() || outline->getUrgency()) {
        if(outline->getImportance() > 0) {
            for(int i=0; i<=4; i++) {
                html += QString::fromUtf8("<td>");
                if(outline->getImportance()>i) {
                    html += QChar(9733);
                } else {
                    html += QChar(9734);
                }
                html += QString::fromUtf8("</td>");
            }
        } else {
            for(int i=0; i<5; i++) {
                html += QString::fromUtf8("<td>");
                html.append(QChar(9734));
                html += QString::fromUtf8("</td>");
            }
        }
        html += QString::fromUtf8("</tr>");
        html += QString::fromUtf8("<tr>");
        if(outline->getUrgency()>0) {
            for(int i=0; i<=4; i++) {
                if(outline->getUrgency()>i) {
                    html += QString::fromUtf8("<td>");
                    html += QChar(0x29D7);
                    html += QString::fromUtf8("</td>");
                } else {
                    html += QString::fromUtf8("<td>");
                    html += QChar(0x29D6);
                    html += QString::fromUtf8("</td>");
                }
            }
        } else {
            for(int i=0; i<5; i++) {
                html += QString::fromUtf8("<td>");
                html.append(QChar(0x29D6));
                html += QString::fromUtf8("</td>");
            }
        }
    }
    html += QString::fromUtf8("</tr></table>");
    html += QString::fromUtf8("</td>");
    html += QString::fromUtf8("</tr></table>");



    html += QString::fromUtf8("<br/>");
    tagsToHtml(outline->getTags(), html);

    html += QString::fromUtf8("<pre>");
    html += QString::fromStdString(outline->getDescriptionAsString());
    html += QString::fromUtf8("</pre>");

    html += QString::fromStdString(MF_HTML_TAIL);
    view->setHtml(html);

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
