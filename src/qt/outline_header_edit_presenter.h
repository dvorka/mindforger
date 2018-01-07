/*
 outline_header_edit_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_HEADER_EDIT_PRESENTER_H
#define M8RUI_OUTLINE_HEADER_EDIT_PRESENTER_H

#include "lib/src/model/outline.h"
#include "lib/src/representations/markdown/markdown_outline_representation.h"

#include "main_window_presenter.h"
#include "outline_header_edit_view.h"

#include <QtWidgets>

namespace m8r {

class OrlojView;

class OutlineHeaderEditPresenter : public QObject
{
    Q_OBJECT

private:
    OutlineHeaderEditView *view;
    MainWindowPresenter* mainPresenter;

    MarkdownOutlineRepresentation* mdRepresentation;

    Outline* currentOutline;

public:
    explicit OutlineHeaderEditPresenter(OutlineHeaderEditView* view, MainWindowPresenter* mwp, QObject* parent);
    OutlineHeaderEditPresenter(const OutlineHeaderEditPresenter&) = delete;
    OutlineHeaderEditPresenter(const OutlineHeaderEditPresenter&&) = delete;
    OutlineHeaderEditPresenter &operator=(const OutlineHeaderEditPresenter&) = delete;
    OutlineHeaderEditPresenter &operator=(const OutlineHeaderEditPresenter&&) = delete;
    ~OutlineHeaderEditPresenter();

    void setCurrentOutline(Outline* outline, std::string* html);

public slots:
    void slotSave(void);
    void slotSaveAndCloseEditor(void);
};

}
#endif // M8RUI_OUTLINE_HEADER_EDIT_PRESENTER_H
