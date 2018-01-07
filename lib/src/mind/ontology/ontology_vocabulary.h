/*
 ontology_map.h     MindForger thinking notebook

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
#ifndef M8R_ONTOLOGY_VOCABULARY_H_
#define M8R_ONTOLOGY_VOCABULARY_H_

#include <string>
// IMPROVE uniq_map - compiler doesn't support it yet :-Z
#include <map>

namespace m8r {

template <class VALUE>
class OntologyVocabulary {
    // IMPROVE migrate to using once compiler supports it
    typedef typename std::map<std::string,const VALUE*> MAP;
    typedef typename MAP::iterator MAP_ITERATOR;
    typedef typename MAP::size_type MAP_SIZE;

protected:
    MAP entries;

public:
    explicit OntologyVocabulary(void);
    OntologyVocabulary(const OntologyVocabulary&) = delete;
    OntologyVocabulary(const OntologyVocabulary&&) = delete;
    OntologyVocabulary &operator=(const OntologyVocabulary&) = delete;
    OntologyVocabulary &operator=(const OntologyVocabulary&&) = delete;
    ~OntologyVocabulary();

    void put(const std::string& name, const VALUE* label);
    const VALUE* get(const std::string& name);
    bool empty(void) const { return entries.empty(); }
    MAP_ITERATOR begin(void) { return entries.begin(); }
    MAP_ITERATOR end(void) { return entries.end(); }
    void clear(void) { entries.clear(); }
    std::vector<const VALUE*>& values(void) {
        // IMPROVE cache this via dirty flag
        static std::vector<const VALUE*> v;
        v.clear();
        for(MAP_ITERATOR i = entries.begin(); i!=entries.end(); ++i) {
          v.push_back(i->second);
        }
        return v;
    }
    MAP_SIZE size(void) { return entries.size(); }
};

template <class VALUE>
OntologyVocabulary<VALUE>::OntologyVocabulary(void)
{
}

template <class VALUE>
OntologyVocabulary<VALUE>::~OntologyVocabulary()
{
    // destructor doesn't destroy members
}

template <class VALUE>
void OntologyVocabulary<VALUE>::put(const std::string& name, const VALUE* label)
{
    entries[name] = label;
}

template <class VALUE>
const VALUE* OntologyVocabulary<VALUE>::get(const std::string& name)
{
    auto result = entries.find(name);
    return result!=entries.end() ? result->second : nullptr;
}

} /* namespace m8r */

#endif /* M8R_ONTOLOGY_VOCABULARY_H_ */
