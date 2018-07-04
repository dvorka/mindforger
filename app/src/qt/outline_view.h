/*
 outline_view.h     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_VIEW_H
#define M8RUI_OUTLINE_VIEW_H

#include <QtWidgets>

#include "../../lib/src/gear/lang_utils.h"

#include "outline_tree_view.h"
#include "look_n_feel.h"

namespace m8r {

class OutlineNamePushButton : public QPushButton
{
public:
    explicit OutlineNamePushButton(QWidget* parent) : QPushButton{parent} {}

protected:
    virtual void enterEvent(QEvent *) {
        setCursor(QCursor(Qt::PointingHandCursor));
        update();
    }

    virtual void leaveEvent(QEvent *) {
        setCursor(QCursor(Qt::ArrowCursor));
        update();
    }
};

class OutlineView : public QWidget
{
    Q_OBJECT

private:
    OutlineNamePushButton* nameLabel;
    OutlineTreeView *outlineTreeView;

    QVBoxLayout headerVerticalLayout;

public:
    explicit OutlineView(QWidget* parent);
    OutlineView(const OutlineView&) = delete;
    OutlineView(const OutlineView&&) = delete;
    OutlineView &operator=(const OutlineView&) = delete;
    OutlineView &operator=(const OutlineView&&) = delete;
    ~OutlineView();

    void refreshHeader(const std::string& name);
    const QPushButton* getNameLabel() const { return nameLabel; }
    OutlineTreeView* getOutlineTree() const { return outlineTreeView; }
};

}
#endif // M8RUI_OUTLINE_VIEW_H
