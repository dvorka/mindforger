/*
 autolinking_preprocessor.h     MindForger thinking notebook

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
#ifndef M8R_AUTOLINKING_PREPROCESSOR_H
#define M8R_AUTOLINKING_PREPROCESSOR_H

#include <regex>

#include "../../mind/ontology/thing_class_rel_triple.h"
#include "../../representations/representation_interceptor.h"

namespace m8r {

/**
 * @brief Autolinking preprocecesssor abstract class.
 */
class AutolinkingPreprocessor : public RepresentationInterceptor
{
public:
    static const std::string PATTERN_LINK;
    static const std::string PATTERN_CODE;
    static const std::string PATTERN_MATH;
    static const std::string PATTERN_HTTP;

protected:
    std::regex linkRegex;
    std::regex codeRegex;
    std::regex mathRegex;
    std::regex httpRegex;

public:
    explicit AutolinkingPreprocessor();
    AutolinkingPreprocessor(const AutolinkingPreprocessor&) = delete;
    AutolinkingPreprocessor(const AutolinkingPreprocessor&&) = delete;
    AutolinkingPreprocessor &operator=(const AutolinkingPreprocessor&) = delete;
    AutolinkingPreprocessor &operator=(const AutolinkingPreprocessor&&) = delete;
    virtual ~AutolinkingPreprocessor();

    virtual void process(const std::vector<std::string*>& in, std::vector<std::string*>& out) = 0;

    bool containsLinkCodeMath(const std::string* line);

    static bool aliasSizeComparator(const Thing* t1, const Thing* t2);
};

}
#endif // M8R_AUTOLINKING_PREPROCESSOR_H
