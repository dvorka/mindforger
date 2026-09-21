/*
 string-utils.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "string_utils.h"

#include <cassert>
#include <regex>
#include <sstream>

using namespace std;

namespace m8r {

bool stringStartsWith(const char* s, const char* prefix)
{
    assert(s);
    assert(prefix);

    size_t prefixLng = strlen(prefix), sLng = strlen(s);
    return sLng < prefixLng ? false : strncmp(prefix, s, prefixLng) == 0;
}

bool stringStartsWith(const string& s, const string& prefix)
{
    return stringStartsWith(s.c_str(), prefix.c_str());
}

bool stringStartsWith(const string& s, const char* prefix)
{
    return stringStartsWith(s.c_str(), prefix);
}

bool stringEndsWith(const string& s, const string& suffix)
{
    if(suffix.size() > s.size()) {
        return false;
    }
    return equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

bool stringEndsWith(const char* s, const char* suffix)
{
    if (!s || !suffix) {
        return false;
    }
    size_t sLng = strlen(s);
    size_t suffixLng = strlen(suffix);
    if (suffixLng >  sLng) {
        return false;
    }
    return strncmp(s + sLng - suffixLng, suffix, suffixLng) == 0;
}

bool stringEndsWith(const string& s, const char* suffix)
{
    if (!s.c_str() || !suffix) {
        return false;
    }
    size_t sLng = strlen(s.c_str());
    size_t suffixLng = strlen(suffix);
    if (suffixLng >  sLng) {
        return false;
    }
    return strncmp(s.c_str() + sLng - suffixLng, suffix, suffixLng) == 0;
}

/**
 * @brief Split spring using delimiter and return array of tokens.
 *
 * This function is outline metadata parser optimized - base size 15
 * and increment 5.
 */
char** stringSplit(const char* s, const char delimiter) {
    return stringSplit(s, delimiter, 15, 5);
}

/**
 * @brief Split spring using delimiter and return array of tokens.
 * @return Array of pointers to split items.
 */
char** stringSplit(
        const char* s,
        const char delimiter,
        u_int16_t resultBaseSize,
        u_int16_t resultIncSize)
{
    // IMPROVE remove if not needed
    UNUSED_ARG(resultIncSize);

    char **result;
    result = new char*[static_cast<unsigned int>(resultBaseSize)];
    const char delim[2] = { delimiter, 0};

    if(result) {
        size_t i  = 0;
        char* offset{};
#ifdef _WIN32
        char* copy = _strdup(s);
        char* token = strtok_s(copy, delim, &offset);
#else
        char* copy = strdup(s);
        char* token = strtok_r(copy, delim, &offset);
#endif
        while(token) {
            // strdup() on new operator
            size_t len = strlen(token);
            if(len) {
                result[i] = new char[len+1];
                for(size_t c=0; c<=len; c++) result[i][c]=token[c];
            } else {
                result[i] = nullptr;
            }
            i++;
#ifdef _WIN32
        token = strtok_s(nullptr, delim, &offset);
#else
        token = strtok_r(nullptr, delim, &offset);
#endif
            // TODO implement auto increase result
            assert(i <= static_cast<size_t>(resultBaseSize));
        }
        result[i] = nullptr;
        free(copy);
    }

    return result;
}

vector<string> stringSplit(const string s, const string regexDelimiter)
{
    regex regexz(regexDelimiter);
    vector<string> list(
        sregex_token_iterator(
            s.begin(), s.end(), regexz, -1
        ),
       sregex_token_iterator()
    );
    return list;
}

/**
 * @brief Normalize a string to NCName.
 *
 * @param name      the NCName
 * @param quoteChar the quote character
 * @return the normalized string
 */
string normalizeToNcName(string name, char quoteChar) {
    string result = name;
    if(!result.empty()) {
        // a non-ASCII (UTF-8, >=0x80) leading byte is part of a Unicode letter/digit,
        // which the XML NCName spec permits as NameStartChar - only an ASCII char
        // that is not alpha numerical needs the safe '_' prefix
        unsigned char first = static_cast<unsigned char>(result[0]);
        if(first < 0x80 && !isalnum(result[0], locale())) {
            result.insert(0, 1, '_');
        }
        for(size_t i=0; i<result.size(); i++) {
            unsigned char c = static_cast<unsigned char>(result[i]);
            if(c < 0x80 && !isalnum(result[i],locale())) {
                result[i] = quoteChar;
            }
        }
    }
    return result;
}

void toString(const std::vector<std::string*>& ss, std::string& os)
{
    if(!ss.empty()) {
        for(std::string* s:ss) {
            os += *s;
            os += "\n";
        }
    }
}

void replaceAll(const std::string& old_s, const std::string& new_s, std::string& s)
{
    size_t from = 0;
    while((from = s.find(old_s, from)) != std::string::npos) {
             s.replace(from, old_s.length(), new_s);
             from += new_s.length();
    }
}

