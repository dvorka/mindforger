/*
 edit_buttons_panel.cpp     MindForger thinking notebook

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
#include "edit_buttons_panel.h"

namespace m8r {

using namespace std;

EditButtonsPanel::EditButtonsPanel(MfWidgetMode mode, QWidget* parent)
    : QWidget(parent), mode(mode)
{
    // widgets
    previewButton = new QPushButton{tr("Preview"), this};
    previewButton->setToolTip("Ctrl+Shift+P");
    moreButton = new QPushButton{tr("Properties"), this};
    moreButton->setToolTip("Alt+Enter");
    rememberButton = new QPushButton{tr("Remember"), this};
    rememberButton->setToolTip("Alt+Left");
    cancelButton = new QPushButton{tr("Cancel"), this};
    cancelButton->setToolTip("Ctrl+G");

    // assembly
    layout = new QHBoxLayout{this};
    layout->addStretch(1);
    layout->addWidget(cancelButton);
    layout->addWidget(moreButton);
    layout->addWidget(previewButton);
    layout->addWidget(rememberButton);
    setLayout(layout);

    // signals
    QObject::connect(previewButton, SIGNAL(clicked()), this, SLOT(handleShowLivePreview()));
    if(mode==MfWidgetMode::OUTLINE_MODE) {
        QObject::connect(moreButton, SIGNAL(clicked()), this, SLOT(handleShowOutlineHeaderEditDialog()));
    } else {
        QObject::connect(moreButton, SIGNAL(clicked()), this, SLOT(handleShowNoteEditDialog()));
    }
}

EditButtonsPanel::~EditButtonsPanel()
{
    delete moreButton;
    delete previewButton;
    delete rememberButton;
    delete cancelButton;
    delete layout;
}

void EditButtonsPanel::handleShowLivePreview()
{
    emit signalShowLivePreview();
}

void EditButtonsPanel::handleShowOutlineHeaderEditDialog()
{
    outlineHeaderEditDialog->show();
}

void EditButtonsPanel::handleCloseOutlineHeaderEditDialog()
{
    outlineHeaderEditDialog->toOutline();
}

void EditButtonsPanel::handleShowNoteEditDialog()
{
    noteEditDialog->show();
}

void EditButtonsPanel::handleCloseNoteEditDialog()
{
    noteEditDialog->toNote();
}

} // m8r namespace
