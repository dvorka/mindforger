/*
 taxonomy.h     MindForger thinking notebook

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
#ifndef M8R_TAXONOMY_H
#define M8R_TAXONOMY_H

#include "clazz.h"
#include "ontology_vocabulary.h"

namespace m8r {

/**
 * @brief Ontology taxonomy.
 *
 * See m8r::Ontology.
 */
template <class CLAZZ>
class Taxonomy : public Clazz
{
private:
    OntologyVocabulary<CLAZZ> classes;

public:
    explicit Taxonomy(std::string& name, CLAZZ* isA);
    Taxonomy(const Taxonomy&) = delete;
    Taxonomy(const Taxonomy&&) = delete;
    Taxonomy &operator=(const Taxonomy&) = delete;
    Taxonomy &operator=(const Taxonomy&&) = delete;
    ~Taxonomy();

    void add(std::string key, CLAZZ* clazz);
    OntologyVocabulary<CLAZZ> getClasses() { return classes; }
};

template <class CLAZZ>
Taxonomy<CLAZZ>::Taxonomy(std::string& name, CLAZZ* isA)
    : Clazz(name, isA)
{
}

template <class CLAZZ>
Taxonomy<CLAZZ>::~Taxonomy()
{
}

template <class CLAZZ>
void Taxonomy<CLAZZ>::add(std::string name, CLAZZ* clazz)
{
    classes.put(name, clazz);
}

}
#endif // M8R_TAXONOMY_H
