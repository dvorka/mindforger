/*
 outline_tree_presenter.cpp     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_TREE_PRESENTER_H
#define M8RUI_OUTLINE_TREE_PRESENTER_H

#include "lib/src/model/outline.h"

#include <QtWidgets>

#include "orloj_presenter.h"
#include "outline_tree_view.h"
#include "outline_tree_model.h"

namespace m8r {

class OrlojPresenter;

class OutlineTreePresenter : public QObject
{
    Q_OBJECT

private:
    OutlineTreeView* view;
    OutlineTreeModel* model;

public:
    explicit OutlineTreePresenter(OutlineTreeView* view, QObject* parent);
    OutlineTreePresenter(const OutlineTreePresenter&) = delete;
    OutlineTreePresenter(const OutlineTreePresenter&&) = delete;
    OutlineTreePresenter &operator=(const OutlineTreePresenter&) = delete;
    OutlineTreePresenter &operator=(const OutlineTreePresenter&&) = delete;

    OutlineTreeModel* getModel() const { return model; }

    void refresh(Outline* outline);
    void refresh(Note* note);

    int getCurrentRow() const;
    Note* getCurrentNote() const;

    ~OutlineTreePresenter();

public slots:
    void slotSelectNextRow();
    void slotSelectLastRow();
};

}
#endif // M8RUI_OUTLINE_TREE_PRESENTER_H
