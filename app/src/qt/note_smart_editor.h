/*
 note_smart_editor.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <QtWidgets>

namespace m8r {

// SMART EDITOR is experimental feature
#define MF_SMART_EDITOR

class NoteSmartEditor
{
private:
    QPlainTextEdit& textEdit;

    int tabWidth;
    bool tabsAsSpaces;

public:
    explicit NoteSmartEditor(QPlainTextEdit& textEdit);
    NoteSmartEditor(const NoteSmartEditor&) = delete;
    NoteSmartEditor(const NoteSmartEditor&&) = delete;
    NoteSmartEditor& operator =(const NoteSmartEditor&) = delete;
    NoteSmartEditor& operator =(const NoteSmartEditor&&) = delete;
    ~NoteSmartEditor();

    void setTabWidth(int tabWidth) { this->tabWidth = tabWidth; }
    int getTabWidth() const { return tabWidth; }
    void setTabsAsSpaces(int tabsAsSpaces) { this->tabsAsSpaces = tabsAsSpaces; }
    bool isPolicyTabsAsSpaces() const { return tabsAsSpaces; }

    /**
     * @brief Is line made only by spaces?
     */
    bool isLineSpacesOnly(const QString& line);
    /**
     * @brief Does line starts with at lest TAB width spaces?
     */
    bool isLineStartsWithTabSpaces(const QString& line);
    void insertTab();
    void removeLine();
    bool fillLineWithSpacesOnEnter(QString& lineStr);

    bool moveSelectionLeftOnBackTab();
    bool moveLineLeftOnBackTab();

    /**
     * @brief Move block of text behind cursor right (by TAB spaces) on TAB.
     */
    bool moveRightOnTab();
    /**
     * @brief Move selected block of text (possible multiple lines) right (by TAB spaces) on TAB.
     */
    bool moveSelectionRightOnTab();

    /**
     * @brief Complete {[" characters and indicate completion.
     */
    bool completePairChairs(QKeyEvent* event);
    /**
     * @brief Erase line made by spaces (e.g. on ENTER) and indicate action.
     */
    bool eraseSpacesLine(QKeyEvent* event);
    /**
     * @brief Complete bulletted/numbered list or code fence and indicate completion.
     */
    bool completeListAndFenceBlocks(QKeyEvent* event);
};

}
#endif // M8R_NOTE_SMART_EDITOR_H
