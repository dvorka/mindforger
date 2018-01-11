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

OrlojView::OrlojView(QWidget* parent)
    : QSplitter(Qt::Horizontal, parent)
{
    addWidget(new QTextEdit("Home: outlines (link), Notes (l), bytes (l), graphs, recent"));

    addWidget(new QTextEdit("Organizer"));

    outlinesTable = new OutlinesTableView(this);
    addWidget(outlinesTable);

    notesTable = new NotesTableView(this);
    addWidget(notesTable);

    navigator = new GraphWidget;
    addWidget(navigator);

    addWidget(new QTextEdit("Inbox"));

    outlineView = new OutlineView(this);
    addWidget(outlineView);

    outlineHeaderView = new OutlineHeaderView(this);
    addWidget(outlineHeaderView);

    outlineHeaderEdit = new OutlineHeaderEditView(this);
    addWidget(outlineHeaderEdit);

    addWidget(new QTextEdit("Outline HTML single page"));

    noteView = new NoteView(this);
    addWidget(noteView);

    addWidget(new QTextEdit("Outline single page edit"));

    noteEdit = new NoteEditorView{this};
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

void OrlojView::fiftyFifty(void)
{
    // 50%/50%
    int half = size().width()/2;
    QList<int> sizes{};
    for(int i{}; i<count(); i++) {
        sizes << half;
    }
    setSizes(sizes);
}

void OrlojView::showFacetOutlines(void)
{
    QSet<QWidget*> v; v << outlinesTable;
    hideChildren(v);
}

void OrlojView::showFacetOutlinesDetail(void)
{
    QSet<QWidget*> v; v << outlinesTable << outlineView;
    hideChildren(v);
}

void OrlojView::showFacetFtsResult(void)
{
    QSet<QWidget*> v; v << notesTable;
    hideChildren(v);
}

void OrlojView::showFacetFtsResultDetail(void)
{
    QSet<QWidget*> v; v << notesTable << noteView;
    hideChildren(v);
}

void OrlojView::showFacetOutlineHeaderView(void)
{
    QSet<QWidget*> v; v << outlineView << outlineHeaderView;
    hideChildren(v);
    outlineView->getOutlineTree()->clearSelection();
}

void OrlojView::showFacetOutlineHeaderEdit(void)
{
    QSet<QWidget*> v; v << outlineView << outlineHeaderEdit;
    hideChildren(v);
}

void OrlojView::showFacetNoteView(void)
{
    QSet<QWidget*> v; v << outlineView << noteView;
    hideChildren(v);
    outlineView->getOutlineTree()->setFocus();
}

void OrlojView::showFacetNoteEdit(void)
{
    QSet<QWidget*> v; v << outlineView << noteEdit;
    hideChildren(v);
    noteEdit->setFocus();
}

}