bool isMarkdownParagraphBoundaryLine(const string& line)
{
    // indented code block: 4+ leading spaces or a leading TAB - checked on the RAW,
    // untrimmed line as trimming would hide the indentation which makes it code
    if(stringStartsWith(line, "\t")) {
        return true;
    }
    size_t leadingSpaces{0};
    while(leadingSpaces<line.size() && line[leadingSpaces]==' ') {
        leadingSpaces++;
    }
    if(leadingSpaces>=4) {
        return true;
    }

    string trimmed{line};
    stringTrim(trimmed);

    if(trimmed.empty()) {
        return true;
    }

    static const regex heading{R"(^#{1,6}(\s|$))"};
    static const regex blockquote{R"(^>)"};
    static const regex codeFence{R"(^(```|~~~))"};
    static const regex bulletList{R"(^[-*]\s+\S)"};
    static const regex numberedList{R"(^\d+\.\s+\S)"};
    static const regex horizontalRule{R"(^(-{3,}|\*{3,}|_{3,})\s*$)"};

    if(regex_search(trimmed, heading)
       || regex_search(trimmed, blockquote)
       || regex_search(trimmed, codeFence)
       || regex_search(trimmed, bulletList)
       || regex_search(trimmed, numberedList)
       || regex_search(trimmed, horizontalRule)
    ) {
        return true;
    }

    // table row - heuristic: any pipe character on the line
    if(trimmed.find('|')!=string::npos) {
        return true;
    }

    return false;
}

static size_t utf8Length(const string& s)
{
    // count Unicode codepoints in a UTF-8 encoded string by skipping continuation
    // bytes (10xxxxxx) - counts UTF-8 bytes, so accented Latin, Cyrillic or non-ASCII

    size_t length{0};
    for(unsigned char c: s) {
        if((c&0xC0)!=0x80) {
            length++;
        }
    }
    return length;
}

static string asciiToLower(const string& s)
{
    // fold A-Z only - unlike std::tolower() this neither depends on the global
    // locale, nor it touches the bytes of non-ASCII UTF-8 characters

    string lower{s};
    for(char& c: lower) {
        if(c>='A' && c<='Z') {
            c += 'a'-'A';
        }
    }
    return lower;
}

vector<string> rewrapParagraphLines(const vector<string>& lines, unsigned width)
{
    // hard break (two trailing spaces or backslash) aware word ensuring
    // it will not be lost during rewrapping
    struct Word {
        string text;
        bool hardBreak;
        string hardBreakSuffix;
    };

    vector<Word> words{};
    for(const string& line: lines) {
        size_t contentEnd{line.size()};
        while(contentEnd>0 && line[contentEnd-1]==' ') {
            contentEnd--;
        }
        bool hardBreak{false};
        string hardBreakSuffix{};
        if(line.size()-contentEnd>=2) {
            hardBreak = true;
            hardBreakSuffix = "  ";
        } else if(contentEnd>0 && line[contentEnd-1]=='\\') {
            hardBreak = true;
        }

        istringstream iss{line};
        string word{};
        size_t wordsBefore{words.size()};
        while(iss >> word) {
            words.push_back(Word{word, false, {}});
        }
        if(hardBreak && words.size()>wordsBefore) {
            words.back().hardBreak = true;
            words.back().hardBreakSuffix = hardBreakSuffix;
        }
    }

    vector<string> result{};
    string currentLine{};
    size_t currentLineLength{0};
    for(const Word& word: words) {
        size_t wordLength = utf8Length(word.text);
        if(currentLine.empty()) {
            currentLine = word.text;
            currentLineLength = wordLength;
        } else if(currentLineLength+1+wordLength<=width) {
            currentLine += ' ';
            currentLine += word.text;
            currentLineLength += 1+wordLength;
        } else {
            result.push_back(currentLine);
            currentLine = word.text;
            currentLineLength = wordLength;
        }
        if(word.hardBreak) {
            currentLine += word.hardBreakSuffix;
            result.push_back(currentLine);
            currentLine.clear();
            currentLineLength = 0;
        }
    }
    if(!currentLine.empty()) {
        result.push_back(currentLine);
    }

    return result;
}

vector<string> sortLinesAlphabetically(const vector<string>& lines)
{
    vector<string> result{lines};

    sort(
        result.begin(),
        result.end(),
        [](const string& l, const string& r) {
            const string lowerL{asciiToLower(l)}, lowerR{asciiToLower(r)};
            // case sensitive tie break makes the ordering of the lines which
            // differ in the case only deterministic
            if(lowerL == lowerR) {
                return l < r;
            }
            return lowerL < lowerR;
        }
    );

    return result;
}

} /* namespace */
