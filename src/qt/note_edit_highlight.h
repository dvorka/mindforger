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

namespace m8r {

class NoteEditHighlight : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit NoteEditHighlight(QTextDocument *parent=0);

protected:
    void highlightBlock(const QString &text);

private:
    enum State {
	    Normal=0x01, 
	    InComment=0x02
    };
    enum Type {
	    Tag, 
	    Attribute, 
	    Entity, 
	    Comment
    };

    void addRegex(Type type, const QString& pattern, bool minimal=true);

    void highlightPatterns(const QString& text);
    void highlightComments(const QString& text);

    QTextCharFormat tagFormat;
    QTextCharFormat attributeNameFormat;
    QTextCharFormat attributeValueFormat;
    QTextCharFormat entityFormat;
    QTextCharFormat commentFormat;
    QMultiHash<Type, QRegExp> regexForType;
};

}

#endif // M8R_NOTE_EDIT_HIGHLIGHT_H
