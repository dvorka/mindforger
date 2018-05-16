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

/**
 * @brief Note editor view.
 *
 * Editor is one of the key MindForger components - editor is where thinking notebook can
 * make difference: auto completion, associations, efficient editation and decent performance.
 */
class NoteEditorView : public QPlainTextEdit
{
    Q_OBJECT

private:
    QWidget* parent;

    QFont f;

    // usability
    bool enableSyntaxHighlighting;
    NoteEditHighlight* highlighter;
    bool showLineNumbers;
    LineNumberPanel* lineNumberPanel;

    // associations
    int hitCounter;

    // autocomplete
    bool completedAndSelected;
    QCompleter* completer;
    QStringListModel* model;

    const StatusBarView* statusBar;

public:
    explicit NoteEditorView(QWidget* parent);
    NoteEditorView(const NoteEditorView&) = delete;
    NoteEditorView(const NoteEditorView&&) = delete;
    NoteEditorView &operator=(const NoteEditorView&) = delete;
    NoteEditorView &operator=(const NoteEditorView&&) = delete;

    void setStatusBar(const StatusBarView* sb) { this->statusBar = sb; }

    // formatting
    QString getSelectedText() const { return textCursor().selectedText(); }
    void wrapSelectedText(const QString &tag) { wrapSelectedText(tag,""); }
    void wrapSelectedText(const QString &tag, const QString &endTag);
    void insertMarkdownText(const QString &text, bool newLine=true, int offset=0);

    // associations
    QString getRelevantWords() const;
    void clearHitCounter() { hitCounter=0; }
    int getHitCounter() const { return hitCounter; }

    // autocomplete
protected:
    void mousePressEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
private:
    void setEditorTabWidth(int tabWidth);
    void performCompletion(const QString& completionPrefix);
    bool handledCompletedAndSelected(QKeyEvent* event);
    void populateModel(const QString& completionPrefix);
private slots:
    void insertCompletion(const QString& completion, bool singleWord=false);
    bool performCompletion();

    // line & line number
public:
    void lineNumberPanelPaintEvent(QPaintEvent* event);
    int lineNumberPanelWidth();
protected:
    void resizeEvent(QResizeEvent* event) override;
private slots:
    void highlightCurrentLine();
    void updateLineNumberPanelWidth(int newBlockCount);
    void updateLineNumberPanel(const QRect&, int);

    // configuration
public:
    void setShowLineNumbers(bool show);
public slots:
    void slotConfigurationUpdated();
};

} // m8r namespace

#endif // M8RUI_NOTE_EDITOR_VIEW_H
