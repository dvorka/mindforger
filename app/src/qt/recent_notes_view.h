/*
 recent_notes_view.h     MindForger thinking notebook

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
#ifndef M8RUI_RECENT_NOTES_VIEW_H
#define M8RUI_RECENT_NOTES_VIEW_H

#include "../../../lib/src/debug.h"

#include <QtWidgets>

#include "qt_commons.h"

namespace m8r {

/**
 * @brief Tree of Notebooks w/ their recent Notes.
 */
class RecentNotesTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit RecentNotesTreeView(QWidget* parent);
    RecentNotesTreeView(const RecentNotesTreeView&) = delete;
    RecentNotesTreeView(const RecentNotesTreeView&&) = delete;
    RecentNotesTreeView &operator=(const RecentNotesTreeView&) = delete;
    RecentNotesTreeView &operator=(const RecentNotesTreeView&&) = delete;
    virtual ~RecentNotesTreeView() override {}

protected:
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;

signals:
    void signalShowSelectedRecentNote();
};

/**
 * @brief Recent Notes view: header bar w/ mode toggle and the tree of recent Notes.
 */
class RecentNotesView : public QWidget
{
    Q_OBJECT

    QRadioButton* viewedOrEditedRadio;
    QRadioButton* editedRadio;
    RecentNotesTreeView* tree;

public:
    explicit RecentNotesView(QWidget* parent);
    RecentNotesView(const RecentNotesView&) = delete;
    RecentNotesView(const RecentNotesView&&) = delete;
    RecentNotesView &operator=(const RecentNotesView&) = delete;
    RecentNotesView &operator=(const RecentNotesView&&) = delete;
    virtual ~RecentNotesView() override {}

    RecentNotesTreeView* getTree() const { return tree; }

    /**
     * @brief Set mode toggle w/o emitting the mode change signal.
     */
    void setEditedOnly(bool editedOnly);

private slots:
    void slotEditedOnlyToggled(bool editedOnly);

signals:
    void signalEditedOnlyChanged(bool editedOnly);
};

}
#endif // M8RUI_RECENT_NOTES_VIEW_H
