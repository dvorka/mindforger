/*
 outline_tree_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_OUTLINE_TREE_VIEW_H
#define M8RUI_OUTLINE_TREE_VIEW_H

#include <QtWidgets>

#include "../../lib/src/gear/lang_utils.h"
#include "../lib/src/debug.h"

namespace m8r {

class OutlineTreeView;

class OutlineTreeViewItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    OutlineTreeViewItemDelegate(QObject* outlineTreeView)
        : QStyledItemDelegate(outlineTreeView)
    {
        installEventFilter(this);
    }

    /**
     * If you want to FILTER the event out, i.e. STOP it being
     * handled further, then return TRUE; otherwise return FALSE.
     *
     * Event filter must be installed in constructor.
     */
    bool eventFilter(QObject* obj, QEvent* event) override
    {
        if(event->type() == QEvent::KeyPress) {
            return true;
        } else {
            return QStyledItemDelegate::eventFilter(obj, event);
        }
    }

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override
    {
        QWidget *result = QStyledItemDelegate::createEditor(parent, option, index);
        result->installEventFilter(new OutlineTreeViewItemDelegate(parent));
        return result;
    }
};

class OutlineTreeView : public QTableView
{
    Q_OBJECT

private:
    // if view is width < threshold columns, then shows simplified view w/o Mind-related columns
    static constexpr int SIMPLIFIED_VIEW_THRESHOLD_WIDTH = 75;

public:
    explicit OutlineTreeView(QWidget* parent);
    OutlineTreeView(const OutlineTreeView&) = delete;
    OutlineTreeView(const OutlineTreeView&&) = delete;
    OutlineTreeView &operator=(const OutlineTreeView&) = delete;
    OutlineTreeView &operator=(const OutlineTreeView&&) = delete;

    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    void refreshNotes(const QModelIndex& from, const QModelIndex& to) { dataChanged(from, to); }

signals:
    void signalOutlineShow();

    void signalFromOutlineTreeToOutlines();

    void signalSelectNextRow();
    void signalSelectPreviousRow();

    void signalChangeUp();
    void signalChangeDown();
    void signalChangePromote();
    void signalChangeDemote();
    void signalChangeFirst();
    void signalChangeLast();

    void signalOutlineOrNoteEdit(); // O or N edit
    void signalEdit(); // N edit
    void signalForget();
};

}
#endif // M8RUI_OUTLINE_TREE_VIEW_H
