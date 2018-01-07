/*
 note_view.h     MindForger thinking notebook

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
#ifndef M8RUI_NOTE_VIEW_H
#define M8RUI_NOTE_VIEW_H

#include "lib/src/model/note.h"

#include "note_view_model.h"

#include <QtWidgets>
#include <QWebView>
#include <QUrl>

namespace m8r {

class NoteView: public QWebView
{
    Q_OBJECT

private:
    NoteViewModel* noteModel;

public:
    NoteView(QWidget* parent);
    NoteView(const NoteView&) = delete;
    NoteView(const NoteView&&) = delete;
    NoteView &operator=(const NoteView&) = delete;
    NoteView &operator=(const NoteView&&) = delete;

    void setModel(NoteViewModel* noteModel) { this->noteModel = noteModel; }

    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void keyPressEvent(QKeyEvent*) override;

signals:
    void signalMouseDoubleClickEvent(void);
    void signalFromViewNoteToOutlines(void);
};

}
#endif // M8RUI_NOTE_VIEW_H
