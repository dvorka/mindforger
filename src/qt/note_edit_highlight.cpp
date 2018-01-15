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
#include "note_edit_highlight.h"

namespace m8r {

NoteEditHighlight::NoteEditHighlight(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    /*
     * Markdown (check QRegExp or Perl regexps)
     */

    // emphasis
    addRegex(Bold, "\\*(:?\\w[\\w\\s]+)\\*"); // shorter match must go first: * first, ** next
    addRegex(Bolder, "\\*\\*(:?\\w[\\w\\s]+)\\*\\*");
    addRegex(Italicer, "__(:?[\\w\\s]+)__");
    addRegex(Italic, "_(:?[\\w\\s]+)_");
    addRegex(Strikethrough, "~~(:?[\\w\\s]+)~~");
    addRegex(Codeblock, "`(:?[\\w\\s]+)`");

    // TODO extra method - HTML comment like
    //addRegex(CodeblockMultiline, "\\*(:?#\\d+|\\w+)\\*");

    // TODO colors from configuration
    boldFormat.setForeground(Qt::red);
    bolderFormat.setForeground(Qt::red);
    bolderFormat.setFontWeight(QFont::ExtraBold);
    italicFormat.setForeground(Qt::darkGreen);
    italicFormat.setFontItalic(true);
    italicerFormat.setForeground(Qt::darkGreen);
    italicerFormat.setFontItalic(true);
    italicerFormat.setFontWeight(QFont::Bold);
    strikethroughFormat.setForeground(Qt::blue);
    codeblockFormat.setForeground(Qt::blue);

    /*
     * HTML inlined in MD
     */

    addRegex(Tag, "<[!?]?\\w+(?:/>)?", false);
    addRegex(Tag, "(?:</\\w+)?[?]?>");
    addRegex(Entity, "&(:?#\\d+|\\w+);");
    addRegex(Comment, "<!--.*-->");
    addRegex(Attribute, "(\\w+(?::\\w+)?)=(\"[^\"]+\"|'[^\']+')");

    // TODO colors from configuration
    tagFormat.setForeground(Qt::darkBlue);
    attributeNameFormat.setForeground(Qt::blue);
    attributeValueFormat.setForeground(Qt::darkYellow);
    entityFormat.setForeground(Qt::darkRed);
    commentFormat.setForeground(Qt::gray);
    commentFormat.setFontItalic(true);
}

/*
 * Add Qt's Perl compatible regexp - see QRegExp or https://perlmaven.com/regex-cheat-sheet
 */
void NoteEditHighlight::addRegex(Type type, const QString &pattern, bool minimal)
{
    QRegExp regex(pattern);
    regex.setPatternSyntax(QRegExp::RegExp2);
    regex.setMinimal(minimal);
    regexForType.insert(type, regex);
}

void NoteEditHighlight::highlightBlock(const QString& text)
{
    setCurrentBlockState(Normal);

    highlightPatterns(text);
    highlightComments(text);
}

void NoteEditHighlight::highlightPatterns(const QString& text)
{
    QHashIterator<Type, QRegExp> i(regexForType);

    while(i.hasNext()) {
        i.next();
        Type type = i.key();
        const QRegExp &regex = i.value();

        int index = regex.indexIn(text);
        while(index > -1) {
            int length = regex.matchedLength();

            if(type == Bolder)
                setFormat(index, length, bolderFormat);
            else if(type == Bold)
                setFormat(index, length, boldFormat);
            else if(type == Italic)
                setFormat(index, length, italicFormat);
            else if(type == Italicer)
                setFormat(index, length, italicerFormat);
            else if(type == Strikethrough)
                setFormat(index, length, strikethroughFormat);
            else if(type == Codeblock)
                setFormat(index, length, codeblockFormat);
            else if(type == Tag)
                setFormat(index, length, tagFormat);
            else if(type == Attribute) {
                setFormat(index, regex.pos(2) - index - 1,
                          attributeNameFormat);
                setFormat(regex.pos(2) + 1, regex.cap(2).length() - 2,
                          attributeValueFormat);
            }
            else if(type == Entity)
                setFormat(index, length, entityFormat);
            else if(type == Comment)
                setFormat(index, length, commentFormat);
            index = regex.indexIn(text, index + length);
        }
    }
}

void NoteEditHighlight::highlightComments(const QString &text)
{
    const QString StartOfComment("<!--");
    const QString EndOfComment("-->");

    if(previousBlockState() > -1 && (previousBlockState() & InComment) == InComment) {
        int end = text.indexOf(EndOfComment);
        if (end == -1) {
            setFormat(0, text.length(), commentFormat);
            setCurrentBlockState(currentBlockState() | InComment);
            return;
        }
        else
            setFormat(0, end + EndOfComment.length(), commentFormat);
    }

    int start = text.lastIndexOf(StartOfComment);
    if(start != -1) {
        int end = text.lastIndexOf(EndOfComment);
        if(end < start) {
            setFormat(start, text.length(), commentFormat);
            setCurrentBlockState(currentBlockState() | InComment);
        }
    }
}

}
