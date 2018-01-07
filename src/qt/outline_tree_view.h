/*
 outline_tree_view.cpp     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_TREE_VIEW_H
#define M8RUI_OUTLINE_TREE_VIEW_H

#include <QtWidgets>

#include "lib/src/gear/lang_utils.h"

namespace m8r {

class OutlineTreeView;

class OutlineTreeViewItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

private:
    QObject* parent;
public:
    OutlineTreeViewItemDelegate(QObject* outlineTreeView)
        : QStyledItemDelegate(outlineTreeView)
    {
        this->parent = outlineTreeView;
        installEventFilter(this);
    }

    /**
     * If you want to FILTER the event out, i.e. STOP it being
     * handled further, then return TRUE; otherwise return false.
     *
     * Event filter must be installed in constructor.
     */
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        if(event->type() == QEvent::KeyPress) {
            qDebug("tree itemDelegate eventFilter()");
            qDebug("tree itemDelegate FORWARD");
            return true;
        } else {
            return QStyledItemDelegate::eventFilter(obj, event);
        }
    }

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override
    {
        QWidget *result = QStyledItemDelegate::createEditor(parent, option, index);
        result->installEventFilter(new OutlineTreeViewItemDelegate(parent));
        return result;
    }
};

class OutlineTreeView : public QTableView
{
    Q_OBJECT

protected:
    QWidget* parent;

public:
    explicit OutlineTreeView(QWidget* parent);
    OutlineTreeView(const OutlineTreeView&) = delete;
    OutlineTreeView(const OutlineTreeView&&) = delete;
    OutlineTreeView &operator=(const OutlineTreeView&) = delete;
    OutlineTreeView &operator=(const OutlineTreeView&&) = delete;

    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;

signals:
    void signalFromOutlineTreeToOutlines(void);

    void signalSelectNextRow(void);
    void signalSelectLastRow(void);

    void signalChangeUp(void);
    void signalChangeDown(void);
    void signalChangePromote(void);
    void signalChangeDemote(void);
    void signalChangeFirst(void);
    void signalChangeLast(void);
};

}
#endif // M8RUI_OUTLINE_TREE_VIEW_H
