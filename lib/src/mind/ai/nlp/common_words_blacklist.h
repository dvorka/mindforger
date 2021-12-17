/*
 common_words_blacklist.h     MindForger thinking notebook

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
#ifndef M8R_COMMON_WORDS_BLACKLIST_H
#define M8R_COMMON_WORDS_BLACKLIST_H

#include "../../../gear/trie.h"

namespace m8r {

class CommonWordsBlacklist
{
private:
    Trie wordBlacklist;

public:
    explicit CommonWordsBlacklist();
    CommonWordsBlacklist(const CommonWordsBlacklist&) = delete;
    CommonWordsBlacklist(const CommonWordsBlacklist&&) = delete;
    CommonWordsBlacklist &operator=(const CommonWordsBlacklist&) = delete;
    CommonWordsBlacklist &operator=(const CommonWordsBlacklist&&) = delete;
    ~CommonWordsBlacklist();

    bool findWord(std::string s) const {
        return wordBlacklist.findWord(s);
    }
    void addWord(std::string word) {
        wordBlacklist.addWord(word);
    }
};

} // m8r namespace

#endif // M8R_COMMON_WORDS_BLACKLIST_H
