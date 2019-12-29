/*
 edit_name_and_buttons_panel.cpp     MindForger thinking notebook

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
#include "edit_name_and_buttons_panel.h"

namespace m8r {

using namespace std;

EditNameAndButtonsPanel::EditNameAndButtonsPanel(Mode mode, QWidget* parent)
    : QWidget(parent), mode(mode)
{
    // widgets
    label = new QLabel{tr("Name:"), this};
    lineEdit = new QLineEdit{parent};
    moreButton = new QPushButton{tr("More..."), this};
    moreButton->setToolTip("Alt+Enter");
    rememberButton = new QPushButton{tr("Remember"), this};
    rememberButton->setToolTip("Alt+Left");
    cancelButton = new QPushButton{tr("Cancel"), this};
    cancelButton->setToolTip("Ctrl+G");

    // assembly
    layout = new QHBoxLayout{this};
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(moreButton);
    layout->addWidget(rememberButton);
    layout->addWidget(cancelButton);
    setLayout(layout);

    // signals
    if(mode==Mode::OUTLINE_MODE) {
        QObject::connect(moreButton, SIGNAL(clicked()), this, SLOT(handleShowOutlineHeaderEditDialog()));
    } else {
        QObject::connect(moreButton, SIGNAL(clicked()), this, SLOT(handleShowNoteEditDialog()));
    }
}

EditNameAndButtonsPanel::~EditNameAndButtonsPanel()
{
    delete label;
    delete lineEdit;
    delete moreButton;
    delete rememberButton;
    delete cancelButton;
    delete layout;
}

void EditNameAndButtonsPanel::handleShowOutlineHeaderEditDialog()
{
    outlineHeaderEditDialog->show();
}

void EditNameAndButtonsPanel::handleCloseOutlineHeaderEditDialog()
{
    outlineHeaderEditDialog->toOutline();
}

void EditNameAndButtonsPanel::handleShowNoteEditDialog()
{
    noteEditDialog->show();
}

void EditNameAndButtonsPanel::handleCloseNoteEditDialog()
{
    noteEditDialog->toNote();
}

} // m8r namespace
