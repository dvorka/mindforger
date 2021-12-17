/*
 naive_autolinking_preprocessor.h     MindForger thinking notebook

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
#ifndef M8R_NAIVE_AUTOLINKING_PREPROCESSOR_H
#define M8R_NAIVE_AUTOLINKING_PREPROCESSOR_H

#ifndef MF_MD_2_HTML_CMARK

#include <regex>
#include <string>

#include "../autolinking_preprocessor.h"

namespace m8r {

/**
 * @brief Simplistic and slow autolinking implementation.
 */
class NaiveAutolinkingPreprocessor : public AutolinkingPreprocessor
{
public:
    static const std::string PATTERN_LINK;
    static const std::string PATTERN_CODE;
    static const std::string PATTERN_MATH;
    static const std::string PATTERN_HTTP;

private:
    std::regex linkRegex;
    std::regex codeRegex;
    std::regex mathRegex;
    std::regex httpRegex;

    std::vector<Thing*> things;

public:
    explicit NaiveAutolinkingPreprocessor(Mind& mind);
    NaiveAutolinkingPreprocessor(const NaiveAutolinkingPreprocessor&) = delete;
    NaiveAutolinkingPreprocessor(const NaiveAutolinkingPreprocessor&&) = delete;
    NaiveAutolinkingPreprocessor &operator=(const NaiveAutolinkingPreprocessor&) = delete;
    NaiveAutolinkingPreprocessor &operator=(const NaiveAutolinkingPreprocessor&&) = delete;
    virtual ~NaiveAutolinkingPreprocessor();

    virtual void process(const std::vector<std::string*>& md, std::string& amd) override;
    void clear();

private:
    bool containsLinkCodeMath(const std::string* line);
    void updateThingsIndex();
    std::vector<Thing*> getThings() { return things; }
};

}
#endif // MF_MD_2_HTML_CMARK
#endif // M8R_NAIVE_AUTOLINKING_PREPROCESSOR_H
