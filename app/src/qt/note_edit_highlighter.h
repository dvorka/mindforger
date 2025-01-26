/*
 note_edit_highlighter.h     MindForger thinking notebook

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
#ifndef M8R_NOTE_EDIT_HIGHLIGHTER_H
#define M8R_NOTE_EDIT_HIGHLIGHTER_H

#include <QtWidgets>

#include "look_n_feel.h"
#include "spelling/dictionary_ref.h"
#include "spelling/dictionary_manager.h"

namespace m8r {

class NoteEditHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

private:
    enum Type {
        Bold,
        Bolder,
        Italic,
        Italicer,
        Strikethrough,
        Link,
        Autolink,
        Codeblock,
        Mathblock,
        UnorderedList,
        TaskDoneItem,
        TaskWipItem,
        TaskTodoItem,
        OrderedList,

        HtmlTag,
        HtmlAttribute,
        HtmlEntity,
        HtmlComment
    };

    enum State {
        Normal=1<<0,
        InComment=1<<1,
        InCode=1<<2
    };

    bool enabled;

    LookAndFeels& lookAndFeels;

    // spell check
    DictionaryRef spellCheckDictionary;
    bool isTypingPaused;
    QPlainTextEdit* noteEditorView;
    QTextDocument* noteEditorDocument;

    // Markdown formats
    QTextCharFormat boldFormat;
    QTextCharFormat bolderFormat;
    QTextCharFormat italicFormat;
    QTextCharFormat italicerFormat;
    QTextCharFormat strikethroughFormat;
    QTextCharFormat linkFormat;
    QTextCharFormat listFormat;
    QTextCharFormat taskDoneFormat;
    QTextCharFormat taskWipFormat;
    QTextCharFormat taskTodoFormat;
    QTextCharFormat codeBlockFormat;
    QTextCharFormat mathBlockFormat;

    // HTML formats
    QTextCharFormat htmlTagFormat;
    QTextCharFormat htmlAttrNameFormat;
    QTextCharFormat htmlAttValueFormat;
    QTextCharFormat htmlEntityFormat;
    QTextCharFormat htmlCommentFormat;

    // well defined order of regexps MATTERS - hashes/maps CANNOT be used
    std::vector<std::pair<Type,QRegExp*>*> typeAndRegex;

public:
    explicit NoteEditHighlighter(QPlainTextEdit* noteEditorView);
    NoteEditHighlighter(const NoteEditHighlighter&) = delete;
    NoteEditHighlighter(const NoteEditHighlighter&&) = delete;
    NoteEditHighlighter &operator=(const NoteEditHighlighter&) = delete;
    NoteEditHighlighter &operator=(const NoteEditHighlighter&&) = delete;
    ~NoteEditHighlighter();

    void setEnabled(bool enable) { enabled = enable; }
    bool isEnabled() const { return enabled; }

protected:
    // implementation of the abstract method that performs highlighting
    virtual void highlightBlock(const QString &text) override;

private:
    void addRegex(Type type, const QString& pattern, bool minimal=true);
    void highlightPatterns(const QString& text);
    bool highlightMultilineMdCode(const QString& text);
    void highlightMultilineHtmlComments(const QString& text);

    void spellCheck(const QString& text);
};

}

#endif // M8R_NOTE_EDIT_HIGHLIGHTER_H
