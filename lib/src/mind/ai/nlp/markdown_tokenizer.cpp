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

MarkdownTokenizer::MarkdownTokenizer(Lexicon& lexicon, Trie& blacklist)
    : lexicon(lexicon), blacklist(blacklist)
{
}

MarkdownTokenizer::~MarkdownTokenizer()
{
}

void MarkdownTokenizer::tokenize(CharProvider& md, WordFrequencyList& wfl, bool useBlacklist, bool lowercase, bool stem)
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

                // remove common words
                if(!useBlacklist || !blacklist.searchWord(w)) {
                    // increment token frequency
                    Lexicon::WordEmbedding* we = lexicon.add(w);
                    ++wfl[&(we->word)];
                }
            }
            w.clear();
            break;
        default:
            if(lowercase) {
                w += tolower(md.get());
            } else {
                w += md.get();
            }
            break;
        }
    }

    lexicon.recalculateWeights();
}

} // m8r namespace
