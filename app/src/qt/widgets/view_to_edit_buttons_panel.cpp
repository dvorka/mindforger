/*
 view_to_edit_buttons_panel.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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

ViewToEditEditButtonsPanel::ViewToEditEditButtonsPanel(
    MfWidgetMode mode, QWidget* parent
) : QWidget(parent),
      mode(mode)
{
    layout = new QHBoxLayout{this};

    // widgets
    if(MfWidgetMode::NOTE_MODE == mode) {
#ifdef __APPLE__
        showOutlineHeaderButton = new QPushButton{tr("View Notebook Header"), this};
#else
        showOutlineHeaderButton = new QPushButton{tr("View Notebook"), this};
#endif
        showOutlineHeaderButton->setToolTip(
            tr("Show preview of Notebook name and its description")
        );

        layout->addWidget(showOutlineHeaderButton);
    }

    editButton = new QPushButton{tr("&Edit"), this};
#ifdef __APPLE__
    editButton->setToolTip("⌘+E");
#else
    editButton->setToolTip("Ctrl+E");
#endif

    if(MfWidgetMode::OUTLINE_MODE == mode) {
#ifdef __APPLE__
        toggleFullOPreviewButton = new QPushButton{
            tr("Full / Header Notebook Preview"), this};
        // IMPROVE editButton->setToolTip("⌘+P");
#else
        toggleFullOPreviewButton = new QPushButton{tr("Whole Notebook &Preview"), this};
#endif
        toggleFullOPreviewButton->setToolTip(
            tr("Show whole Notebook preview or Notebook header preview")
        );

        layout->addWidget(toggleFullOPreviewButton);
    }

    lastNoteButton = new QPushButton{"↓", this};
    layout->addWidget(lastNoteButton);
    nextNoteButton = new QPushButton{"↑", this};
    layout->addWidget(nextNoteButton);

    layout->addWidget(editButton);

    setLayout(layout);
}

ViewToEditEditButtonsPanel::~ViewToEditEditButtonsPanel()
{
    delete editButton;
    delete layout;
}

} // m8r namespace
