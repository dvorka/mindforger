/*
 note_edit_highlight.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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

using namespace std;

NoteEditHighlight::NoteEditHighlight(QTextDocument* parent)
    : QSyntaxHighlighter(parent),
      lookAndFeels(LookAndFeels::getInstance())
{
    /*
     * HTML inlined in MD - goes first so that formatting can be rewritten by MD
     */

    // IMPROVE consider making HTML highlighting optional (config)
    addRegex(HtmlTag, "<[!?]?\\w+(?:/>)?", false);
    addRegex(HtmlTag, "(?:</\\w+)?[?]?>");
    addRegex(HtmlEntity, "&(:?#\\d+|\\w+);");
    addRegex(HtmlComment, "<!--.*-->");
    addRegex(HtmlAttribute, "(\\w+(?::\\w+)?)=(\"[^\"]+\"|'[^\']+')");

    htmlTagFormat.setForeground(lookAndFeels.getEditorHtmlTag());
    htmlAttrNameFormat.setForeground(lookAndFeels.getEditorHtmlAttrName());
    htmlAttValueFormat.setForeground(lookAndFeels.getEditorHtmlAttrValue());
    htmlEntityFormat.setForeground(lookAndFeels.getEditorHtmlEntity());
    htmlCommentFormat.setForeground(lookAndFeels.getEditorHtmlComment());
    htmlCommentFormat.setFontItalic(true);

    /*
     * Markdown (check QRegExp or Perl regexps)
     */

    // regexps
    addRegex(Bold, "\\*\\S[\\S\\s]+\\*",true);
    addRegex(Bolder, "\\*\\*[\\S\\s]+\\*\\*");
    addRegex(Italic, "_[\\S\\s]+_");
    addRegex(Italicer, "__[\\S\\s]+\\__");
    addRegex(Strikethrough, "~~[\\S\\s]+\\~~");
    addRegex(Link, "\\[(:?[\\S\\s]+)\\]\\([\\S\\s]+\\)");
    addRegex(Autolink, "https?://\\S+",false);
    addRegex(Codeblock, "`[\\S\\s]+`");
    addRegex(Mathblock, "\\$[\\S\\s]+\\$");
    addRegex(UnorderedList, "^(:?    )*[\\*\\+\\-] ");
    addRegex(OrderedList, "^(:?    )*\\d\\d?\\. ");
    addRegex(TaskDoneItem, "^(:?    )*[\\*\\+\\-] \\[x\\]");
    addRegex(TaskWipItem, "^(:?    )*[\\*\\+\\-] \\[ \\]");

    // formats
    boldFormat.setForeground(lookAndFeels.getEditorBold());
    bolderFormat.setForeground(lookAndFeels.getEditorBolder());
    italicFormat.setForeground(lookAndFeels.getEditorItalic());
    italicFormat.setFontItalic(true);
    italicerFormat.setForeground(lookAndFeels.getEditorItalicer());
    italicerFormat.setFontItalic(true);
    italicerFormat.setFontWeight(QFont::Bold);
    strikethroughFormat.setForeground(lookAndFeels.getEditorStrikethrough());
    linkFormat.setForeground(lookAndFeels.getEditorLink());
    listFormat.setForeground(lookAndFeels.getEditorList());
    taskDoneFormat.setForeground(lookAndFeels.getEditorTaskDone());
    taskWipFormat.setForeground(lookAndFeels.getEditorTaskWip());
    codeBlockFormat.setForeground(lookAndFeels.getEditorCodeblock());
    mathBlockFormat.setForeground(lookAndFeels.getEditorCodeblock());

#if QT_VERSION > QT_VERSION_CHECK(5, 5, 0)
    bolderFormat.setFontWeight(QFont::ExtraBold);
    listFormat.setFontWeight(QFont::ExtraBold);
    taskDoneFormat.setFontWeight(QFont::ExtraBold);
    taskWipFormat.setFontWeight(QFont::ExtraBold);
#else
    bolderFormat.setFontWeight(QFont::Black);
    listFormat.setFontWeight(QFont::Black);
#endif
}

NoteEditHighlight::~NoteEditHighlight()
{
    for(auto& p:typeAndRegex) {
        delete p;
    }
    typeAndRegex.clear();
}


/**
 * @brief Add regexp for matching.
 * @param minimal   controls non-greed vs greedy matching
 *
 * Add Qt's Perl compatible regexp - see QRegExp or https://perlmaven.com/regex-cheat-sheet
 */
void NoteEditHighlight::addRegex(Type type, const QString &pattern, bool minimal)
{
    QRegExp* regex = new QRegExp{pattern};
    regex->setPatternSyntax(QRegExp::RegExp2);
    regex->setMinimal(minimal);

    std::pair<Type,QRegExp*>* p = new std::pair<Type,QRegExp*>(type,regex);
    typeAndRegex.push_back(p);
}

/**
 * @brief This method is called for EACH line to highlight it.
 *
 * Multi-line highlighting is solved by maintaining a state as the
 * whole document is being highlighted.
 */
