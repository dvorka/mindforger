/*
 ontology.h     MindForger thinking notebook

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
#ifndef M8R_ONTOLOGY_H_
#define M8R_ONTOLOGY_H_

// IMPROVE something wrong w/ this header - had to include it in .cpp files in AST

#include "../../config/configuration.h"
#include "../../model/tag.h"
#include "../../model/outline_type.h"
#include "../../model/note_type.h"
#include "ontology_vocabulary.h"

namespace m8r {

class Configuration;

/**
 * @brief Mind ontology.
 *
 * Rationalization of the huge academic ontology theory to a reasonable
 * extend that suits MindForger needs.
 *
 * An ontology is a formal naming and definition of the types, properties,
 * and interrelationships of the entities that exist in a particular
 * domain. It extends taxonomy (ISA only) with additional relationship types.
 * Ontologies are created to limit complexity and organize information in Memory
 * for Mind.
 */
class Ontology
{
private:
    // IMPROVE classes, attributes, relationships (synonym, antonym, ...)

    // ontology is loaded from the configuration (and by harvesting memory content)
    const Configuration &config;

    // default types
    OutlineType* defaultOutlineType;
    NoteType* defaultNoteType;

    /**
     * Tags are shared among Outlines and Notes (Outline can become Note w/ its children
     * and vice versa) - shared tag vocabulary is better than outline labels/note tags/keywords/...
     *
     * Tag naming convention: lowercase (:alpha :number space); e.g. cool, super cool, ...
     */
    OntologyVocabulary<Tag> tags;

    /**
     * Outline type naming convention: first uppercase; e.g. Grow
     */
    OntologyVocabulary<OutlineType> outlineTypes;

    /**
     * Note type naming convention: first uppercase; e.g. Question
     */
    OntologyVocabulary<NoteType> noteTypes;

public:
    Ontology() = delete;
    Ontology(const Configuration &configuration);
    Ontology(const Ontology&) = delete;
    Ontology(const Ontology&&) = delete;
    Ontology &operator=(const Ontology&) = delete;
    Ontology &operator=(const Ontology&&) = delete;
    virtual ~Ontology();

    void load();
    void save() const;

    /**
     * @brief Find tag w/ key ensuring it exist in one instance through the application.
     * @return tag w/ given key.
     */
    const Tag* findOrCreateTag(const std::string& key);
    OntologyVocabulary<Tag>& getTags() { return tags; }

    const OutlineType* findOrCreateOutlineType(const std::string& key);
    const OutlineType* getDefaultOutlineType() const { return defaultOutlineType; }
    OntologyVocabulary<OutlineType>& getOutlineTypes() { return outlineTypes; }

    const NoteType* findOrCreateNoteType(const std::string& key);
    const NoteType* getDefaultNoteType() const { return defaultNoteType; }
    OntologyVocabulary<NoteType>& getNoteTypes() { return noteTypes; }
};

} /* namespace m8r */

#endif /* M8R_ONTOLOGY_H_ */
