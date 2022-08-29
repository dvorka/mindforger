/*
 orloj_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <vector>

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

void OrlojView::hideChildren(const QList<QWidget*>& visibleChildren)
{
    // save layout
    if (leftPanelIndex != -1)
        savedLeftPanelSize = sizes()[leftPanelIndex];

    // restore layout
    auto sizes = QList<int>::fromStdList(std::list<int>(count(), width() / 2)); // 50%/50%
    if (visibleChildren.size() == 2) {
        leftPanelIndex = indexOf(visibleChildren[0]);
        if (savedLeftPanelSize) {
            sizes[leftPanelIndex] = savedLeftPanelSize;
            sizes[indexOf(visibleChildren[1])] = width() - handleWidth() - savedLeftPanelSize;
        }
    } else {
        leftPanelIndex = -1;
    }
    setSizes(sizes);

    for(int i{}; i<count(); i++)
        widget(i)->setVisible(visibleChildren.contains(widget(i)));
}

void OrlojView::showFacetDashboard()
{
    hideChildren({dashboard});
}

void OrlojView::showFacetOrganizers()
{
    hideChildren({ organizersTable });
}

void OrlojView::showFacetOrganizer()
{
    hideChildren({ organizer });
}

void OrlojView::showFacetKanban()
{
    hideChildren({ kanban });
}

void OrlojView::showFacetTagCloud()
{
    hideChildren({ tagCloud });
}

void OrlojView::showFacetOutlines()
{
    hideChildren({ outlinesTable });
}

void OrlojView::showFacetOutlinesDetail()
{
    hideChildren({ outlinesTable, outlineView });
}

void OrlojView::showFacetRecentNotes()
{
    hideChildren({ recentNotesTable });
}

void OrlojView::showFacetOutlineHeaderView()
{
    if(!outlineHeaderView->getEditPanel()->isExpertMode()) {
        outlineHeaderView->getEditPanel()->setVisible(true);
    }

    if(menuView->actionViewHoist->isChecked()) {
        showFacetHoistedOutlineHeaderView();
    } else {
        hideChildren({ outlineView, outlineHeaderView });
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
            hideChildren({ outlineHeaderView, outlineHeaderEdit });
        } else {
            hideChildren({ outlineView, outlineHeaderEdit });
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
        hideChildren({ outlineView, noteView });
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
            hideChildren({ noteView, noteEdit });
        } else {
            hideChildren({ outlineView, noteEdit });
        }

        noteEdit->giveEditorFocus();
    }
}

void OrlojView::showFacetNavigator()
{
    hideChildren({ navigator });
}

void OrlojView::showFacetNavigatorOutline()
{
    hideChildren({ navigator, outlineHeaderView });
}

void OrlojView::showFacetNavigatorNote()
{
    hideChildren({ navigator, noteView });
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
    hideChildren({ outlineHeaderView });
    outlineHeaderView->giveViewerFocus();
}

void OrlojView::showFacetHoistedOutlineHeaderEdit()
{
    hideChildren({ outlineHeaderEdit });
    outlineHeaderEdit->giveEditorFocus();
}

void OrlojView::showFacetHoistedNoteView()
{
    hideChildren({ noteView });
    noteView->giveViewerFocus();
}

void OrlojView::showFacetHoistedNoteEdit()
{
    hideChildren({ noteEdit });
    noteEdit->giveEditorFocus();
}

} // m8r namespace
