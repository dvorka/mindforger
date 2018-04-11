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

MarkdownTokenizer::MarkdownTokenizer(Lexicon& lexicon, CommonWordsBlacklist& blacklist)
    : lexicon(lexicon), blacklist(blacklist), stemmer{}
{
}

MarkdownTokenizer::~MarkdownTokenizer()
{
}

void MarkdownTokenizer::tokenize(CharProvider& md, WordFrequencyList& wfl, bool useBlacklist, bool lowercase, bool stem)
{
    // tokenize relationships
    bool parseRels=false;
    vector<pair<string,string>> links{};
    bool inRelLabel=false;
    bool inRelLink=false;

    string w{}, link{};
    while(md.hasNext()) {
        const char c = md.next();

        // parse relationships/image links
        if(parseRels) {
            if(inRelLabel||inRelLink) {
                link += (md.get());
            } else {
                if(link.size()) link.clear();
            }

            switch(c) {
            case '\n':
            case '\r':
                inRelLabel=inRelLink=false;
                break;
            case '[':
                inRelLabel=true;
                break;
            case ']':
                if(inRelLabel && md.hasNext() && md.getLookahead()=='(') {
                    inRelLabel=false;
                    inRelLink=true;
                } else {
                    inRelLabel=false;
                }
                break;
            case ')':
                if(inRelLink) {
                    // relationship successfully parsed
                    size_t pos = link.find("](");
                    links.push_back(std::make_pair(link.substr(0, pos),link.substr(pos+2,link.size()-pos-3)));
                    MF_DEBUG("  Parsed link: " << links[links.size()-1].first << " / " << links[links.size()-1].second << endl);
                    inRelLink = false;
                    link.clear();
                }
                break;
            default:
                ;
            }
        }

        // tokenize text
        switch(c) {
        case '-':
            // check lookahead to accept words like: self-awareness
            if(md.hasNext() && md.getLookahead()!='-') {
                w += md.get();
                break;
            }
        case '\n':
        case '\r':
        case ' ':
        case '\t':
        case '!':
        case '?':
        case '.':
        case ',':
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
            handleWord(wfl, w, stem, useBlacklist);
            break;
        default:
            if(md.get() < 0) {
                // skip HIGH Unicode chars
                handleWord(wfl, w, stem, useBlacklist);
            } else {
                if(lowercase) {
                    w += tolower(md.get());
                } else {
                    w += md.get();
                }
            }

            break;
        }
    }

    lexicon.recalculateWeights();
}

void MarkdownTokenizer::handleWord(WordFrequencyList& wfl, string &w, bool stem, bool useBlacklist)
{
    if(w.size()>1) {
        // stem
        if(stem) {
            w = stemmer.stem(w);
        }

        // remove common words
        if(!useBlacklist || !blacklist.findWord(w)) {
            // increment token frequency
            Lexicon::WordEmbedding* we = lexicon.add(w);
            ++wfl[&(we->word)];
        }
    }
    w.clear();
}

} // m8r namespace
