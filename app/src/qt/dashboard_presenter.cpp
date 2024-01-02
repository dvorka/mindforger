/*
 dashboard_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
    : config(Configuration::getInstance())
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
    // IMPROVE set size based on system resolution/zoom
    view->getWelcomeDashboardlet()->setHtml(
        QString(
            "<html><head>"
            "<style>"
            "body { font-size: 14px; }"
            "</style>"
            "<body><center><div>&nbsp;</div>"
            "<h1>We are <a href='https://www.mindforger.com'>MindForger</a>!</h1>"
            "MindForger is personal "
            "<a href='http://www.mindforger.com/#vision'>thinking notebook</a> "
            "and <a href='http://www.mindforger.com/#features'>Markdown editor</a>:"
            "<ul>"
            "<li><a href='https://github.com/dvorka/mindforger-repository/blob/master/memory/mindforger/index.md'>Documentation</a>.</li>"
            "<li><a href='https://www.mindforger.com/#download'>Releases</a>, "
            "<a href='https://github.com/dvorka/mindforger/issues'>bugs</a> and "
            "<a href='https://github.com/dvorka/mindforger'>source code</a>.</li>"
            "<li><a href='https://twitter.com/mindforger'>Twitter</a>, "
            "<a href='http://www.facebook.com/pages/MindForger/172099806154112'>Facebook</a> and "
            "<a href='https://www.youtube.com/user/MindForgerChannel'>YouTube</a>.</li>"
            "</ul>"
            "<b>Tips</b>:"
            "<ul>"
            "<li>Return to this <b>dashboard</b> with <code>Ctrl-Shift-d</code>.</li>"
            "<li><b>List</b> notebooks/Markdown files with <code>Ctrl-Shift-o</code>.</li>"
            "<li><b>Open</b> item in a table <b>listing</b> with "
            "<b>double-click</b> or <code>ENTER</code>.</li>"
            "<li><b>Edit</b> note with HTML preview <b>double-click</b> or "
            "<code>Ctrl-e</code>.</li>"
            "<li><b>Zoom</b> note HTML preview with <code>Ctrl-</code><b>mouse wheel</b> "
            "or <b>scroll</b>.</li>"
            "</ul>"
            "<b>Statistics</b>:"
            "<ul>"
            "<li><b>" + stringFormatIntAsUs(os.size()) + "</b> notebooks, "
            "<b>" + stringFormatIntAsUs(ns.size()) + "</b> notes, "
            "<b>" + stringFormatIntAsUs(ts.size()) + "</b> tags and "
            "<b>" + stringFormatIntAsUs(bytes) + "</b> bytes.</li>"
            "<li>Most used notebook: <b>" + QString::fromStdString(stats->mostReadOutline?stats->mostReadOutline->getName():"") + "</b>.</li>"
            "<li>Most used note: <b>" + QString::fromStdString(stats->mostReadNote?stats->mostReadNote->getName():"") + "</b> in "
            "<b>" + QString::fromStdString(stats->mostReadNote?stats->mostReadNote->getOutline()->getName():"") + "</b>.</li>"
            // TODO RD vs. WR: "<li>- Most written notebook: <b>" + QString::fromStdString(stats->mostWrittenOutline?stats->mostWrittenOutline->getName():"") + "</b>.</li>"
            // TODO RD vs. WR: "<li>- Most written note: <b>" + QString::fromStdString(stats->mostWrittenNote?stats->mostWrittenNote->getName():"") + "</b>.</li>"
            "<li>Most used tag: <b><span style='background-color: " + QString::fromStdString(stats->mostUsedTag?stats->mostUsedTag->getColor().asHtml():"") + "'>&nbsp;"
            "" + QString::fromStdString(stats->mostUsedTag?stats->mostUsedTag->getName():"") + "&nbsp;</span></b>.</li>"
            // TODO O w/ most Ns
            "</ul>"
            "</center></body></html>"
    ));

    vector<Note*> doFirstOs;
    if(os.size()) {
        for(Outline* o:os) {
            if(o->getUrgency()>2) {
                if(o->getImportance()>2) {
                    doFirstOs.push_back(o->getOutlineDescriptorAsNote());
                }
            }
        }
    }
    doFirstDashboardletPresenter->refresh(doFirstOs, true, true, true);

    outlinesDashboardletPresenter->refresh(os);
    recentDashboardletPresenter->refresh(ns);
    // IMPROVE: consider showing recent O: navigatorDashboardletPresenter->showInitialView(ns[0]->getOutline());
    navigatorDashboardletPresenter->showInitialView();
    tagsDashboardletPresenter->refresh(ts);

    view->setMindForgerMode(
        doFirstOs.size() > 0
          &&
        config.getActiveRepository()->getType()
         ==
        Repository::RepositoryType::MINDFORGER);

    view->getRecentDashboardlet()->setFocus();
}

} // m8r namespace
