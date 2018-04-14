/*
 markdown_tokenizer.h     MindForger thinking notebook

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
#ifndef M8R_MARKDOWN_TOKENIZER_H
#define M8R_MARKDOWN_TOKENIZER_H

#include <set>

#include "../../../debug.h"
#include "../../../gear/lang_utils.h"
#include "../../../mind/ai/nlp/common_words_blacklist.h"
#include "char_provider.h"
#include "lexicon.h"
#include "word_frequency_list.h"
#include "stemmer/stemmer.h"

namespace m8r {

/**
 * @brief Tokenize Markdown text to words.
 *
 * On tokenization:
 *
 *   - hardcoded delimiters
 *   - filters out words w/ length <1
 *   - stems words (optional)
 *   - computes token frequency via Lexicon
 *
 * See also:
 * https://www.ibm.com/developerworks/community/blogs/nlp/entry/tokenization?lang=en
 */
class MarkdownTokenizer
{
    Lexicon& lexicon;

    /**
     * @brief Word blacklist (e.g. and, but, have, do, ...)
     */
    CommonWordsBlacklist& blacklist;

    Stemmer stemmer;

public:
    explicit MarkdownTokenizer(Lexicon& lexicon, CommonWordsBlacklist& blacklist);
    MarkdownTokenizer(const MarkdownTokenizer&) = delete;
    MarkdownTokenizer(const MarkdownTokenizer&&) = delete;
    MarkdownTokenizer &operator=(const MarkdownTokenizer&) = delete;
    MarkdownTokenizer &operator=(const MarkdownTokenizer&&) = delete;
    ~MarkdownTokenizer();

    /**
     * @brief Tokenize a stream of characters.
     */
    void tokenize(CharProvider& md, WordFrequencyList& wfl, bool useBlacklist=true, bool lowercase=true, bool stem=true);

    /**
     * @brief Remove non-alpha numeric characters from the 1st word and return it.
     */
    static std::string stripNonAlpha(CharProvider& md);

    /**
     * @brief Strip non-alpha numeric characters from front and back of the string.
     */
    static std::string stripFrontBackNonAlpha(std::string s);

    /**
     * @brief Is non-alpha numberic character?
     */
    static bool isNonAlpha(char c);

private:
    inline void handleWord(WordFrequencyList& wfl, std::string &w, bool stem, bool useBlacklist);
};

}
#endif // M8R_MARKDOWN_TOKENIZER_H
