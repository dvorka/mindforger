/*
 thing.h     MindForger thinking notebook

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
#ifndef M8R_THING_H_
#define M8R_THING_H_

#include <string>
#include <set>

namespace m8r {

class Relationship;

/**
 * @brief Ontology Thing.
 *
 * See m8r::Ontology.
 */
class Thing
{
protected:
    /**
     * @brief Display name.
     */
    std::string name;

    /**
     * @brief Relationships.
     *
     * Explicit relationships (both incoming and outgoing) distinquished
     * using subject.
     */
    std::set<Relationship*> relationships;

public:
    Thing();
    explicit Thing(std::string name);
    Thing(const Thing&) = delete;
    Thing(const Thing&&) = delete;
    Thing &operator=(const Thing&) = delete;
    Thing &operator=(const Thing&&) = delete;
    virtual ~Thing();

    const std::string& getName() const { return name; }
    void setName(std::string name) { this->name = name; }
};

} // m8r namespace

#endif /* M8R_THING_H_ */
