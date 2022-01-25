/*
 note_smart_editor.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "note_smart_editor.h"

namespace m8r {

using namespace std;

NoteSmartEditor::NoteSmartEditor(QPlainTextEdit& textEdit)
    : textEdit{textEdit},
      tabWidth{Configuration::getInstance().getUiEditorTabWidth()},
      tabsAsSpaces{Configuration::getInstance().isUiEditorTabsAsSpaces()}
{
}

NoteSmartEditor::~NoteSmartEditor()
{
}

bool NoteSmartEditor::isLineSpacesOnly(const QString& line)
{
    if(line.size()) {
        int i;
        for(i=0; i<line.size() && line[i]==' '; i++) ;
        if(i == line.size()) {
            return true;
        }
    }
    return false;
}

bool NoteSmartEditor::isLineStartsWithTabSpaces(const QString& line)
{
    if(line.size()) {
        int i;
        for(i=0; i<line.size() && i<=this->tabWidth && line[i]==' '; i++) ;
        if(i >= this->tabWidth) {
            return true;
        }
    }
    return false;
}

QString NoteSmartEditor::getNextChar()
{
    QString nextChar{};

    QTextCursor cursor = textEdit.textCursor();
    if(cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor)) {
        nextChar = cursor.selectedText();
        cursor.clearSelection();
    }
    textEdit.setTextCursor(cursor);

    return nextChar;
}

QString NoteSmartEditor::getLastChar()
{
    QString lastChar{};

    QTextCursor cursor = textEdit.textCursor();
    if(cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor)) {
        lastChar = cursor.selectedText();
        cursor.clearSelection();
        cursor.movePosition(QTextCursor::NextCharacter);
    }
    textEdit.setTextCursor(cursor);

    MF_DEBUG("Smart editor last char: '" << lastChar.toStdString() << "'" << endl);

    return lastChar;
}

void NoteSmartEditor::insertTab()
{
    QString completion{};
    if(this->tabWidth == 8) {
        completion.append("        ");
    } else {
        completion.append("    ");
    }
    textEdit.textCursor().insertText(completion);
}

void NoteSmartEditor::currentLineRemove()
{
    QTextCursor cursor = textEdit.textCursor();

    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();

    textEdit.setTextCursor(cursor);
}

bool NoteSmartEditor::completeLineWithSpaces(QString& line)
{
    // fill only if cursor not at the beginning of line
    MF_DEBUG("Smart editor: cursor position @ fill " << textEdit.textCursor().positionInBlock() << endl);
    if(textEdit.textCursor().positionInBlock() && line.startsWith(" ")) {
        // count the number of spaces: delete line if spaces only, indent otherwise
        for(int i=0; i<line.size(); i++) {
            if(line[i] != ' ') {
                string s{"\n"};
                for(int is=0; is<i; is++) {
                    s+=" ";
                }
                textEdit.textCursor().insertText(s.c_str());
                return true;
            }
        }
        textEdit.textCursor().select(QTextCursor::BlockUnderCursor);
        textEdit.textCursor().removeSelectedText();
        textEdit.textCursor().insertText("\n");
        return true;
    }
    return false;
}

bool NoteSmartEditor::moveSelectedLinesLeftByTab()
{
    MF_DEBUG("Editor: backtab WITH selection" << endl);

#ifdef MF_WIP
    // TODO move all lines in the selection
    throw "Not implemented";

    // process selection IF every line starts with empty character - helper function
    //    remove by size of tab OR by one if smaller
#endif

    return false;
}

bool NoteSmartEditor::moveLineLeftByTab()
{
    MF_DEBUG("Editor: backtab WITHOUT selection" << endl);

    QString lineStr{};
    if(getCurrentLineText(lineStr)) {
        if(isLineStartsWithTabSpaces(lineStr)) {
            // if there are more spaces than tab width, delete one tab
           textEdit.moveCursor(QTextCursor::StartOfLine);
           for(int i=0; i<this->tabWidth; i++) {
               textEdit.moveCursor(QTextCursor::Right, QTextCursor::KeepAnchor);
           }
           textEdit.textCursor().removeSelectedText();
           return true;
        } else {
            // if there are less spaces than tab width, delete one space
            if(lineStr.startsWith(" ")) {
                textEdit.moveCursor(QTextCursor::StartOfLine);
                textEdit.textCursor().deleteChar();
                return true;
            }
        }
    }
    return false;
}

bool NoteSmartEditor::moveLineRightByTab()
{
    if(textEdit.textCursor().hasSelection()) {
        return moveSelectedLinesRightByTab();
    } else {
        insertTab();
        return true;
    }
    return false;
}

bool NoteSmartEditor::moveSelectedLinesRightByTab()
{
#ifdef MF_WIP
    MF_DEBUG("Editor: move selection RIGHT not implemented" << endl);
    throw "Not implemented";
#endif

    return false;
}

bool NoteSmartEditor::isLineCountOfGivenCharOdd(string c)
{
    UNUSED_ARG(c);

#ifdef MF_WIP
    MF_DEBUG("Editor: odd chars counter TBD" << endl);
    throw "Not implemented";
#endif

    return false;
}

bool NoteSmartEditor::completePairChars(QKeyEvent* event) {
#ifdef MF_SMART_EDITOR
    switch (event->key()) {
        case Qt::Key_ParenLeft:
            textEdit.textCursor().insertText("()");
            textEdit.moveCursor(QTextCursor::PreviousCharacter);
            return true;
        case Qt::Key_BraceLeft:
            textEdit.textCursor().insertText("{}");
            textEdit.moveCursor(QTextCursor::PreviousCharacter);
            return true;
        case Qt::Key_BracketLeft:
            textEdit.textCursor().insertText("[]");
            textEdit.moveCursor(QTextCursor::PreviousCharacter);
            return true;
        case Qt::Key_Less:
            textEdit.textCursor().insertText("<>");
            textEdit.moveCursor(QTextCursor::PreviousCharacter);
            return true;
        /* SKIPPED: does not help when creating bulleted lists
           TODO re-enable, detect beginning of line and create list, ** otherwise
        case Qt::Key_Asterisk:
            textEdit.textCursor().insertText("**");
            textEdit.moveCursor(QTextCursor::PreviousCharacter);
            return true;
        */
        case Qt::Key_Underscore:
            textEdit.textCursor().insertText("__");
            textEdit.moveCursor(QTextCursor::PreviousCharacter);
            return true;
        case Qt::Key_AsciiTilde:
            textEdit.textCursor().insertText("~~");
            textEdit.moveCursor(QTextCursor::PreviousCharacter);
            return true;
        case Qt::Key_QuoteDbl:
            textEdit.textCursor().insertText("\"\"");
            textEdit.moveCursor(QTextCursor::PreviousCharacter);
            return true;
        case Qt::Key_QuoteLeft:
            // if at the beginning of current line
            if(isAtTheBeginningOfLine()) {
                textEdit.textCursor().insertText("```");
                textEdit.moveCursor(QTextCursor::EndOfLine);
            } else {
                // if previous char is space and user wants to start new block
                if(getLastChar().toStdString() == " ") {
                    textEdit.textCursor().insertText("``");
                    textEdit.moveCursor(QTextCursor::PreviousCharacter);
                } else {
                    // normal handling will insert `
                    return false;
                }
            }
            return true;
        /* SKIPPED: does not help when writing: don't doesn't hasn't .. >> detect previous/next non-spaces as ^ and re-enable
           TODO fix according to `  which will make it useful
        case Qt::Key_Apostrophe:
            textEdit.textCursor().insertText("''");
            textEdit.moveCursor(QTextCursor::PreviousCharacter);
            return;
        */
        case Qt::Key_Backtab:
            if(textEdit.textCursor().hasSelection()) {
                if(moveSelectedLinesLeftByTab()) {
                    return true;
                }
            } else {
                if(moveLineLeftByTab()) {
                    return true;
                }
            }
            break;
    }
