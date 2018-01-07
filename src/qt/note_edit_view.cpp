/*
 note_edit.cpp     MindForger thinking notebook

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
#include "note_edit_view.h"

namespace m8r {

inline
bool caseInsensitiveLessThan(const QString &a, const QString &b)
{
    return a.compare(b, Qt::CaseInsensitive) < 0;
}

NoteEditView::NoteEditView(QWidget* parent, bool doClashingConnections)
    : QPlainTextEdit(parent), completedAndSelected(false)
{
    createWidgets();
    createConnections(doClashingConnections);
    highlightCurrentLine();
}

void NoteEditView::createWidgets()
{
    (void)new NoteEditHighlight(document());

    model = new QStringListModel(this);
    completer = new QCompleter(this);
    completer->setWidget(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setModel(model);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(true);
}

void NoteEditView::createConnections(bool doClashingConnections)
{
    QObject::connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(highlightCurrentLine()));
    QObject::connect(completer, SIGNAL(activated(const QString&)),
            this, SLOT(insertCompletion(const QString&)));

    // IMPROVE from text to Qt key constants
    new QShortcut(
        QKeySequence(tr("Alt+/", "Complete")),
        this, SLOT(performCompletion()));
    // IMPROVE from text to Qt key constants
    new QShortcut(
        QKeySequence(tr("Alt+LEFT", "Save and Close Editor")),
        this, SLOT(slotSaveAndCloseEditor()));

    if(doClashingConnections) {
        // IMPROVE from text to Qt key constants
        new QShortcut(
            QKeySequence(tr("Ctrl+s", "Save")),
            this, SLOT(slotSaveNote()));
    }
}

void NoteEditView::keyPressEvent(QKeyEvent *event)
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

bool NoteEditView::handledCompletedAndSelected(QKeyEvent *event)
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

void NoteEditView::slotSaveAndCloseEditor(void)
{
    emit signalSaveAndCloseEditor();
}

void NoteEditView::slotSaveNote(void)
{
    emit signalSaveNote();
}

void NoteEditView::performCompletion()
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    const QString completionPrefix = cursor.selectedText();

    if (!completionPrefix.isEmpty() && completionPrefix.at(completionPrefix.length() - 1).isLetter()) {
        performCompletion(completionPrefix);
    }
}

void NoteEditView::performCompletion(const QString& completionPrefix)
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

void NoteEditView::populateModel(const QString& completionPrefix)
{
    QStringList strings = toPlainText().split(QRegExp("\\W+"));
    strings.removeAll(completionPrefix);
    strings.removeDuplicates();
    qSort(strings.begin(), strings.end(), caseInsensitiveLessThan);
    model->setStringList(strings);
}

void NoteEditView::insertCompletion(const QString& completion, bool singleWord)
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

void NoteEditView::mousePressEvent(QMouseEvent* event)
{
    if (completedAndSelected) {
        completedAndSelected = false;
        QTextCursor cursor = textCursor();
        cursor.removeSelectedText();
        setTextCursor(cursor);
    }
    QPlainTextEdit::mousePressEvent(event);
}

void NoteEditView::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    QBrush highlightColor = palette().alternateBase();
    selection.format.setBackground(highlightColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
    setExtraSelections(extraSelections);
}

}
