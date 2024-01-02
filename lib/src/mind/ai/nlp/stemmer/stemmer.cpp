/*
 stemmer.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "stemmer.h"

namespace m8r {

using namespace std;

Stemmer::Stemmer()
{
    language = ENGLISH;
}

Stemmer::~Stemmer()
{
}

string Stemmer::stem(string word)
{
    // IMPROVE: despite stemmer works in wstring mode, MindForger runs just in string mode - wstring to come later when entire application is switched
    std::wstringstream swide;
    swide << word.c_str();
    wstring wide = swide.str();

    // IMPROVE switch by language (configuration)
    StemEnglish(wide);

    return converter.to_bytes(wide);
}

} // m8r namespace
