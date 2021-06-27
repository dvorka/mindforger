/*
 string_char_provider.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_STRING_CHAR_PROVIDER_H
#define M8R_STRING_CHAR_PROVIDER_H

#include <string>

#include "../../../debug.h"
#include "char_provider.h"

namespace m8r {

// IMPROVE consider template
class StringCharProvider : public CharProvider
{
private:
    const std::string& s;
    bool fst;
    size_t i;

public:
    explicit StringCharProvider(const std::string& s);
    StringCharProvider(const StringCharProvider&) = delete;
    StringCharProvider(const StringCharProvider&&) = delete;
    StringCharProvider &operator=(const StringCharProvider&) = delete;
    StringCharProvider &operator=(const StringCharProvider&&) = delete;
    ~StringCharProvider();

    virtual bool hasNext() {
        if(s.empty()) {
            return false;
        } else {
            if(fst) {
                return true;
            }  else {
                return i<s.size()-1;
            }
        }
    }
    virtual const char& next() {
        if(fst) {
            fst=false;
            return s[0];
        } else
            return s[++i];
    }
    virtual const char& get() { return s[i]; }
    virtual const char& getLookahead() { return s[i+1]; }
};

}
#endif // M8R_STRING_CHAR_PROVIDER_H
