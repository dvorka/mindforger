/*
 note_smart_editor.h     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_NOTE_SMART_EDITOR_H
#define M8R_NOTE_SMART_EDITOR_H

#include "../../lib/src/debug.h"
#include "../../lib/src/config/configuration.h"
#include "../../lib/src/gear/string_utils.h"
#include "../../lib/src/representations/unicode.h"

#include <QtWidgets>

namespace m8r {

// SMART EDITOR is experimental feature
#define MF_SMART_EDITOR

/**
 * @brief Smart Markdown editor.
 *
 * Smart Markdown editor aims to make editation of Markdown documents
 * simpler by code completion, indentation and cursor navigation.
 */
class NoteSmartEditor
{
private:
    QPlainTextEdit& textEdit;

    // configuration: tab width (updated on every editor refresh)
    int tabWidth;
    // configuration: convert tabs to spaces (updated on every editor refresh)
    bool tabsAsSpaces;

public:
    explicit NoteSmartEditor(QPlainTextEdit& textEdit);
    NoteSmartEditor(const NoteSmartEditor&) = delete;
    NoteSmartEditor(const NoteSmartEditor&&) = delete;
    NoteSmartEditor& operator =(const NoteSmartEditor&) = delete;
    NoteSmartEditor& operator =(const NoteSmartEditor&&) = delete;
    ~NoteSmartEditor();

    /*
     * Configuration
     */

    void setTabWidth(int tabWidth) { this->tabWidth = tabWidth; }
    int getTabWidth() const { return tabWidth; }
    void setPolicyTabsAsSpaces(int tabsAsSpaces) { this->tabsAsSpaces = tabsAsSpaces; }
    bool isPolicyTabsAsSpaces() const { return tabsAsSpaces; }

    /*
     * Tests
     */

    bool isAtTheBeginningOfLine() {
        return textEdit.textCursor().atBlockStart();
    }

    bool isAtTheEndOfLine() {
        return textEdit.textCursor().atBlockEnd();
    }

    /**
     * @brief Is line made only by spaces?
     */
    bool isLineSpacesOnly(const QString& line);
    /**
     * @brief Does line starts with at lest TAB width spaces?
     */
    bool isLineStartsWithTabSpaces(const QString& line);
    /**
     * @brief Does line has ODD number of given character occurences?
     */
    bool isLineHasOddCharCount(const std::string c);

    /**
     * @brief Is the count of given characters on the current line odd?
     */
    bool isLineCountOfGivenCharOdd(std::string c);

    /*
     * Getters
     */

    /**
     * @brief Get current line text and indicate whether current line is valid.
     */
    bool getCurrentLineText(QString& text);

    /**
     * @brief Removing leading spaces.
     */
    QString chopLeadingSpaces(const QString text);

    /**
     * @brief Get next character - on the right from cursor - on the current line.
     */
    QString getNextChar();
    /**
     * @brief Get previous character - on the left from cursor - on the current line.
     */
    QString getLastChar();

    /*
     * Actions
     */

    /**
     * @brief Complete {[" characters and indicate completion.
     */
    bool completePairChars(QKeyEvent* event);
    /**
     * @brief Complete bulletted/numbered list or code fence and indicate completion.
     */
    bool completeListAndFenceBlocks(QKeyEvent* event);

    /**
     * @brief Complete current line with NEW line pre-filled with spaces (if desired).
     */
    bool completeLineWithSpaces(QString& currentLine);

    /**
     * @brief Get TAB as string made of spaces.
     */
    std::string getTabAsSpaces();

    /**
     * @brief Insert tab at current cursor position.
     */
    void insertTab();

    /**
     * @brief Do not add TAB blindly, but add spaces to the current position so that
     * their number is multiplication of TAB from the beginning of the line.
     */
    void smartInsertTab();

    /**
     * @brief Move block of text on current line behind cursor right by TAB spaces (on TAB).
     */
    bool moveLineRightByTab();
    /**
     * @brief Move selected block of text (possible multiple lines) right by TAB spaces (on TAB).
     */
    bool moveSelectedLinesRightByTab();

    /**
     * @brief Move current line to left by TAB spaces (on BACK TAB).
     */
    bool moveLineLeftByTab();
    /**
     * @brief Move selected lines to left by 1 space (on BACK TAB) as it's more practical than
     * move text left by TAB spaces.
     */
    bool moveSelectedLinesLeftByTab();

    /**
     * @brief Remove current line.
     */
    void currentLineRemove();
    /**
     * @brief Remove current line if made by spaces (e.g. on ENTER) and indicate action.
     */
    bool currentLineRemoveIfSpacesOnly();
};

}
#endif // M8R_NOTE_SMART_EDITOR_H
