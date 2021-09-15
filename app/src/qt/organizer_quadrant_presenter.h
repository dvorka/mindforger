/*
 organizer_quadrant_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_ORGANIZER_QUADRANT_PRESENTER_H
#define M8RUI_ORGANIZER_QUADRANT_PRESENTER_H

#include <QtWidgets>

#include <vector>

#include "organizer_quadrant_view.h"
#include "organizer_quadrant_model.h"
#include "orloj_presenter.h"
#include "html_delegate.h"
#include "dialogs/organizer_cfg_dialog.h"

#include "../../lib/src/model/outline.h"

namespace m8r {

class OrlojPresenter;

class OrganizerQuadrantPresenter : public QObject
{
    Q_OBJECT

private:
    static const int NO_ROW = -1;

    OrganizerQuadrantView* view;
    OrganizerQuadrantModel* model;

    OrlojPresenter* orloj;

    OrganizerConfigDialog* configDialog;

public:
    explicit OrganizerQuadrantPresenter(OrganizerQuadrantView* view, OrlojPresenter* orloj, QString title);
    OrganizerQuadrantPresenter(const OrganizerQuadrantPresenter&) = delete;
    OrganizerQuadrantPresenter(const OrganizerQuadrantPresenter&&) = delete;
    OrganizerQuadrantPresenter &operator=(const OrganizerQuadrantPresenter&) = delete;
    OrganizerQuadrantPresenter &operator=(const OrganizerQuadrantPresenter&&) = delete;
    ~OrganizerQuadrantPresenter();

    void setTitle(QString& title) { model->setTitle(title); }

    int getCurrentRow() const;
    void refresh(const std::vector<Outline*>& os, bool urgency, bool importance);
    OrganizerQuadrantView* getView() const { return view; }

public slots:
    void slotShowSelectedOutline();
    void slotShowOutline(const QItemSelection& selected, const QItemSelection& deselected);
    void slotHeaderClicked(int section);
};

}
#endif // M8RUI_ORGANIZER_QUADRANT_PRESENTER_H
