/*
 note_edit_highlight.h     MindForger thinking notebook

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
#ifndef M8R_NOTE_EDIT_HIGHLIGHT_H
#define M8R_NOTE_EDIT_HIGHLIGHT_H

#include <QtWidgets>

#include "look_n_feel.h"

namespace m8r {

class NoteEditHighlight : public QSyntaxHighlighter
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
        Codeblock,

        HtmlTag,
        HtmlAttribute,
        HtmlEntity,
        HtmlComment
    };

    enum State {
        Normal=0x01,
        InComment=0x02
    };

    LookAndFeels& lookAndFeels;

    // Markdown formats
    QTextCharFormat boldFormat;
    QTextCharFormat bolderFormat;
    QTextCharFormat italicFormat;
    QTextCharFormat italicerFormat;
    QTextCharFormat strikethroughFormat;
    QTextCharFormat linkFormat;
    QTextCharFormat codeblockFormat;

    // HTML formats
    QTextCharFormat htmlTagFormat;
    QTextCharFormat htmlAttrNameFormat;
    QTextCharFormat htmlAttValueFormat;
    QTextCharFormat htmlEntityFormat;
    QTextCharFormat htmlCommentFormat;

    QMultiHash<Type, QRegExp> regexForType;

public:
    explicit NoteEditHighlight(QTextDocument* parent);
    NoteEditHighlight(const NoteEditHighlight&) = delete;
    NoteEditHighlight(const NoteEditHighlight&&) = delete;
    NoteEditHighlight &operator=(const NoteEditHighlight&) = delete;
    NoteEditHighlight &operator=(const NoteEditHighlight&&) = delete;

protected:
    void highlightBlock(const QString &text);

private:
    void addRegex(Type type, const QString& pattern, bool minimal=true);
    void highlightPatterns(const QString& text);
    void highlightComments(const QString& text);
};

}

#endif // M8R_NOTE_EDIT_HIGHLIGHT_H
