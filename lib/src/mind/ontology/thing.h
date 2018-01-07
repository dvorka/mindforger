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

#include "../../config/color.h"

namespace m8r {

// TODO make category a thing

/**
 * @brief Ontology concept aka Thing.
 *
 * All outline labels and note tags are organized to MindForger ontology that
 * defines semantical hierarchy of all maintained knowledge - labels/tags
 * annotate outlines/notes, thus by organizing labels/tags are in fact
 * organized all outlines/notes into a single semantical tree.
 *
 * Thing is a root of the hierarchy. Any newly created label/tag becomes child
 * of Thing. User (or MindForger) may move label/tag around the ontology.
 *
 * Although Thing ontology forms a single tree it is multi-layered - sub-trees
 * that beginning just below Thing. Such layer defines facet i.e. technical,
 * functional, emotional, etc. view of the label/tag.
 */
class Thing
{
protected:
    std::string name;
    const Color& color;

    /**
     * @brief Parent concept.
     *
     * All things are organized to ISA hierarchy ("C++ ISA programming language).
     */
    //Thing isA;

public:
    explicit Thing(std::string name, const Color& color);
    Thing(const Thing&) = delete;
    Thing(const Thing&&) = delete;
    Thing &operator=(const Thing&) = delete;
    Thing &operator=(const Thing&&) = delete;
    virtual ~Thing();

    const Color& getColor() const;
    const std::string& getName() const;
};

} /* namespace m8r */

#endif /* M8R_THING_H_ */
