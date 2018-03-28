/*
 lexicon.h     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_LEXICON_H
#define M8R_LEXICON_H

#include <map>
#include <vector>
#include <string>

#include "../../../gear/lang_utils.h"

namespace m8r {

/**
 * @brief Lexicon of all words w/ global frequencies.
 */
// IMPROVE Stanford GloVe lexicon w/ word attributes & semantic domains (configure > check existince > use OR skip)
class Lexicon
{
private:
    std::map<std::string,int> m; // duplicity detection on merging
    std::vector<std::pair<std::string*,int>> words;

public:
    explicit Lexicon();
    Lexicon(const Lexicon&) = delete;
    Lexicon(const Lexicon&&) = delete;
    Lexicon &operator=(const Lexicon&) = delete;
    Lexicon &operator=(const Lexicon&&) = delete;
    ~Lexicon();

    void merge(std::map<std::string,int>) {} // TODO
    void finalize() {} // TODO delete map when nothing more will be added, blocks new additions
    std::string& operator [](int i) { static std::string s; UNUSED_ARG(i); return s; } // TODO get word for index
};

}
#endif // M8R_LEXICON_H
