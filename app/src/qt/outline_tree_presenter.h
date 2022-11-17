/*
 outline_tree_presenter.cpp     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_TREE_PRESENTER_H
#define M8RUI_OUTLINE_TREE_PRESENTER_H

#include <set>

#include "../../lib/src/model/outline.h"
#include "../../lib/src/mind/mind.h"

#include <QtWidgets>

#include "orloj_presenter.h"
#include "outline_tree_view.h"
#include "outline_tree_model.h"

namespace m8r {

class OrlojPresenter;

class OutlineTreePresenter : public QObject
{
    Q_OBJECT

public:
    static const int NO_ROW = -1;

private:
    OutlineTreeView* view;
    OutlineTreeModel* model;

    Mind* mind;

public:
    explicit OutlineTreePresenter(OutlineTreeView* view, MainWindowPresenter* mwp, QObject* parent);
    OutlineTreePresenter(const OutlineTreePresenter&) = delete;
    OutlineTreePresenter(const OutlineTreePresenter&&) = delete;
    OutlineTreePresenter &operator=(const OutlineTreePresenter&) = delete;
    OutlineTreePresenter &operator=(const OutlineTreePresenter&&) = delete;

    OutlineTreeView* getView() const { return view; }
    OutlineTreeModel* getModel() const { return model; }

    void refresh(Outline* outline, Outline::Patch* patch=nullptr);
    void refresh(Note* note);
    void selectRow(int row);
    void insertAndSelect(Note* note);

    void clearSelection();
    void focus() { view->setFocus(); }
    void selectRowByNote(const Note* note);

    int getCurrentRow() const;
    Note* getCurrentNote() const;
    Note* getAdjacentNote() const;

    ~OutlineTreePresenter();

public slots:
    void slotSelectNextRow();
    void slotSelectPreviousRow();
};

}
#endif // M8RUI_OUTLINE_TREE_PRESENTER_H
