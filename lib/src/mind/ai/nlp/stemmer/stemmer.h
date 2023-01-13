/*
 stemmer.h     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_STEMMER_H
#define M8R_STEMMER_H

#include <string>
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>

#include "stemming/english_stem.h"
#include "stemming/french_stem.h"
#include "stemming/german_stem.h"
#include "stemming/finnish_stem.h"
#include "stemming/swedish_stem.h"
#include "stemming/spanish_stem.h"
#include "stemming/dutch_stem.h"
#include "stemming/danish_stem.h"
#include "stemming/italian_stem.h"
#include "stemming/norwegian_stem.h"
#include "stemming/portuguese_stem.h"
#include "stemming/russian_stem.h"

namespace m8r {

class Stemmer
{
public:
    enum Language {
        ENGLISH
    };

private:
    Language language;

    stemming::english_stem<> StemEnglish;
    stemming::german_stem<> StemGerman;
    stemming::finnish_stem<> StemFinnish;
    stemming::swedish_stem<> StemSwedish;
    stemming::dutch_stem<> StemDutch;
    stemming::spanish_stem<> StemSpanish;
    stemming::italian_stem<> StemItalian;
    stemming::norwegian_stem<> StemNorwgian;
    stemming::danish_stem<> StemDanish;
    stemming::portuguese_stem<> StemPortuguese;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

public:
    explicit Stemmer();
    Stemmer(const Stemmer&) = delete;
    Stemmer(const Stemmer&&) = delete;
    Stemmer &operator=(const Stemmer&) = delete;
    Stemmer &operator=(const Stemmer&&) = delete;
    ~Stemmer();

    void setLanguage(Language lang) { this->language = lang; }

    std::string stem(std::string word);
};

}
#endif // M8R_STEMMER_H
