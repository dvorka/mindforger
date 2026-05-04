/*
 organizer.h     MindForger thinking notebook

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
#ifndef M8R_ORGANIZER_H
#define M8R_ORGANIZER_H

#include <string>
#include <vector>

#include "../gear/datetime_utils.h"
#include "../mind/ontology/thing_class_rel_triple.h"
#include "../mind/ontology/ontology.h"

namespace m8r {

/**
 * @brief Organizer - Outlines and Notes sorter.
 */
class Organizer : public Thing
{
public:
    enum OrganizerType {
        EISENHOWER_MATRIX,
        KANBAN,
    };

    enum FilterBy {
        OUTLINES_NOTES,
        OUTLINES,
        NOTES
    };

    static constexpr const auto CONFIG_VALUE_FILTER_BY_O = "notebooks";
    static constexpr const auto CONFIG_VALUE_FILTER_BY_N = "notes";
    static constexpr const auto CONFIG_VALUE_FILTER_BY_O_N = "notebooks and notes";

    static const std::string TYPE_STR_KANBAN;
    static const std::string TYPE_STR_EISENHOWER_MATRIX;

    static std::string createOrganizerKey(
        const std::set<std::string>& keys,
        const std::string& directory,
        const std::string& id,
        const std::string& separator,
        const std::string& type = "organizer"
    );

private:
    std::vector<std::reference_wrapper<std::set<std::string>>> quadrantTags;

public:
    OrganizerType organizerType;

    // EM: upper left quandrant tag / Kanban: 1st column
    std::set<std::string> tagsUlQuadrant;
    // EM: upper right quandrant tag / Kanban: 2nd column
    std::set<std::string> tagsUrQuadrant;
    // EM: lower left quandrant tag / Kanban: 3rd column
    std::set<std::string> tagsLlQuadrant;
    // EM: lower right quandrant tag / Kanban: 4th column
    std::set<std::string> tagsLrQuadrant;

    // values: O, N, O+N; default O+N (show both O and N)
    int filterBy;

    // O or global scope ("")
    std::string scopeOutlineId;

    /*
     * transient fields
     */

    time_t modified;

public:
    explicit Organizer(const std::string& name, OrganizerType organizerType);
    explicit Organizer(const Organizer& o);
    Organizer(const Organizer&&) = delete;
    Organizer& operator=(const Organizer&) = delete;
    Organizer& operator=(const Organizer&&) = delete;
    ~Organizer();

    OrganizerType getOrganizerType() const { return organizerType; }
    const std::string& getOrganizerTypeAsStr() const {
        if(OrganizerType::KANBAN == this->organizerType) {
            return TYPE_STR_KANBAN;
        } else {
            return TYPE_STR_EISENHOWER_MATRIX;
        }
    }

    bool operator<(const Organizer& other) const {
        return modified < other.modified;
    }

    bool operator<(Organizer* other) const {
        return modified < other->modified;
    }

    std::set<std::string>& getStringTagsForQuadrant(unsigned column);
    std::vector<const Tag*> getTagsForQuadrant(unsigned column, Ontology& ontology);

    void setKey(const std::string& key) { this->key = key; }

    std::set<std::string>& getUpperRightTags() {
        return tagsUrQuadrant;
    }
    std::set<std::string>& getLowerRightTags() {
        return this->tagsLrQuadrant;
    }
    std::set<std::string>& getLowerLeftTags() {
        return this->tagsLlQuadrant;
    }
    std::set<std::string>& getUpperLeftTags() {
        return this->tagsUlQuadrant;
    }

    void setUpperRightTag(const std::string tag) {
        this->tagsUrQuadrant.clear();
        this->tagsUrQuadrant.insert(tag);
    }
    void setLowerRightTag(const std::string tag) {
        this->tagsLrQuadrant.clear();
        this->tagsLrQuadrant.insert(tag);
    }
    void setLowerLeftTag(const std::string tag) {
        this->tagsLlQuadrant.clear();
        this->tagsLlQuadrant.insert(tag);
    }
    void setUpperLeftTag(const std::string tag) {
        this->tagsUlQuadrant.clear();
        this->tagsUlQuadrant.insert(tag);
    }

    void setUpperRightTags(const std::set<std::string> tags) {
        this->tagsUrQuadrant.clear();
        this->tagsUrQuadrant = tags;
    }
    void setLowerRightTags(const std::set<std::string> tags) {
        this->tagsLrQuadrant.clear();
        this->tagsLrQuadrant = tags;
    }
    void setLowerLeftTags(const std::set<std::string> tags) {
        this->tagsLlQuadrant.clear();
        this->tagsLlQuadrant = tags;
    }
    void setUpperLeftTags(const std::set<std::string> tags) {
        this->tagsUlQuadrant.clear();
        this->tagsUlQuadrant = tags;
    }

    int getFilterBy() const { return this->filterBy; }
    std::string getFilterByAsStr();
    void setFilterBy(int filterBy) { this->filterBy = filterBy; }

    std::string getOutlineScope() const {
        return this->scopeOutlineId.length() ? this->scopeOutlineId : "";
    }
    void clearOutlineScope() {
        this->scopeOutlineId.clear();
    }
    void setOutlineScope(std::string outlineId) {
        this->scopeOutlineId = outlineId;
    }

    bool isValid() const;

    time_t getModified() const { return this->modified; }
    void makeModified();

private:
    void initQuadrantTags();

};

}
#endif // M8R_ORGANIZER_H
