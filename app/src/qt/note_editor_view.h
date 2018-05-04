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

#include "../../lib/src/gear/lang_utils.h"

#include "note_edit_highlight.h"
#include "widgets/line_number_panel.h"
#include "status_bar_view.h"

namespace m8r {

class LineNumberPanel;

class NoteEditorView : public QPlainTextEdit
{
    Q_OBJECT

private:
    QWidget* parent;
    QFont f;

    bool completedAndSelected;
    QCompleter* completer;
    QStringListModel* model;

    NoteEditHighlight* highlighter;
    const StatusBarView* statusBar;

    bool showLineNumbers;
    bool enableSyntaxHighlighting;

    int hitCounter;

public:
    explicit NoteEditorView(QWidget* parent);
    NoteEditorView(const NoteEditorView&) = delete;
    NoteEditorView(const NoteEditorView&&) = delete;
    NoteEditorView &operator=(const NoteEditorView&) = delete;
    NoteEditorView &operator=(const NoteEditorView&&) = delete;

    QString getSelectedText() const { return textCursor().selectedText(); }
    void wrapSelectedText(const QString &tag) { wrapSelectedText(tag,""); }
    void wrapSelectedText(const QString &tag, const QString &endTag);
    void insertMarkdownText(const QString &text, bool newLine=true, int offset=0);

    void setShowLineNumbers(bool show);
    void setStatusBar(const StatusBarView* sb) { this->statusBar = sb; }

    void clearHitCounter() { hitCounter=0; }
    int getHitCounter() const { return hitCounter; }
    QString getRelevantWords() const;

protected:
    void mousePressEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
private:
    void createWidgets();
    void createConnections();
    QString textUnderCursor() const;
    void setEditorTabWidth(int tabWidth);
    void performCompletion(const QString &completionPrefix);
    bool handledCompletedAndSelected(QKeyEvent *event);
    void populateModel(const QString &completionPrefix);

public slots:
    void slotConfigurationUpdated();
private slots:
    void insertCompletion(const QString& completion, bool singleWord=false);
    void highlightCurrentLine();
    void performCompletion();

// line number panel
private:
    LineNumberPanel* lineNumberPanel;
public:
    void lineNumberPanelPaintEvent(QPaintEvent *event);
    int lineNumberPanelWidth();
protected:
    void resizeEvent(QResizeEvent *event) override;
private slots:
    void updateLineNumberPanelWidth(int newBlockCount);
    void updateLineNumberPanel(const QRect &, int);
};

} // m8r namespace

#endif // M8RUI_NOTE_EDITOR_VIEW_H
