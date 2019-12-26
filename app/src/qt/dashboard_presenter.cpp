/*
 dashboard_presenter.cpp     MindForger thinking notebook

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
#include "dashboard_presenter.h"

namespace m8r {

using namespace std;

DashboardPresenter::DashboardPresenter(DashboardView* view, OrlojPresenter* orloj)
{
    this->view = view;

    doFirstDashboardletPresenter = new OrganizerQuadrantPresenter(
        view->getDoFirstDashboardlet(),
        orloj,
        tr("Do first")
    );
    outlinesDashboardletPresenter = new OutlinesTablePresenter(
        view->getOutlinesDashboardlet(),
        orloj->getMainPresenter()->getHtmlRepresentation()
    );
    navigatorDashboardletPresenter = new NavigatorPresenter(
        view->getNavigatorDashboardlet(),
        this,
        orloj->getMind()->getKnowledgeGraph()
    );
    recentDashboardletPresenter = new RecentNotesTablePresenter(
        view->getRecentDashboardlet(),
        orloj->getMainPresenter()->getHtmlRepresentation()
    );
    tagsDashboardletPresenter = new TagsTablePresenter(
        view->getTagsDashboardlet(),
        orloj->getMainPresenter()->getHtmlRepresentation()
    );
}

DashboardPresenter::~DashboardPresenter()
{
}

void DashboardPresenter::refresh(
        const vector<Outline*>& os,
        const vector<Note*>& ns,
        const map<const Tag*,int>& ts,
        int bytes,
        MindStatistics* stats)
{
    view->getWelcomeDashboardlet()->setHtml(
        QString(
            "<html><body bgcolor='#fff' color='#000'><font color='black'><center>"
            "<br/>"
            "<h1>We are MindForger!</h1>"
            "<br/>"
            "<a href='https://www.mindforger.com'>MindForger</a> is human mind inspired personal "
            "<a href='http://www.mindforger.com/#vision'>knowledge management tool</a>. "
            "It's also open, free, well performing "
            "<a href='http://www.mindforger.com/#floss'>Markdown editor</a> which respects "
            "your privacy and enables security."
            "<ul>"
            "<li>- <a href='https://github.com/dvorka/mindforger-repository/blob/master/memory/mindforger/index.md'>Documentation</a>.</li>"
            "<li>- <a href='https://www.mindforger.com/#download'>Releases</a>, "
            "<a href='https://github.com/dvorka/mindforger'>source code</a> and "
            "<a href='https://github.com/dvorka/mindforger/issues'>bugs</a>.</li>"
            "<li>- <a href='https://twitter.com/mindforger'>Twitter</a>, "
            "<a href='http://www.facebook.com/pages/MindForger/172099806154112'>Facebook</a> and "
            "<a href='https://www.youtube.com/user/MindForgerChannel'>YouTube</a>.</li>"
            "</ul>"
            "<b>Tips</b>:"
            "</center>"
            "<ul>"
            "<li>- Return to this <b>dashboard</b> with <code>Ctrl-Shift-d</code>.</li>"
            "<li>- <b>List</b> notebooks/Markdown files with <code>Ctrl-Shift-o</code>.</li>"
            "<li>- <b>Open</b> any item in table <b>listing</b> with "
            "mouse <b>double-click</b>.</li>"
            "<li>- <b>Edit</b> note with HTML preview <b>double-click</b> or "
            "using <code>Ctrl-e</code>.</li>"
            "<li>- <b>Zoom</b> note HTML preview with <code>Ctrl-</code><b>mouse wheel</b> "
            "or <b>scroll</b>.</li>"
            "</ul>"
            "<center>"
            "<b>Statistics</b>:"
            "<ul>"
            "<li>- <b>" + QString::fromStdString(stringIntFormat(std::to_string(os.size()))) + "</b> notebooks.</li>"
            "<li>- <b>" + QString::fromStdString(stringIntFormat(std::to_string(ns.size()))) + "</b> notes.</li>"
            "<li>- <b>" + QString::fromStdString(stringIntFormat(std::to_string(ts.size()))) + "</b> tags.</li>"
            "<li>- <b>" + QString::fromStdString(stringIntFormat(std::to_string(bytes))) + "</b> bytes.</li>"
            "<li>- Most used notebook: <b>" + QString::fromStdString(stats->mostReadOutline?stats->mostReadOutline->getName():"") + "</b>.</li>"
            "<li>- Most used note: <b>" + QString::fromStdString(stats->mostReadNote?stats->mostReadNote->getName():"") + "</b> in "
            "<b>" + QString::fromStdString(stats->mostReadNote?stats->mostReadNote->getOutline()->getName():"") + "</b>.</li>"
            // TODO RD vs. WR: "<li>- Most written notebook: <b>" + QString::fromStdString(stats->mostWrittenOutline?stats->mostWrittenOutline->getName():"") + "</b>.</li>"
            // TODO RD vs. WR: "<li>- Most written note: <b>" + QString::fromStdString(stats->mostWrittenNote?stats->mostWrittenNote->getName():"") + "</b>.</li>"
            "<li>- Most used tag: <b><span style='background-color: " + QString::fromStdString(stats->mostUsedTag?stats->mostUsedTag->getColor().asHtml():"") + "'>&nbsp;"
            "" + QString::fromStdString(stats->mostUsedTag?stats->mostUsedTag->getName():"") + "&nbsp;</span></b>.</li>"
            // TODO O w/ most Ns
            "</ul>"
            "</center></font></body></html>"
    ));

    vector<Outline*> doFirstOs;
    if(os.size()) {
        for(Outline* o:os) {
            if(o->getUrgency()>2) {
                if(o->getImportance()>2) {
                    doFirstOs.push_back(o);
                }
            }
        }
    }
    doFirstDashboardletPresenter->refresh(doFirstOs, true, true);

    outlinesDashboardletPresenter->refresh(os);
    recentDashboardletPresenter->refresh(ns);
    // IMPROVE: consider showing recent O: navigatorDashboardletPresenter->showInitialView(ns[0]->getOutline());
    navigatorDashboardletPresenter->showInitialView();
    tagsDashboardletPresenter->refresh(ts);
}

} // m8r namespace
