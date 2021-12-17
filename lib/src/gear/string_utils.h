/*
 string-utils.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_STRING_UTILS_H_
#define M8R_STRING_UTILS_H_

#include <cstring>
#include <cctype>

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <locale>
#include <vector>

#include "../definitions.h"
#include "lang_utils.h"

namespace m8r {

/*
 *  Helper functions for handling C character strings and std::strings.
 */

bool stringStartsWith(const char* s, const char* prefix);
bool stringStartsWith(const std::string& s, const char* prefix);
bool stringStartsWith(const std::string& s, const std::string& prefix);
bool stringEndsWith(const char* s, const char* suffix);
bool stringEndsWith(const std::string& s, const char* suffix);
bool stringEndsWith(const std::string& s, const std::string& suffix);
char** stringSplit(const char* s, const char delimiter);
char** stringSplit(const char* s, const char delimiter, u_int16_t resultBaseSize, u_int16_t resultIncSize);

/**
 * @brief Normalizes a string to NCName.
 *
 * @param name      the NCName
 * @param quoteChar the quote character
 * @return the normalized string
 */
std::string normalizeToNcName(std::string name, char quoteChar);

/**
 * @brief Check wheter strings are identical while ignoring case.
 */
static inline bool stringistring(const std::string& a, const std::string& b)
{
    unsigned long long asize = a.size();
    if(b.size()==asize) {
        for(unsigned int i = 0; i < asize; ++i) {
            if(tolower(a[i]) != tolower(b[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

static inline void stringToLower(const std::string& s, std::string& lowerS)
{
    static const std::locale locale;
    for(std::string::size_type i=0; i<s.length(); ++i) {
        lowerS += std::tolower(s[i],locale);
    }
}

/**
 * @brief Trim leading and trailing whitespaces.
 *
 * New trimmed string is created and returned - caller is responsible for freeing it.
 */
static inline char *stringTrim(const char *s) {
    if(s) {
        while(isspace(static_cast<unsigned char>(*s))) {
            s++;
        }
        if(*s == 0) {
            char *result = new char[1];
            result[0] = 0;
            return result;
        }

        const char* end = s + strlen(s) - 1;
        while(end > s && isspace(static_cast<unsigned char>(*end))) {
            end--;
        }
        ++ end;

        size_t resultSize = static_cast<size_t>(end - s);
        char* result = new char[resultSize + 1];
        memcpy(result, s, resultSize);
        result[resultSize] = 0;

        return result;
    }
    return nullptr;
}

static inline std::string &stringLeftTrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(isspace))));
    return s;
}

static inline std::string &stringRightTrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

static inline std::string &stringTrim(std::string& s) {
    return stringLeftTrim(stringRightTrim(s));
}

static inline std::string to_string(int x)
{
  size_t length = static_cast<size_t>(snprintf(nullptr, 0, "%d", x));
  char* buf = new char[length + 1];
  snprintf(buf, length + 1, "%d", x);
  std::string str(buf);
  delete[] buf;
  return str;
}

static inline std::string to_stringl(time_t x)
{
  size_t length = static_cast<size_t>(snprintf(nullptr, 0, "%d", static_cast<int>(x)));
  char* buf = new char[length + 1];
  snprintf(buf, length + 1, "%ld", x);
  std::string str(buf);
  delete[] buf;
  return str;
}

void toString(const std::vector<std::string*>& ss, std::string& os);

static inline std::string stringIntFormat(std::string value, char thousandSep = ',')
{
    unsigned long long len = value.length();
    unsigned long long dlen = 3;

    while(len > dlen) {
        value.insert(len - dlen, 1, thousandSep);
        dlen += 4;
        len += 1;
    }

    return value;
}

void replaceAll(const std::string& old_s, const std::string& new_s, std::string& s);

} /* namespace*/

#endif /* M8R_STRING_UTILS_H_ */
