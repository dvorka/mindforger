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

#include "../../representations/representation_interceptor.h"
#include "../../mind/mind.h"
#include "../../gear/trie.h"
#include "../../debug.h"

namespace m8r {

/**
 * @brief Autolinking pre-processor abstract class.
 *
 * Autolinking pre-processor injects Os and Ns text links to Markdown
 * text based on their names. Pre-processor output is valid Markdown text
 * which is typically rendered to HTML or other representation.
 */
class AutolinkingPreprocessor : public RepresentationInterceptor
{
public:
    static const std::string CODE_BLOCK;
    static const std::string MATH_BLOCK;
    static const std::string MF_URL_PROTOCOL;
    static const std::string FILE_URL_PROTOCOL;

protected:
    std::vector<Thing*> things;
    Trie* trie;
    bool insensitive;

    Mind& mind;

public:
    explicit AutolinkingPreprocessor(Mind& mind);
    AutolinkingPreprocessor(const AutolinkingPreprocessor&) = delete;
    AutolinkingPreprocessor(const AutolinkingPreprocessor&&) = delete;
    AutolinkingPreprocessor &operator=(const AutolinkingPreprocessor&) = delete;
    AutolinkingPreprocessor &operator=(const AutolinkingPreprocessor&&) = delete;
    virtual ~AutolinkingPreprocessor();

    /**
     * @brief Rebuild indices (like trie) e.g. on new MD/repository load.
     */
    virtual void reindex() = 0;

    /**
     * @brief Inject links to given MD source (list of rows) and return valid MD string.
     */
    virtual void process(const std::vector<std::string*>& in, std::string& out) = 0;

    /**
     * @brief Clear indices.
     */
    virtual void clear();

protected:
    /**
     * @brief Comparator used to sort Os/Ns by name (w/ stripped abbreviation prefix).
     */
    static bool aliasSizeComparator(const Thing* t1, const Thing* t2);

    /**
     * @brief Update indice of all Ns and Os.
     */
    void updateThingsIndex();
    /**
     * @brief Update trie-based Os and Ns names index.
     */
    void updateTrieIndex();
    /**
     * @brief Add thing's name (and abbrev) to trie.
     */
    void addThingToTrie(const Thing *t);
};

}
#endif // M8R_AUTOLINKING_PREPROCESSOR_H
