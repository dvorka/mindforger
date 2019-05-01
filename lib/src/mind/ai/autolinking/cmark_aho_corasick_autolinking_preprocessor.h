/*
 cmark_aho_corasick_autolinking_preprocessor.h     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_CMARK_AHO_CORASICK_AUTOLINKING_PREPROCESSOR_H
#define M8R_CMARK_AHO_CORASICK_AUTOLINKING_PREPROCESSOR_H

namespace m8r {

/**
 * @brief cmark-gfm AST and Aho-Corasick autolinking pre-processor.
 *
 * Autolinking implementation which aims to be both precise (cmark-gfm AST)
 * and fast (Aho-Corasick).
 */
class CmarkAhoCorasickAutolinkingPreprocessor
{
public:
    explicit CmarkAhoCorasickAutolinkingPreprocessor();
    CmarkAhoCorasickAutolinkingPreprocessor(const CmarkAhoCorasickAutolinkingPreprocessor&) = delete;
    CmarkAhoCorasickAutolinkingPreprocessor(const CmarkAhoCorasickAutolinkingPreprocessor&&) = delete;
    CmarkAhoCorasickAutolinkingPreprocessor &operator=(const CmarkAhoCorasickAutolinkingPreprocessor&) = delete;
    CmarkAhoCorasickAutolinkingPreprocessor &operator=(const CmarkAhoCorasickAutolinkingPreprocessor&&) = delete;
    ~CmarkAhoCorasickAutolinkingPreprocessor();
};

}
#endif // M8R_CMARK_AHO_CORASICK_AUTOLINKING_PREPROCESSOR_H
