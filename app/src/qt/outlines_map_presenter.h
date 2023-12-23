/*
 outlines_map_presenter.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_OUTLINES_MAP_PRESENTER_H
#define M8RUI_OUTLINES_MAP_PRESENTER_H

#include <set>

#include "../../lib/src/model/outline.h"
#include "../../lib/src/mind/mind.h"

#include <QtWidgets>

#include "orloj_presenter.h"
#include "outlines_map_view.h"
#include "outlines_map_model.h"

namespace m8r {

class OrlojPresenter;

/**
 * @brief The Outlines map presenter.
 *
 * Outlines map ~ tree of Outlines is in fact ~ mind map of Outlines
 */
class OutlinesMapPresenter : public QObject
{
    Q_OBJECT

    // Assembly ... view / model / presenter:
    //
    //   view <- new View
    //     presenter <- new Presenter(view) w/ new Model(view)
    //       model <- presenter.model
    //
    // Integration:
    //
    //   orlojView <- new View()
    //   orlojPresenter <- presenter
    //
    // Materialization / view:
    //
    //   menu ... MainMenuView::actionViewOutlinesMap
    //     -> <signal> @ MainMenuPresenter
    //       -> <handler> MainWindowPresenter::doActionViewOutlinesMap()
    //          ... generate virtual O from Os headers
    //         -> OrlojPresenter::showFacetOutlinesMap( virtualO )
    //

public:
    static const int NO_ROW = -1;

private:
    OutlinesMapView* view;
    OutlinesMapModel* model;

    Mind* mind;

public:
    explicit OutlinesMapPresenter(
        OutlinesMapView* view, MainWindowPresenter* mwp, QObject* parent);
    OutlinesMapPresenter(const OutlinesMapPresenter&) = delete;
    OutlinesMapPresenter(const OutlinesMapPresenter&&) = delete;
    OutlinesMapPresenter &operator=(const OutlinesMapPresenter&) = delete;
    OutlinesMapPresenter &operator=(const OutlinesMapPresenter&&) = delete;

    OutlinesMapView* getView() const { return view; }
    OutlinesMapModel* getModel() const { return model; }

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

    ~OutlinesMapPresenter();

public slots:
    void slotSelectNextRow();
    void slotSelectPreviousRow();
};

}
#endif // M8RUI_OUTLINES_MAP_PRESENTER_H
