/*
 markdown_tokenizer.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "markdown_tokenizer.h"

namespace m8r {

using namespace std;

MarkdownTokenizer::MarkdownTokenizer()
{
}

MarkdownTokenizer::~MarkdownTokenizer()
{
}

void MarkdownTokenizer::tokenize(CharProvider& md, BagOfWords& bow, bool blacklist, bool stem)
{
    // IMPROVE incorporate options
    UNUSED_ARG(blacklist);
    UNUSED_ARG(stem);

    string w{};
    while(md.hasNext()) {
        switch(md.next()) {
        case '-':
            // check lookahead to accept words like: self-awareness
            if(md.hasNext() && md.getLookahead()!='-') {
                w += md.get();
                break;
            }
        case ' ':
        case '\n':
        case '\r':
        case '!':
        case '?':
        case '.':
        case ':':
        case ';':
        case '#':
        case '=':
        case '`':
        case '(':
        case ')':
        case '[':
        case ']':
        case '*':
        case '_':
        case '"':
        case '\'':
        case '~':
        case '@':
        case '$':
        case '%':
        case '^':
        case '&':
        case '+':
        case '{':
        case '}':
        case '|':
        case '\\':
        case '<':
        case '>':
        case '/':
            if(w.size()>1) {
                // stem
                if(stem) {
                    // TODO stem word
                }

                // blacklist
                // TODO remove blacklisted words like: but, else, and, ...

                // increment token frequency
                ++bow[w];
            }
            w.clear();
            break;
        default:
            w += md.get();
            break;
        }
    }

}

} // m8r namespace
