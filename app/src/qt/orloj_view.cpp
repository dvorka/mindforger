/*
 outline_view.cpp     MindForger thinking notebook

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
#include "orloj_view.h"

namespace m8r {

using namespace std;

OrlojView::OrlojView(QWidget* parent)
    : QSplitter(Qt::Horizontal, parent)
{
    // TODO to be implemented
    //addWidget(new QTextEdit("Home: outlines (link), Notes (l), bytes (l), graphs, recent"), this);

    organizer = new OrganizerView(this);
    addWidget(organizer);

    tagCloud = new TagsTableView(this);
    addWidget(tagCloud);

    outlinesTable = new OutlinesTableView(this);
    addWidget(outlinesTable);

    notesTable = new NotesTableView(this);
    addWidget(notesTable);

    // TODO to be implemented
    //navigator = new GraphWidget(this);
    //addWidget(navigator);

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

void OrlojView::showFacetOrganizer()
{
    QSet<QWidget*> v; v << organizer;
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

void OrlojView::showFacetOutlinesDetail()
{
    QSet<QWidget*> v; v << outlinesTable << outlineView;

    hideChildren(v);
}

void OrlojView::showFacetFtsResult()
{
    QSet<QWidget*> v; v << notesTable;
    hideChildren(v);
}

void OrlojView::showFacetFtsResultDetail()
{
    QSet<QWidget*> v; v << notesTable << noteView;
    hideChildren(v);
}

void OrlojView::showFacetOutlineHeaderView()
{
    QSet<QWidget*> v; v << outlineView << outlineHeaderView;
    hideChildren(v);
    outlineView->getOutlineTree()->clearSelection();
}

void OrlojView::showFacetOutlineHeaderEdit()
{
    QSet<QWidget*> v; v << outlineView << outlineHeaderEdit;
    hideChildren(v);
}

void OrlojView::showFacetNoteView()
{
    QSet<QWidget*> v; v << outlineView << noteView;
    hideChildren(v);
    outlineView->getOutlineTree()->setFocus();
}

void OrlojView::showFacetNoteEdit()
{
    QSet<QWidget*> v; v << outlineView << noteEdit;
    hideChildren(v);
    noteEdit->giveFocusToEditor();
}

/*
 * Hoisting.
 */

bool OrlojView::isHoistView()
{
    if(!outlineView->isVisible()) {
        if(outlineHeaderView->isVisible()) {
            return true;
        } else if(outlineHeaderEdit->isVisible()) {
            return true;
        } else if(noteView->isVisible()) {
            return true;
        } else if(noteEdit->isVisible()) {
            return true;
        }
    }

    return false;
}

void OrlojView::showFacetHoistedOutlineHeaderView()
{
    QSet<QWidget*> v; v << outlineHeaderView;
    hideChildren(v);
    outlineHeaderView->setFocus();
}

void OrlojView::showFacetHoistedOutlineHeaderEdit()
{
    QSet<QWidget*> v; v << outlineHeaderEdit;
    hideChildren(v);
    outlineHeaderEdit->giveFocusToEditor();
}

void OrlojView::showFacetHoistedNoteView()
{
    QSet<QWidget*> v; v << noteView;
    hideChildren(v);
    noteView->setFocus();
}

void OrlojView::showFacetHoistedNoteEdit()
{
    QSet<QWidget*> v; v << noteEdit;
    hideChildren(v);
    noteEdit->giveFocusToEditor();
}

} // m8r namespace
