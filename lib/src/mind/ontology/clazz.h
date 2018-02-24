/*
 clazz.h     MindForger thinking notebook

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
#ifndef M8R_CLAZZ_H
#define M8R_CLAZZ_H

#include "relationship.h"

namespace m8r {

class Thing;

/**
 * @brief Ontology class.
 *
 * See m8r::Ontology.
 */
class Clazz : public Thing
{
private:
    /**
     * @brief Parent class in ISA hierarchy.
     *
     * All Classes are organized to ISA (C++ is-a programming language) hierarchy.
     * The root of ISA hierarchy is the Thing (instance of m8r::Class) defined by Ontology.
     */
    Relationship isA;

public:
    explicit Clazz(std::string& name, Clazz* isA);
    Clazz(const Clazz&) = delete;
    Clazz(const Clazz&&) = delete;
    Clazz &operator=(const Clazz&) = delete;
    Clazz &operator=(const Clazz&&) = delete;
    ~Clazz();
};

}
#endif // M8R_CLAZZ_H
