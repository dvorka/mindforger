/*
 note_editor_view.cpp     MindForger thinking notebook

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
#include "note_editor_view.h"

namespace m8r {

inline
bool caseInsensitiveLessThan(const QString &a, const QString &b)
{
    return a.compare(b, Qt::CaseInsensitive) < 0;
}

NoteEditorView::NoteEditorView(QWidget* parent)
    : QPlainTextEdit(parent), parent(parent), completedAndSelected(false)
{
    // font
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    // alternatively
    //font.setFamily("Courier");
    //font.setStyleHint(QFont::Monospace);
    //font.setFixedPitch(true);
    //font.setPointSize(10);

    setFont(font);

    // tab width 4 vs 8
    // IMPROVE move this constant to Configuration (number is number of characters)
    const int tabStop = 4;
    QFontMetrics metrics(font);
    setTabStopWidth(tabStop * metrics.width(' '));

    createWidgets();
    createConnections();
    highlightCurrentLine();

    updateLineNumberPanelWidth(0);
}

void NoteEditorView::createWidgets()
{
    highlighter = new NoteEditHighlight{document()};

    lineNumberPanel = new LineNumberPanel{this};
    lineNumberPanel->setVisible(showLineNumbers);

    model = new QStringListModel{this};
    completer = new QCompleter{this};
    completer->setWidget(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setModel(model);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(true);
}

void NoteEditorView::createConnections()
{
    QObject::connect(
        this, SIGNAL(blockCountChanged(int)),
        this, SLOT(updateLineNumberPanelWidth(int)));
    QObject::connect(
        this, SIGNAL(updateRequest(QRect,int)),
        this, SLOT(updateLineNumberPanel(QRect,int)));

    QObject::connect(
        this, SIGNAL(cursorPositionChanged()),
        this, SLOT(highlightCurrentLine()));
    QObject::connect(
        completer, SIGNAL(activated(const QString&)),
        this, SLOT(insertCompletion(const QString&)));
    new QShortcut(
        QKeySequence(QKeySequence(Qt::ALT+Qt::Key_Slash)),
        this, SLOT(performCompletion()));
}

void NoteEditorView::setShowLineNumbers(bool show)
{
    showLineNumbers = show;
    lineNumberPanel->setVisible(showLineNumbers);
}

void NoteEditorView::setEnableSyntaxHighlighting(bool enable)
{
    enableSyntaxHighlighting = enable;
    highlighter->setEnabled(enableSyntaxHighlighting);
}

void NoteEditorView::keyPressEvent(QKeyEvent *event)
{
    if (completedAndSelected && handledCompletedAndSelected(event)) {
        return;
    } else {
        completedAndSelected = false;
    }

    if (completer->popup()->isVisible()) {
        switch (event->key()) {
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
                event->ignore();
                return;
            default:
                completer->popup()->hide();
                break;
        }
    }
    QPlainTextEdit::keyPressEvent(event);
}

bool NoteEditorView::handledCompletedAndSelected(QKeyEvent *event)
{
    completedAndSelected = false;

    QTextCursor cursor = textCursor();
    switch(event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            cursor.clearSelection();
            break;
        case Qt::Key_Escape:
            cursor.removeSelectedText();
            break;
        default:
            return false;
    }

    setTextCursor(cursor);
    event->accept();
    return true;
}

void NoteEditorView::performCompletion()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    const QString completionPrefix = cursor.selectedText();

    if (!completionPrefix.isEmpty() && completionPrefix.at(completionPrefix.length() - 1).isLetter()) {
        performCompletion(completionPrefix);
    }
}

void NoteEditorView::performCompletion(const QString& completionPrefix)
{
    populateModel(completionPrefix);

    if (completionPrefix != completer->completionPrefix()) {
        completer->setCompletionPrefix(completionPrefix);
        completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
    }

    if (completer->completionCount() == 1) {
        insertCompletion(completer->currentCompletion(), true);
    } else {
        QRect rect = cursorRect();
        rect.setWidth(
            completer->popup()->sizeHintForColumn(0) +
            completer->popup()->verticalScrollBar()->sizeHint().width());
        completer->complete(rect);
    }
}

void NoteEditorView::populateModel(const QString& completionPrefix)
{
    QStringList strings = toPlainText().split(QRegExp{"\\W+"});
    strings.removeAll(completionPrefix);
    strings.removeDuplicates();
    qSort(strings.begin(), strings.end(), caseInsensitiveLessThan);
    model->setStringList(strings);
}

void NoteEditorView::insertCompletion(const QString& completion, bool singleWord)
{
    QTextCursor cursor = textCursor();
    int numberOfCharsToComplete
        = completion.length() - completer->completionPrefix().length();
    int insertionPosition
        = cursor.position();
    cursor.insertText(completion.right(numberOfCharsToComplete));
    if(singleWord) {
        cursor.setPosition(insertionPosition);
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        completedAndSelected = true;
    }
    setTextCursor(cursor);
}

void NoteEditorView::mousePressEvent(QMouseEvent* event)
{
    if (completedAndSelected) {
        completedAndSelected = false;
        QTextCursor cursor = textCursor();
        cursor.removeSelectedText();
        setTextCursor(cursor);
    }
    QPlainTextEdit::mousePressEvent(event);
}

void NoteEditorView::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    if(!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        //QColor lineColor = QColor(Qt::yellow).lighter(160);
        QBrush highlightColor = palette().alternateBase();
        selection.format.setBackground(highlightColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);

        // IMPROVE if line number changed
        if(isVisible()) {
            QString m{"    "};
            m.append(QString::number(textCursor().blockNumber()));
            m.append(":");
            m.append(QString::number(textCursor().positionInBlock()));
            statusBar->showInfo(m);
        }
    }
    setExtraSelections(extraSelections);
}

/*
 * Line number rendering
 */

int NoteEditorView::lineNumberPanelWidth()
{
    if(showLineNumbers) {
        int digits = 1;
        int max = qMax(1, blockCount());
        while(max >= 10) {
            max /= 10;
            ++digits;
        }
        int space = 3 + fontMetrics().width(QLatin1Char{'9'}) * digits;
        return space;
    } else {
        return 0;
    }
}

void NoteEditorView::updateLineNumberPanelWidth(int newBlockCount)
{
    UNUSED_ARG(newBlockCount);

    // IMPROVE comment to parameter and ignore macro
    setViewportMargins(lineNumberPanelWidth(), 0, 0, 0);
}

void NoteEditorView::updateLineNumberPanel(const QRect& r, int deltaY)
{
    if(showLineNumbers) {
        if(deltaY) {
            lineNumberPanel->scroll(0, deltaY);
        } else {
            lineNumberPanel->update(0, r.y(), lineNumberPanel->width(), r.height());
        }

        if (r.contains(viewport()->rect())) {
            updateLineNumberPanelWidth(0);
        }
    }
}

void NoteEditorView::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect contents = contentsRect();
    lineNumberPanel->setGeometry(QRect(contents.left(), contents.top(), lineNumberPanelWidth(), contents.height()));
}

void NoteEditorView::lineNumberPanelPaintEvent(QPaintEvent* event)
{
    QPainter painter(lineNumberPanel);
    painter.fillRect(event->rect(), LookAndFeels::getInstance().getEditorLineNumbersBackgroundColor());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while(block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(LookAndFeels::getInstance().getEditorLineNumbersForegroundColor());
            painter.drawText(0, top, lineNumberPanel->width(), fontMetrics().height(), Qt::AlignCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)blockBoundingRect(block).height();
        ++blockNumber;
    }
}

} // namespace
