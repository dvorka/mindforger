/*
 cmark_aho_corasick_block_autolinking_preprocessor.h     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_CMARK_AHO_CORASICK_BLOCK_AUTOLINKING_PREPROCESSOR_H
#define M8R_CMARK_AHO_CORASICK_BLOCK_AUTOLINKING_PREPROCESSOR_H

#include "../autolinking_preprocessor.h"

namespace m8r {

/**
 * @brief cmark-gfm AST and Aho-Corasick autolinking pre-processor.
 *
 * Autolinking implementation which aims to be both precise (cmark-gfm AST)
 * and fast (Aho-Corasick).
 *
 * Ideal autolinking implementation has two goals:
 *
 *  - correctness
 *  - performance
 *
 * It aims to be FAST, therefore it's based on Aho-Corasick search structure
 * when autolinking lines.
 *
 * It aims to AVOID BREAKING rendered HTML by partial parsing of MD
 * constructions and autolinking only safe segments of line's AST.
 *
 * Algorithm description:
 *
 *  - O or N description is autolinked
 *  - O or N or T links are injected
 *
 * Preparation:
 *
 *  1. Aho-Corasick search structure is (re)build:
 *    - Aho-Corasick structure is Trie w/ internal links
 *    - the structure can be CACHED and rebuilt only when name or description
 *      of O/N is changed OR tag is added/removed
 *
 * Method:
 *
 *  1. WHOLE description is processed.
 *    - Previous line by line implementation was not able to correctly parse the
 *      document w/o context e.g. deep bullet lists.
 *  2. Description is parsed to AST by cmark-gfm.
 *  3. AST is traversed and O/N/T links injected.
 *    a) TEXT nodes are processed only
 *       - If TEXT node parent is NOT PARAGRAPH, then it's skipped
 *          - This preservers inlined links/bold/italics/...
 *          - This preserves code blocks, ...
 *            ... but NOT math blocks (must be tracked)
 *  4. Text nodes in AST are searched for O/N/T names presense using Aho-Corasick
 *     search structure:
 *    a) Aho-Corasics structure searches for all O/N/T names at once in single pass
 *    b) Aho-Corasics structure has LINEAR COMPLEXITY
 *    c) Aho-Corasics structure gives ALL MATCHES found in the text
 *    d) every MATCH to have:
 *      - begin offset
 *      - end offset
 *      - length
 *      - type O/N/T
 *    e) in case of overlapping/included MATCHES:
 *      - longest match always wins
 *  5. When there is O/N/T name clash (e.g. `ML` can be Outline, Note and Tag, actually
 *     there can be even more Notes with this name), then these matches are remembered
 *     and when user CLICKs the link in the description, then DIALOG with all options
 *     is opened (think Wikipedia cross-road page) and user can choose which link
 *     to use.
 *  6. Link nodes are injected instead original text nodes if they
 *     contain O/N/T.
 *    a) Original text nodes are NOT immediately removed, but collected to
 *       protect AST walker iterator/walker integrity.
 *  7. When the whole AST is walked and links injected:
 *    a) AST iterator is released.
 *    b) Collected original text nodes (zombies) are unlinked and deleted.
 */
class CmarkAhoCorasickBlockAutolinkingPreprocessor : public AutolinkingPreprocessor
{
public:
    // allowed text MD snippets words trailing chars (\\... added newly)
    static const std::string TRAILING_CHARS;

public:
    explicit CmarkAhoCorasickBlockAutolinkingPreprocessor(Mind& mind);
    CmarkAhoCorasickBlockAutolinkingPreprocessor(const CmarkAhoCorasickBlockAutolinkingPreprocessor&) = delete;
    CmarkAhoCorasickBlockAutolinkingPreprocessor(const CmarkAhoCorasickBlockAutolinkingPreprocessor&&) = delete;
    CmarkAhoCorasickBlockAutolinkingPreprocessor &operator=(const CmarkAhoCorasickBlockAutolinkingPreprocessor&) = delete;
    CmarkAhoCorasickBlockAutolinkingPreprocessor &operator=(const CmarkAhoCorasickBlockAutolinkingPreprocessor&&) = delete;
    ~CmarkAhoCorasickBlockAutolinkingPreprocessor();

    /**
     * @brief Autolink Markdown.
     */
    virtual void process(const std::vector<std::string*>& md, std::string& amd) override;
};

}
#endif // M8R_CMARK_AHO_CORASICK_BLOCK_AUTOLINKING_PREPROCESSOR_H
