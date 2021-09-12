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

    static constexpr const auto KEY_EISENHOWER_MATRIX = "/m1ndf0rg3r/organizers/eisenhower-matrix";

    static std::string createOrganizerKey(
        const std::set<std::string>& keys,
        const std::string& directory,
        const std::string& id,
        const std::string& separator
    );

    static Organizer* createEisenhowMatrixOrganizer() {
        Organizer* emO = new Organizer("Eisenhower Matrix");
        emO->setKey(Organizer::KEY_EISENHOWER_MATRIX);

        emO->setUpperRightTag("important & urgent");
        emO->setLowerRightTag("important");
        emO->setUpperLeftTag("urgent");
        emO->setLowerLeftTag(".");

        return emO;
    };

    static constexpr const auto ESC_TAG_DELIMITER = ",,";

    // save tags as strings (w/ escaped delimiter ,, ~ ,)
    static std::string tagsToString(std::vector<std::string>& tags) {
        std::string s{};
        for(std::string t:tags) {
            s.append(t);
            s.append(ESC_TAG_DELIMITER);
        }
        if(s.length()) {
            s = s.substr(0, s.length()-2);
        }
        return s;
    }

    // parse tags from escaped string
    static std::vector<std::string> tagsFromString(std::string& s) {
        std::vector<std::string> tags{};

        if(s.size()) {
            size_t last = 0;
            size_t next = 0;
            while ((next = s.find(ESC_TAG_DELIMITER, last)) != std::string::npos) {
                tags.push_back(s.substr(last, next-last));
                last = next + 2;
            }
            tags.push_back(s.substr(last));
        }

        return tags;
    }

public:
    // upper right quandrant tag
    std::vector<std::string> tagsUrQuadrant;
    // lower right quandrant tag
    std::vector<std::string> tagsLrQuadrant;
    // lower left quandrant tag
    std::vector<std::string> tagsLlQuadrant;
    // upper left quandrant tag
    std::vector<std::string> tagsUlQuadrant;

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

    void setKey(const std::string& key) { this->key = key; }

    std::vector<std::string>& getUpperRightTags() { return this->tagsUrQuadrant; }
    std::vector<std::string>& getLowerRightTags() { return this->tagsLrQuadrant; }
    std::vector<std::string>& getLowerLeftTags() { return this->tagsLlQuadrant; }
    std::vector<std::string>& getUpperLeftTags() { return this->tagsUlQuadrant; }

    void setUpperRightTag(const std::string tag) {
        this->tagsUrQuadrant.clear();
        this->tagsUrQuadrant.push_back(tag);
    }
    void setLowerRightTag(const std::string tag) {
        this->tagsLrQuadrant.clear();
        this->tagsLrQuadrant.push_back(tag);
    }
    void setLowerLeftTag(const std::string tag) {
        this->tagsLlQuadrant.clear();
        this->tagsLlQuadrant.push_back(tag);
    }
    void setUpperLeftTag(const std::string tag) {
        this->tagsUlQuadrant.clear();
        this->tagsUlQuadrant.push_back(tag);
    }

    void setUpperRightTags(const std::vector<std::string> tags) {
        this->tagsUrQuadrant = tags;
    }
    void setLowerRightTags(const std::vector<std::string> tags) {
        this->tagsLrQuadrant = tags;
    }
    void setLowerLeftTags(const std::vector<std::string> tags) {
        this->tagsLlQuadrant = tags;
    }
    void setUpperLeftTags(const std::vector<std::string> tags) {
        this->tagsUlQuadrant = tags;
    }

    int getSortBy() const { return this->sortBy; }
    const std::string getSortByAsStr();
    void setSortBy(int sortBy) { this->sortBy = sortBy; }

    int getFilterBy() const { return this->filterBy; }
    std::string getFilterByAsStr();
    void setFilterBy(int filterBy) { this->filterBy = filterBy; }

    std::string getOutlineScope() const {
        return this->scopeOutlineId.length() ? this->scopeOutlineId : "";
    }
    void setOutlineScope(std::string outlineId) {
        this->scopeOutlineId = outlineId;
    }

    bool isValid() const;
};

}
#endif // M8R_ORGANIZER_H
