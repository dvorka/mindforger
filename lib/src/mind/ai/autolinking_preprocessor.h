/*
 autolinking_preprocessor.h     MindForger thinking notebook

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
#ifndef M8R_AUTOLINKING_PREPROCESSOR_H
#define M8R_AUTOLINKING_PREPROCESSOR_H

#include <string>
#include <vector>
#include <set>

#include "../mind.h"
#include "../../representations/representation_interceptor.h"

namespace m8r {

class AutolinkingPreprocessor : public RepresentationInterceptor
{
    Mind& mind;

    std::vector<Note*> notes;
    // IMPROVE trie
    std::set<std::string> noteNames;

public:
    explicit AutolinkingPreprocessor(Mind& mind);
    AutolinkingPreprocessor(const AutolinkingPreprocessor&) = delete;
    AutolinkingPreprocessor(const AutolinkingPreprocessor&&) = delete;
    AutolinkingPreprocessor &operator=(const AutolinkingPreprocessor&) = delete;
    AutolinkingPreprocessor &operator=(const AutolinkingPreprocessor&&) = delete;
    ~AutolinkingPreprocessor();

    virtual void process(const std::vector<std::string*>& md, std::vector<std::string*>& amd) override;

    /**
     * @brief Update N names/links indices.
     */
    void updateIndices();
};

}
#endif // M8R_AUTOLINKING_PREPROCESSOR_H
