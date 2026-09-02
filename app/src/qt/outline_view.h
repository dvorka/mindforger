/*
 outline_view.h     MindForger thinking notebook

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

    // raw (non-elided) name of the currently shown Outline - cached so that
    // the elided nameLabel text can be recomputed once this widget's width
    // is actually known (see resizeEvent())
    std::string currentName{};

    void updateNameLabelText();

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

protected:
    // recompute the elided nameLabel text on every resize: on the very first
    // Outline shown after MindForger start, refreshHeader() may run before
    // this widget has received its first real layout pass, so width() is
    // still stale/incorrect at that point - the subsequent resize event(s)
    // fired once the surrounding QSplitter finishes laying out fix it up
    void resizeEvent(QResizeEvent* event) override;
};

}
#endif // M8RUI_OUTLINE_VIEW_H