void NoteEditHighlight::highlightBlock(const QString& text)
{
    if(enabled) {
        // clear format of the text
        setCurrentBlockState(Normal);

        // when in MD code section, then there is no need to highlight anything
        if(!highlightMultilineMdCode(text)) {
            // highlight patterns defined using regexps
            if(text.size()) highlightPatterns(text);
            // eventually overwrite certain formatting with *multiline(s)* like MD code or HTML comments
            highlightMultilineHtmlComments(text);
        }
    }
}

/*
 * This method get editor's text and it uses regexps to tokenize
 * it. Then it assigns a format to every detected token using
 * setFormat(offset,length) function.
 */
void NoteEditHighlight::highlightPatterns(const QString& text)
{
    // iterate all regexps - ORDER matters as latter regexps may OVERWRITE format of
    // earlier regexps, e.g. consider bold rewritten by multiline code or bold rewriting
    // bolder
    // IMPROVE improve O(n) which is BIG and depends on the number of regexps:
    //   O(n) = size(regexps) * lng(text)
    for(auto p:typeAndRegex) {
        Type type = p->first;
        QRegExp* regex = p->second;

        // find 1st match for regex in text
        int index = regex->indexIn(text);
        // loop until there are other matches
        while(index > -1) {
            int length = regex->matchedLength();

            switch(type) {
            case Bolder:
                setFormat(index, length, bolderFormat);
                break;
            case Bold:
                setFormat(index, length, boldFormat);
                break;
            case Italic:
                setFormat(index, length, italicFormat);
                break;
            case Italicer:
                setFormat(index, length, italicerFormat);
                break;
            case Strikethrough:
                setFormat(index, length, strikethroughFormat);
                break;
            case Codeblock:
                setFormat(index, length, codeBlockFormat);
                break;
            case Mathblock:
                setFormat(index, length, mathBlockFormat);
                break;
            case Link:
                setFormat(index, length, linkFormat);
                break;
            case Autolink:
                setFormat(index, length, linkFormat);
                break;
            case UnorderedList:
                setFormat(index, length, listFormat);
                break;
            case OrderedList:
                setFormat(index, length, listFormat);
                break;
            case TaskDoneItem:
                setFormat(index, length, taskDoneFormat);
                break;
            case TaskWipItem:
                setFormat(index, length, taskWipFormat);
                break;
            case HtmlTag:
                setFormat(index, length, htmlTagFormat);
                break;
            case HtmlAttribute:
                setFormat(
                    index,
                    regex->pos(2) - index - 1,
                    htmlAttrNameFormat);
                setFormat(
                    regex->pos(2) + 1,
                    regex->cap(2).length() - 2,
                    htmlAttValueFormat);
                break;
            case HtmlEntity:
                setFormat(index, length, htmlEntityFormat);
                break;
            case HtmlComment:
                // this is single line comment - multiline comments are matched by separate method
                setFormat(index, length, htmlCommentFormat);
                break;
            }

            // match again
            index = regex->indexIn(text, index+length);
        }
    }
}

/**
 * @brief Highlight MD multiline code and return true if the line has been formatted.
 */
bool NoteEditHighlight::highlightMultilineMdCode(const QString &text)
{
    static const QString TOKEN("```");

    if(previousBlockState()!=-1 && (previousBlockState()&InCode)==InCode) {
        // already inside block
        if(!text.compare(TOKEN)) {
            // finish block ~ don't send anything
            setFormat(0, TOKEN.length(), codeBlockFormat);
            return true;
        } else {
            // continue block
            setCurrentBlockState(currentBlockState()|InCode);
            setFormat(0, text.size(), codeBlockFormat);
            return true;
        }
    } else {
        // outside block
        if(text.startsWith(TOKEN)) {
            // enter block ~ don't send anything
            setCurrentBlockState(currentBlockState()|InCode);
            setFormat(0, text.size(), codeBlockFormat);
            return true;
        } else {
            setCurrentBlockState(Normal);
            return false;
        }
    }
}

void NoteEditHighlight::highlightMultilineHtmlComments(const QString &text)
{
    static const QString BEGIN_TOKEN("<!--");
    static const QString END_TOKEN("-->");

    if(previousBlockState() > -1 && (previousBlockState() & InComment) == InComment) {
        int end = text.indexOf(END_TOKEN);
        if (end == -1) {
            setFormat(0, text.length(), htmlCommentFormat);
            setCurrentBlockState(currentBlockState() | InComment);
            return;
        }
        else {
            setFormat(0, end + END_TOKEN.length(), htmlCommentFormat);
        }
    }

    int start = text.lastIndexOf(BEGIN_TOKEN);
    if(start != -1) {
        int end = text.lastIndexOf(END_TOKEN);
        if(end < start) {
            setFormat(start, text.length(), htmlCommentFormat);
            setCurrentBlockState(currentBlockState() | InComment);
        }
    }
}

} // m8r namespace
