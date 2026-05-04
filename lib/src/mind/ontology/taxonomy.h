/*
 taxonomy.h     MindForger thinking notebook

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
#ifndef M8R_TAXONOMY_H
#define M8R_TAXONOMY_H

#include "ontology_vocabulary.h"

namespace m8r {

class Clazz;

/**
 * @brief Ontology taxonomy.
 *
 * See m8r::Ontology.
 */
template <class CLAZZ>
class Taxonomy : public Clazz
{
    // IMPROVE migrate to using once compiler supports it
    typedef typename std::map<std::string,const CLAZZ*> MAP;
    typedef typename MAP::iterator MAP_ITERATOR;
    typedef typename MAP::size_type MAP_SIZE;

private:
    OntologyVocabulary<CLAZZ> classes;

public:
    explicit Taxonomy();
    explicit Taxonomy(std::string& name, CLAZZ* isA);
    Taxonomy(const Taxonomy&) = delete;
    Taxonomy(const Taxonomy&&) = delete;
    Taxonomy &operator=(const Taxonomy&) = delete;
    Taxonomy &operator=(const Taxonomy&&) = delete;
    ~Taxonomy();

    bool empty() const { return classes.empty(); }
    MAP_SIZE size() { return classes.size(); }
    const CLAZZ* get(const std::string& name);
    void add(const std::string& key, const CLAZZ* clazz);
    std::vector<const CLAZZ*>& values() { return classes.values(); }
    void clear() { classes.clear(); }

    OntologyVocabulary<CLAZZ>& getClasses() { return classes; }
};

template <class CLAZZ>
Taxonomy<CLAZZ>::Taxonomy()
    : Clazz("", nullptr)
{
}

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
const CLAZZ* Taxonomy<CLAZZ>::get(const std::string& name)
{
    return classes.get(name);
}

template <class CLAZZ>
void Taxonomy<CLAZZ>::add(const std::string& name, const CLAZZ* clazz)
{
    classes.put(name, clazz);
}

}
#endif // M8R_TAXONOMY_H
