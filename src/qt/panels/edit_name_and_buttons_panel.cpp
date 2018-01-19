/*
 edit_name_and_buttons_panel.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "edit_name_and_buttons_panel.h"

namespace m8r {

using namespace std;

EditTitleAndButtonsPanel::EditTitleAndButtonsPanel(QWidget* parent)
    : QWidget(parent)
{
    // widgets
    label = new QLabel{tr("Name:"), this};
    lineEdit = new QLineEdit{parent};
    moreButton = new QPushButton{tr("More..."), this};
    rememberButton = new QPushButton{tr("Remember"), this};
    cancelButton = new QPushButton{tr("Cancel"), this};

    // assembly
    layout = new QHBoxLayout{this};
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(moreButton);
    layout->addWidget(rememberButton);
    layout->addWidget(cancelButton);
    setLayout(layout);

    // signals
    QObject::connect(moreButton, SIGNAL(clicked()), this, SLOT(slotNoteEditDialog()));
}

EditTitleAndButtonsPanel::~EditTitleAndButtonsPanel()
{
    delete label;
    delete lineEdit;
    delete moreButton;
    delete rememberButton;
    delete cancelButton;
    delete layout;
}

void EditTitleAndButtonsPanel::slotNoteEditDialog(void)
{
    noteEditDialog->show();
}

}
