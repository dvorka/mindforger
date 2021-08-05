/*
 organizer.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_ORGANIZER_H
#define M8R_ORGANIZER_H

#include <string>
#include <vector>

#include "../mind/ontology/thing_class_rel_triple.h"

namespace m8r {

/**
 * @brief Organizer - Eisenhower Matrix style Outlines and Notes sorter.
 */
class Organizer : public Thing
{
public:
    enum SortBy {
        IMPORTANCE,
        URGENCY
    };

    enum FilterBy {
        OUTLINES_NOTES,
        OUTLINES,
        NOTES
    };

    static constexpr const auto CONFIG_VALUE_SORT_BY_I = "importance";
    static constexpr const auto CONFIG_VALUE_SORT_BY_U = "urgency";
    static constexpr const auto CONFIG_VALUE_FILTER_BY_O = "outlines";
    static constexpr const auto CONFIG_VALUE_FILTER_BY_N = "notes";
    static constexpr const auto CONFIG_VALUE_FILTER_BY_O_N = "outlines and notes";

public:
    // upper right quandrant tag
    std::string tagUrQuadrant;
    // lower right quandrant tag
    std::string tagLrQuadrant;
    // lower left quandrant tag
    std::string tagLlQuadrant;
    // upper left quandrant tag
    std::string tagUlQuadrant;

    // values: importance, urgency; default: importance
    int sortBy;
    // values: O, N, O+N; default O+N (show both O and N)
    int filterBy;

    // O or global scope (null)
    std::string scopeOutlineId;

public:
    explicit Organizer(const std::string& name);
    Organizer(const Organizer&) = delete;
    Organizer(const Organizer&&) = delete;
    Organizer &operator=(const Organizer&) = delete;
    Organizer &operator=(const Organizer&&) = delete;
    ~Organizer();

    std::string& getUpperRightTag() { return this->tagUrQuadrant; }
    std::string& getLowerRightTag() { return this->tagLrQuadrant; }
    std::string& getLowerLeftTag() { return this->tagLlQuadrant; }
    std::string& getUpperLeftTag() { return this->tagUlQuadrant; }

    void setUpperRightTag(std::string tag) { this->tagUrQuadrant = tag; }
    void setLowerRightTag(std::string tag) { this->tagLrQuadrant = tag; }
    void setLowerLeftTag(std::string tag) { this->tagLlQuadrant = tag; }
    void setUpperLeftTag(std::string tag) { this->tagUlQuadrant = tag; }

    int getSortBy() const { return this->sortBy; }
    const std::string getSortByAsStr();
    int getFilterBy() const { return this->filterBy; }
    std::string getFilterByAsStr();

    std::string getOutlineScope() {
        return this->scopeOutlineId.length() ? this->scopeOutlineId : "";
    }

    bool isValid() const;
};

}
#endif // M8R_ORGANIZER_H
