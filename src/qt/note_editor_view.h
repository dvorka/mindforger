/*
 note_editor_view.h     MindForger thinking notebook

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
#ifndef M8RUI_NOTE_EDITOR_VIEW_H
#define M8RUI_NOTE_EDITOR_VIEW_H

#include <QtWidgets>

#include "note_edit_highlight.h"

namespace m8r {

class NoteEditorView : public QPlainTextEdit
{
    Q_OBJECT

private:
    bool completedAndSelected;
    QCompleter *completer;
    QStringListModel *model;

public:
    explicit NoteEditorView(QWidget* parent, bool doClashingConnections=true);
    NoteEditorView(const NoteEditorView&) = delete;
    NoteEditorView(const NoteEditorView&&) = delete;
    NoteEditorView &operator=(const NoteEditorView&) = delete;
    NoteEditorView &operator=(const NoteEditorView&&) = delete;

protected:
    void mousePressEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);

private slots:
    void insertCompletion(const QString& completion, bool singleWord=false);
    void highlightCurrentLine(void);
    void performCompletion(void);
    void slotSaveNote(void);
    void slotSaveAndCloseEditor(void);

signals:
    void signalSaveAndCloseEditor(void);
    void signalSaveNote(void);

private:
    void createWidgets();
    void createConnections(bool doClashingConnections);
    QString textUnderCursor() const;
    void performCompletion(const QString &completionPrefix);
    bool handledCompletedAndSelected(QKeyEvent *event);
    void populateModel(const QString &completionPrefix);
};

}

#endif // M8RUI_NOTE_EDITOR_VIEW_H
