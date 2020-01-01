/*
 view_to_edit_buttons_panel.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_VIEW_TO_EDIT_BUTTONS_PANEL_H
#define M8R_VIEW_TO_EDIT_BUTTONS_PANEL_H

#include <QtWidgets>

#include "mf_widgets.h"

namespace m8r {

class ViewToEditEditButtonsPanel : public QWidget
{
    Q_OBJECT

private:
    MfWidgetMode mode;

    QHBoxLayout* layout;
    QPushButton* editButton;

public:
    explicit ViewToEditEditButtonsPanel(MfWidgetMode mode, QWidget* parent);
    ViewToEditEditButtonsPanel(const ViewToEditEditButtonsPanel&) = delete;
    ViewToEditEditButtonsPanel(const ViewToEditEditButtonsPanel&&) = delete;
    ViewToEditEditButtonsPanel &operator=(const ViewToEditEditButtonsPanel&) = delete;
    ViewToEditEditButtonsPanel &operator=(const ViewToEditEditButtonsPanel&&) = delete;
    ~ViewToEditEditButtonsPanel();

public:
    QPushButton* getEditButton() const { return editButton; }
};

}
#endif // M8R_VIEW_TO_EDIT_BUTTONS_PANEL_H