#endif
    return false;
}

bool NoteSmartEditor::getCurrentLineText(QString& text)
{
    const QTextBlock block = textEdit.textCursor().block();
    if(block.isValid() && block.layout()) {
        text = block.text();
        return true;
    }
    return false;
}

bool NoteSmartEditor::currentLineRemoveIfSpacesOnly()
{
    QString line{};
    if(getCurrentLineText(line)) {
        if(isLineSpacesOnly(line)) {
            currentLineRemove();
            return true;
        }
    }
    return false;
}


bool NoteSmartEditor::completeListAndFenceBlocks(QKeyEvent* event)
{
    UNUSED_ARG(event);

    MF_DEBUG(
       "SMART EDITOR: current line '" << textEdit.textCursor().block().text().toStdString() << "'"
        << endl
    );

    const QTextBlock block = textEdit.textCursor().block();

    if(block.isValid() && block.layout()) {
        QString lineStr{block.text()};
        // indented block / code fence autocomplete
        if(completeLineWithSpaces(lineStr)) {
            return true;
        // numbered list autocomplete
        } else if(lineStr.startsWith("1. ")) {
            textEdit.textCursor().insertText("\n   ");
            return true;
        // bulletted list autocomplete
        } else if(lineStr.startsWith("* ") || lineStr.startsWith("- ")) {
            textEdit.textCursor().insertText("\n  ");
            return true;
        }
    }

    return false;
}

} // m8r namespace
