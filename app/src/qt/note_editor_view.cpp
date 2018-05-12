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

using namespace std;

inline
bool caseInsensitiveLessThan(const QString &a, const QString &b)
{
    return a.compare(b, Qt::CaseInsensitive) < 0;
}

NoteEditorView::NoteEditorView(QWidget* parent)
    : QPlainTextEdit(parent), parent(parent), completedAndSelected(false)
{
    hitCounter = 0;

    // font
    f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    // alternatives:
    //f.setFamily("Courier");
    //f.setStyleHint(QFont::Monospace);
    //f.setFixedPitch(true);
    //f.setPointSize(10);
    setFont(f);

    setEditorTabWidth(Configuration::getInstance().getUiEditorTabWidth());

    createWidgets();
    createConnections();
    highlightCurrentLine();

    updateLineNumberPanelWidth(0);
}

void NoteEditorView::createWidgets()
{
    highlighter = new NoteEditHighlight{document()};
    // syntax highligting
    enableSyntaxHighlighting = Configuration::getInstance().isUiEditorEnableSyntaxHighlighting();
    highlighter->setEnabled(enableSyntaxHighlighting);

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

void NoteEditorView::wrapSelectedText(const QString &tag, const QString &endTag)
{
    QTextCursor cursor = textCursor();
    QTextDocument *doc = document();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    if (cursor.hasSelection() && doc->findBlock(start) == doc->findBlock(end)) {
        cursor.beginEditBlock();
        QString text = cursor.selectedText();
        text.prepend(tag);
        if(endTag.size()) text.append(endTag); else text.append(tag);
        cursor.insertText(text);
        cursor.endEditBlock();
        cursor.setPosition(start + tag.length());
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, end - start);
        setTextCursor(cursor);
    } else if (!cursor.hasSelection()) {
        if(endTag.size()) cursor.insertText(tag+endTag); else cursor.insertText(tag+tag);
        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, tag.length());
        setTextCursor(cursor);
    }

    setFocus();
}

void NoteEditorView::insertMarkdownText(const QString &text, bool newLine, int offset)
{
    QTextCursor cursor = textCursor();
    if(cursor.hasSelection()) {
        cursor.clearSelection();
    }
    if(newLine) {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::Down);
    }
    cursor.insertText(text);
    cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, text.length()-offset);
    setTextCursor(cursor);

    setFocus();
}

void NoteEditorView::setShowLineNumbers(bool show)
{
    showLineNumbers = show;
    lineNumberPanel->setVisible(showLineNumbers);
}

QString NoteEditorView::getRelevantWords() const
{
    // IMPROVE get whole line and cut word on which is curser and it before/after siblings: return textCursor().block().text(); ...
    QString result{};
    if(textCursor().block().text().size()) {
        QString t = textCursor().block().text();
        int c = textCursor().positionInBlock();
        if(t[c]!=' ') {
            // extend c to LEFT and to RIGHT
            for(int i=c-1; i>=0; i--) {
                if(t[i]==' ') break;
                result.prepend(t[i]);
            }
            for(int i=c; i<t.size(); i++) {
                if(t[i]==' ') break;
                result += t[i];
            }
        }
    }
    return result;
    //return textCursor().block().text();
}

void NoteEditorView::setEditorTabWidth(int tabWidth)
{
    MF_DEBUG("SETTING tabstop: " << tabWidth << endl);

    // tab width: 4 or 8
    QFontMetrics metrics(f);
    setTabStopWidth(tabWidth * metrics.width(' '));
}

void NoteEditorView::slotConfigurationUpdated()
{
    MF_DEBUG("CONFIG UPDATED @ editor " << endl);

    enableSyntaxHighlighting = Configuration::getInstance().isUiEditorEnableSyntaxHighlighting();
    highlighter->setEnabled(enableSyntaxHighlighting);

    setEditorTabWidth(Configuration::getInstance().getUiEditorTabWidth());
}

void NoteEditorView::keyPressEvent(QKeyEvent *event)
{
    hitCounter++;

    // IMPROVE get configuration reference and editor mode setting - this must be fast
    if(Configuration::getInstance().getEditorKeyBinding()==Configuration::EditorKeyBindingMode::EMACS) {
        if(event->modifiers() & Qt::ControlModifier){
            switch (event->key()) {
            case Qt::Key_A:
                moveCursor(QTextCursor::StartOfLine);
                return; // exit to override default key binding
            }
        }
    }

    if(completedAndSelected && handledCompletedAndSelected(event)) {
        return;
    } else {
        completedAndSelected = false;
    }

    if(completer->popup()->isVisible()) {
        switch(event->key()) {
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
    } else {
        // IMPROVE get configuration reference and setting - this must be fast
        if(Configuration::getInstance().getMindState()==Configuration::MindState::THINKING) {
            // TODO automatic completion suggestions when thinking - to be FIXED: doubled chars, backspace doesn't work, ...
            //if(performCompletion()) {
            //    event->ignore();
            //}
        }
    }

    QPlainTextEdit::keyPressEvent(event);
}

bool NoteEditorView::handledCompletedAndSelected(QKeyEvent *event)
{
    completedAndSelected = false;

    QTextCursor cursor = textCursor();
    switch(event->key()) {
    case Qt::Key_Space:
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

bool NoteEditorView::performCompletion()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    const QString completionPrefix = cursor.selectedText();
    if(!completionPrefix.isEmpty() && completionPrefix.at(completionPrefix.length()-1).isLetter()) {
        performCompletion(completionPrefix);
        return true;
    } else {
        return false;
    }
}

void NoteEditorView::performCompletion(const QString& completionPrefix)
{
    populateModel(completionPrefix);

    if(completionPrefix != completer->completionPrefix()) {
        completer->setCompletionPrefix(completionPrefix);
        completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
    }

    if(completer->completionCount() == 1) {
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
    if(completedAndSelected) {
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
        extraSelections += selection;

        // IMPROVE if line number changed
        if(isVisible()) {
            QString m{"    "};
            m += QString::number(textCursor().blockNumber());
            m += ":";
            m += QString::number(textCursor().positionInBlock());
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

} // m8r namespace
