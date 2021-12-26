/*
 markdown_tokenizer.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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

MarkdownTokenizer::~MarkdownTokenizer() = default;

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
            // avoid compiler fall-through warning
            MF_FALL_THROUGH;
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

bool MarkdownTokenizer::isNonAlpha(char c)
{
    switch(c) {
    case '-':
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
        return true;
    default:
        if(c<0) {
            // high unicode
            return true;
        } else {
            return false;
        }
    }
}

string MarkdownTokenizer::stripFrontBackNonAlpha(string s)
{
    MF_DEBUG("Strip F-B before: " << s << endl);
    if(s.size()) {
        string w{};
        // front (points to 1st alpha)
        size_t f = 0;
        while(f<s.size() && isNonAlpha(s.at(f))) {
            f++;
        }
        // back (points to last alpha)
        size_t b = s.size()-1;
        while(b>=1 && isNonAlpha(s.at(b))) {
            b--;
        }

        if(f == b) {
            w += s.at(f);
            MF_DEBUG("Strip F-B after : " << w << endl);
            return w;
        } else if(f > b) {
            return w;
        } else {
            for(size_t i=f; i<=b; i++) {
               w += s.at(i);
            }
            MF_DEBUG("Strip F-B after : " << w << endl);
            return w;
        }
    } else {
        return s;
    }
}

string MarkdownTokenizer::stripNonAlpha(CharProvider& md)
{
    string w{};

    while(md.hasNext()) {
        const char c = md.next();

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
            return w;
        default:
            if(md.get() < 0) {
                return w;
            } else {
                w += md.get();
            }
            break;
        }
    }

    return w;
}

} // m8r namespace
