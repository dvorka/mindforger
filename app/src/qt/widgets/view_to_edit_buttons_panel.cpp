/*
 view_to_edit_buttons_panel.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "view_to_edit_buttons_panel.h"

namespace m8r {

using namespace std;

ViewToEditEditButtonsPanel::ViewToEditEditButtonsPanel(MfWidgetMode mode, QWidget* parent)
    : QWidget(parent), mode(mode)
{
    // widgets
    editButton = new QPushButton{tr("Edit"), this};
    editButton->setToolTip("Ctrl+E");

    // TODO Show whole Notebook OR Show Notebook description (toggle buttons)

    // assembly
    layout = new QHBoxLayout{this};
    layout->addStretch(1);
    layout->addWidget(editButton);
    setLayout(layout);
}

ViewToEditEditButtonsPanel::~ViewToEditEditButtonsPanel()
{
    delete editButton;
    delete layout;
}

} // m8r namespace
