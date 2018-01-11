/*
 note_edit_presenter.h     MindForger thinking notebook

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
#ifndef M8RUI_NOTE_EDIT_PRESENTER_H
#define M8RUI_NOTE_EDIT_PRESENTER_H

#include "lib/src/mind/mind.h"
#include "lib/src/representations/markdown/markdown_outline_representation.h"

#include <QtWidgets>

#include "orloj_presenter.h"
#include "note_editor_view.h"
#include "note_edit_model.h"

namespace m8r {

class OrlojPresenter;

class NoteEditPresenter : public QObject
{
    Q_OBJECT

private:
    NoteEditorView* view;
    NoteEditModel* model;
    MainWindowPresenter* mainPresenter;

    MarkdownOutlineRepresentation* mdRepresentation;

    Note* currentNote;

public:
    NoteEditPresenter(NoteEditorView* view, MainWindowPresenter* mainPresenter, QObject* parent);
    NoteEditPresenter(const NoteEditPresenter&) = delete;
    NoteEditPresenter(const NoteEditPresenter&&) = delete;
    NoteEditPresenter &operator=(const NoteEditPresenter&) = delete;
    NoteEditPresenter &operator=(const NoteEditPresenter&&) = delete;
    ~NoteEditPresenter();

    NoteEditModel* getModel(void) const { return model; }
    void setCurrentNote(Note* note, std::string* md);

public slots:
    void slotSaveAndCloseEditor(void);
    void slotSaveNote(void);
};

}

#endif // M8RUI_NOTE_EDIT_PRESENTER_H
