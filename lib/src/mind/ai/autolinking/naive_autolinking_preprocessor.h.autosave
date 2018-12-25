/*
 naive_autolinking_preprocessor.h     MindForger thinking notebook

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
#ifndef M8R_NAIVE_AUTOLINKING_PREPROCESSOR_H
#define M8R_NAIVE_AUTOLINKING_PREPROCESSOR_H

#include <set>
#include <regex>

#include "../autolinking_preprocessor.h"

namespace m8r {

class NaiveAutolinkingPreprocessor : public AutolinkingPreprocessor
{
public:
    explicit NaiveAutolinkingPreprocessor(Mind& mind);
    NaiveAutolinkingPreprocessor(const AutolinkingPreprocessor&) = delete;
    NaiveAutolinkingPreprocessor(const AutolinkingPreprocessor&&) = delete;
    NaiveAutolinkingPreprocessor &operator=(const AutolinkingPreprocessor&) = delete;
    NaiveAutolinkingPreprocessor &operator=(const AutolinkingPreprocessor&&) = delete;
    virtual ~NaiveAutolinkingPreprocessor();

    virtual void process(const std::vector<std::string*>& md, std::vector<std::string*>& amd) override;

private:

    /**
     * @brief Update N names/links indices.
     */
    void updateIndices();

    void injectLink(
            std::string* line,
            const std::string& label,
            const std::string& link);

    bool findLinkOrInlineCode(const std::string* nl);
};

}
#endif // M8R_NAIVE_AUTOLINKING_PREPROCESSOR_H
