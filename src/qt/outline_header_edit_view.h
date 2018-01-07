/*
 outline_header_edit_view.h     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_HEADER_EDIT_VIEW_H
#define M8RUI_OUTLINE_HEADER_EDIT_VIEW_H

#include <QtWidgets>

#include "note_edit_view.h"

namespace m8r {

class OrlojView;

class OutlineHeaderEditView : public NoteEditView
{
    Q_OBJECT

public:
    explicit OutlineHeaderEditView(QWidget* parent);
    OutlineHeaderEditView(const OutlineHeaderEditView&) = delete;
    OutlineHeaderEditView(const OutlineHeaderEditView&&) = delete;
    OutlineHeaderEditView &operator=(const OutlineHeaderEditView&) = delete;
    OutlineHeaderEditView &operator=(const OutlineHeaderEditView&&) = delete;
    ~OutlineHeaderEditView();

private slots:
    void slotSaveOutlineHeader(void);

signals:
    void signalSaveOutlineHeader(void);
};

}
#endif // M8RUI_OUTLINE_HEADER_EDIT_VIEW_H
