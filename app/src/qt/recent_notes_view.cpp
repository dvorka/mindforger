/*
 recent_notes_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "recent_notes_view.h"

namespace m8r {

using namespace std;

/*
 * Tree
 */

RecentNotesTreeView::RecentNotesTreeView(QWidget* parent)
    : QTreeView(parent)
{
    setSortingEnabled(false);
    setUniformRowHeights(true);
    setAllColumnsShowFocus(true);
    // double click opens O instead of collapsing/expanding it
    setExpandsOnDoubleClick(false);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

void RecentNotesTreeView::keyPressEvent(QKeyEvent* event)
{
    if(!(event->modifiers() & Qt::AltModifier)
         &&
       !(event->modifiers() & Qt::ControlModifier)
         &&
       !(event->modifiers() & Qt::ShiftModifier))
    {
        switch(event->key()) {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            emit signalShowSelectedRecentNote();
            return;
        case Qt::Key_Right:
            // right on N (leaf) opens it, right on O expands it/moves to its 1st N
            if(currentIndex().isValid() && currentIndex().parent().isValid()) {
                emit signalShowSelectedRecentNote();
                return;
            }
            break;
        }
    }

    // up, down, left, home, end, page up/down, ... handled by the tree
    QTreeView::keyPressEvent(event);
}

void RecentNotesTreeView::mouseDoubleClickEvent(QMouseEvent* event)
{
    // double click selects the row first, then opens O/N
    QTreeView::mouseDoubleClickEvent(event);

    if(indexAt(event->pos()).isValid()) {
        emit signalShowSelectedRecentNote();
    }
}

void RecentNotesTreeView::resizeEvent(QResizeEvent* event)
{
    MF_DEBUG("RecentNotesTreeView::resizeEvent " << event << std::endl);

    if(header()->count() > 0) {
        // ensure that 1st column gets the remaining space from others
        header()->setStretchLastSection(false);
        header()->setSectionResizeMode(0, QHeaderView::Stretch);

        // rds/wrs
        setColumnWidth(1, fontMetrics().averageCharWidth()*6);
        setColumnWidth(2, fontMetrics().averageCharWidth()*6);
        // pretty rd/wr
        setColumnWidth(3, fontMetrics().averageCharWidth()*12);
        setColumnWidth(4, fontMetrics().averageCharWidth()*12);
    }

    QTreeView::resizeEvent(event);
}

/*
 * View
 */

RecentNotesView::RecentNotesView(QWidget* parent)
    : QWidget(parent)
{
    // header bar w/ mode toggle
    viewedOrEditedRadio = new QRadioButton(tr("Viewed or edited"), this);
    viewedOrEditedRadio->setToolTip(tr("Show Notes which were recently viewed or edited"));
    editedRadio = new QRadioButton(tr("Edited"), this);
    editedRadio->setToolTip(tr("Show Notes which were recently edited only"));
    viewedOrEditedRadio->setChecked(true);
    // buttons are exclusive as they share the parent > no button group needed

    QHBoxLayout* headerLayout = new QHBoxLayout{};
    headerLayout->setContentsMargins(5, 2, 5, 2);
    headerLayout->addWidget(new QLabel(tr("Show:"), this));
    headerLayout->addWidget(viewedOrEditedRadio);
    headerLayout->addWidget(editedRadio);
    headerLayout->addStretch();

    tree = new RecentNotesTreeView(this);

    QVBoxLayout* layout = new QVBoxLayout{this};
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addLayout(headerLayout);
    layout->addWidget(tree);
    setLayout(layout);

    // edited radio is enough to signal both - radios are exclusive
    QObject::connect(
        editedRadio, SIGNAL(toggled(bool)),
        this, SLOT(slotEditedOnlyToggled(bool)));
}

void RecentNotesView::setEditedOnly(bool editedOnly)
{
    QSignalBlocker blocker{editedRadio};
    if(editedOnly) {
        editedRadio->setChecked(true);
    } else {
        viewedOrEditedRadio->setChecked(true);
    }
}

void RecentNotesView::slotEditedOnlyToggled(bool editedOnly)
{
    MF_DEBUG("RecentNotesView: edited only toggled to " << editedOnly << std::endl);
    emit signalEditedOnlyChanged(editedOnly);
}

} // m8r namespace
