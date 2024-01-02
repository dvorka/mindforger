/*
 outline_header_edit_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "outline_header_edit_view.h"

namespace m8r {

OutlineHeaderEditView::OutlineHeaderEditView(QWidget* parent)
    : QWidget(parent)
{
    // widgets
    topNamePanel = new EditNamePanel{MfWidgetMode::OUTLINE_MODE, this};
    noteEditor = new NoteEditorView{this};
    bottomButtonsPanel = new EditButtonsPanel{MfWidgetMode::OUTLINE_MODE, this};

    // assembly
    QVBoxLayout* layout = new QVBoxLayout{this};
    // ensure that wont be extra space around member widgets
    layout->setContentsMargins(QMargins(0,0,0,0));
    layout->addWidget(topNamePanel);
    layout->addWidget(noteEditor);
    layout->addWidget(bottomButtonsPanel);
    setLayout(layout);

    // signals
#ifdef __APPLE__
    new QShortcut(
        QKeySequence(Qt::CTRL+Qt::Key_L),
        this, SLOT(slotSaveAndCloseEditor()));
#else
    new QShortcut(
        QKeySequence(Qt::ALT+Qt::Key_Left),
        this, SLOT(slotSaveAndCloseEditor()));
#endif
    new QShortcut(
        QKeySequence(Qt::CTRL+Qt::Key_G),
        this, SLOT(slotCloseEditor()));
    new QShortcut(
#if __APPLE__
        QKeySequence(Qt::CTRL+Qt::Key_Return),
#else
        QKeySequence(Qt::ALT+Qt::Key_Return),
#endif
        this, SLOT(slotOpenOutlineHeaderPropertiesEditor()));
    new QShortcut(
        QKeySequence(Qt::CTRL+Qt::Key_S),
        this, SLOT(slotSaveOutlineHeader()));
    QObject::connect(
        bottomButtonsPanel->getRememberButton(), SIGNAL(clicked()),
        this, SLOT(slotSaveOutlineHeader()));
    QObject::connect(
        bottomButtonsPanel->getRememberAndLeaveButton(), SIGNAL(clicked()),
        this, SLOT(slotSaveAndCloseEditor()));
    QObject::connect(
        bottomButtonsPanel->getCancelButton(), SIGNAL(clicked()),
        this, SLOT(slotCloseEditor()));

    QObject::connect(
        noteEditor, SIGNAL(signalCloseEditorWithEsc()),
        this, SLOT(slotCloseEditor()));
}

OutlineHeaderEditView::~OutlineHeaderEditView()
{
}

void OutlineHeaderEditView::slotOpenOutlineHeaderPropertiesEditor()
{
    bottomButtonsPanel->handleShowOutlineHeaderEditDialog();
}

void OutlineHeaderEditView::slotSaveAndCloseEditor()
{
    emit signalSaveAndCloseEditor();
}

void OutlineHeaderEditView::slotCloseEditor()
{
    emit signalCloseEditor();
}

void OutlineHeaderEditView::slotSaveOutlineHeader()
{
    emit signalSaveOutlineHeader();
}

}
