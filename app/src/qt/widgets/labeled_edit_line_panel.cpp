/*
 labeled_edit_line_panel.cpp     MindForger thinking notebook

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
#include "labeled_edit_line_panel.h"

namespace m8r {

LabeledEditLinePanel::LabeledEditLinePanel(const QString& text, QWidget *parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout{this};
    layout->setContentsMargins(0,0,0,0);
    label = new QLabel{text, this};
    edit = new QLineEdit{this};
    layout->addWidget(label);
    layout->addWidget(edit);
    setLayout(layout);
}

LabeledEditLinePanel::~LabeledEditLinePanel()
{
}

} // m8r namespace
