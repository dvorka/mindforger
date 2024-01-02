/*
 orloj_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "orloj_view.h"

namespace m8r {

using namespace std;

OrlojView::OrlojView(QWidget* parent)
    : QSplitter(Qt::Horizontal, parent)
{
    dashboard = new DashboardView(this);
    addWidget(dashboard);

    organizersTable = new OrganizersTableView(this);
    addWidget(organizersTable);

    organizer = new OrganizerView(this);
    addWidget(organizer);

    kanban = new KanbanView(4, this);
    addWidget(kanban);

    tagCloud = new TagsTableView(this);
    addWidget(tagCloud);

    outlinesTable = new OutlinesTableView(this);
    addWidget(outlinesTable);

    outlinesMap = new OutlinesMapView(this);
    addWidget(outlinesMap);

    recentNotesTable = new RecentNotesTableView(this);
    addWidget(recentNotesTable);

    navigator = new NavigatorView(this);
    addWidget(navigator);

    outlineView = new OutlineViewSplitter(this);
    addWidget(outlineView);

    outlineHeaderView = new OutlineHeaderView(this);
    addWidget(outlineHeaderView);

    outlineHeaderEdit = new OutlineHeaderEditView(this);
    addWidget(outlineHeaderEdit);

    noteView = new NoteView(this);
    addWidget(noteView);

    noteEdit = new NoteEditView{this};
    addWidget(noteEdit);

    // DEBUG: orloj layout assembly
    //setStyleSheet("border: 1px solid red;");
}

void OrlojView::hideChildren(const QSet<QWidget*>& visibleChildren)
{
    // IMPROVE this method to be called on app window resize only
    fiftyFifty();

    for(int i{}; i<count(); i++) {
        if(visibleChildren.contains(widget(i))) {
            if(widget(i)->isHidden()) {
                widget(i)->show();
            }
        } else {
            if(widget(i)->isVisible()) {
                widget(i)->hide();
            }
        }
    }
}

void OrlojView::fiftyFifty()
{
    // 50%/50%
    int half = size().width()/2;
    QList<int> sizes{};
    for(int i{}; i<count(); i++) {
        sizes << half;
    }
    setSizes(sizes);
}

void OrlojView::showFacetDashboard()
{
    QSet<QWidget*> v; v << dashboard;
    hideChildren(v);
}

void OrlojView::showFacetOrganizers()
{
    QSet<QWidget*> v; v << organizersTable;
    hideChildren(v);
}

void OrlojView::showFacetOrganizer()
{
    QSet<QWidget*> v; v << organizer;
    hideChildren(v);
}

void OrlojView::showFacetKanban()
{
    QSet<QWidget*> v; v << kanban;
    hideChildren(v);
}

void OrlojView::showFacetTagCloud()
{
    QSet<QWidget*> v; v << tagCloud;
    hideChildren(v);
}

void OrlojView::showFacetOutlines()
{
    QSet<QWidget*> v; v << outlinesTable;
    hideChildren(v);
}

void OrlojView::showFacetOutlinesMap()
{
    QSet<QWidget*> v; v << outlinesMap;
    hideChildren(v);
}

void OrlojView::showFacetOutlinesDetail()
{
    QSet<QWidget*> v; v << outlinesTable << outlineView;

    hideChildren(v);
}

void OrlojView::showFacetRecentNotes()
{
    QSet<QWidget*> v; v << recentNotesTable;
    hideChildren(v);
}

void OrlojView::showFacetOutlineHeaderView()
{
    if(!outlineHeaderView->getEditPanel()->isExpertMode()) {
        outlineHeaderView->getEditPanel()->setVisible(true);
    }

    if(menuView->actionViewHoist->isChecked()) {
        showFacetHoistedOutlineHeaderView();
    } else {
        QSet<QWidget*> v; v << outlineView << outlineHeaderView;
        hideChildren(v);
        outlineView->getOutlineTree()->clearSelection();
        outlineView->getOutlineTree()->setFocus();
    }
}

void OrlojView::showFacetOutlineHeaderEdit()
{
    if(menuView->actionViewHoist->isChecked()) {
        showFacetHoistedOutlineHeaderEdit();
    } else {
        if(menuView->actionEditLiveNotePreview->isChecked()) {
            outlineHeaderView->getEditPanel()->setVisible(false);
            QSet<QWidget*> v; v << outlineHeaderView << outlineHeaderEdit;
            hideChildren(v);
        } else {
            QSet<QWidget*> v; v << outlineView << outlineHeaderEdit;
            hideChildren(v);
        }
        outlineHeaderEdit->giveEditorFocus();
    }
}

void OrlojView::showFacetNoteView()
{
    if(!noteView->getButtonsPanel()->isExpertMode()) {
        noteView->getButtonsPanel()->setVisible(true);
    }

    if(menuView->actionViewHoist->isChecked()) {
        showFacetHoistedNoteView();
    } else {
        QSet<QWidget*> v; v << outlineView << noteView;
        hideChildren(v);
        outlineView->getOutlineTree()->setFocus();
    }
}

void OrlojView::showFacetNoteEdit()
{
    if(menuView->actionViewHoist->isChecked()) {
        showFacetHoistedNoteEdit();
    } else {
        if(menuView->actionEditLiveNotePreview->isChecked()) {
            noteView->getButtonsPanel()->setVisible(false);
            QSet<QWidget*> v; v << noteView << noteEdit;
            hideChildren(v);
        } else {
            QSet<QWidget*> v; v << outlineView << noteEdit;
            hideChildren(v);
        }

        noteEdit->giveEditorFocus();
    }
}

void OrlojView::showFacetNavigator()
{
    QSet<QWidget*> v; v << navigator;
    hideChildren(v);
}

void OrlojView::showFacetNavigatorOutline()
{
    QSet<QWidget*> v; v << navigator << outlineHeaderView;
    hideChildren(v);
}

void OrlojView::showFacetNavigatorNote()
{
    QSet<QWidget*> v; v << navigator << noteView;
    hideChildren(v);
}

/*
 * Hoisting.
 */

bool OrlojView::isHoisting() const
{
    return menuView->actionViewHoist->isChecked();
}

void OrlojView::showFacetHoistedOutlineHeaderView()
{
    QSet<QWidget*> v; v << outlineHeaderView;
    hideChildren(v);
    outlineHeaderView->giveViewerFocus();
}

void OrlojView::showFacetHoistedOutlineHeaderEdit()
{
    QSet<QWidget*> v; v << outlineHeaderEdit;
    hideChildren(v);
    outlineHeaderEdit->giveEditorFocus();
}

void OrlojView::showFacetHoistedNoteView()
{
    QSet<QWidget*> v; v << noteView;
    hideChildren(v);
    noteView->giveViewerFocus();
}

void OrlojView::showFacetHoistedNoteEdit()
{
    QSet<QWidget*> v; v << noteEdit;
    hideChildren(v);
    noteEdit->giveEditorFocus();
}

} // m8r namespace
