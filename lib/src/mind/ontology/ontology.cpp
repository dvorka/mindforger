/*
 ontology.cpp     MindForger thinking notebook

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
#include "ontology.h"

namespace m8r {

using namespace std;

string Ontology::KEY_THING = string("Thing");
string Ontology::KEY_TAXONOMY_OUTLINE_TYPES = string("OutlineTypes");
string Ontology::KEY_TAXONOMY_NOTE_TYPES = string("NoteTypes");
string Ontology::KEY_TAXONOMY_RELATIONSHIP_TYPES = string("RelationshipTypes");
string Ontology::KEY_TAXONOMY_TAGS = string("Tags");

Ontology::Ontology(const Configuration& configuration)
    : config(configuration),
      thing(KEY_THING, Clazz::ROOT_CLASS)//,
{
    // taxonomy: tags
    tagTaxonomy.setName(KEY_TAXONOMY_TAGS);
    tagTaxonomy.setIsA(&thing);
    tagTaxonomy.add(Tag::KeyCool(), new Tag{Tag::KeyCool(), &tagTaxonomy, Color::MF_BLUE()});
    tagTaxonomy.add(Tag::KeyImportant(), new Tag{Tag::KeyImportant(), &tagTaxonomy, Color::MF_RED()});
    tagTaxonomy.add(Tag::KeyLater(), new Tag{Tag::KeyLater(), &tagTaxonomy, Color::MF_PURPLE()});
    tagTaxonomy.add(Tag::KeyObsolete(), new Tag{Tag::KeyObsolete(), &tagTaxonomy, Color::MF_GRAY()});
    tagTaxonomy.add(Tag::KeyPersonal(), new Tag{Tag::KeyPersonal(), &tagTaxonomy, Color::MF_GREEN()});
    tagTaxonomy.add(Tag::KeyProblem(), new Tag{Tag::KeyProblem(), &tagTaxonomy, Color::MF_BLACK()});
    tagTaxonomy.add(Tag::KeyTodo(), new Tag{Tag::KeyTodo(), &tagTaxonomy, Color::MF_YELLOW()});
    // knowledge type
    tagTaxonomy.add(Tag::KeyWhat(), new Tag{Tag::KeyWhat(), &tagTaxonomy, Color::MF_TURQUOISE()});
    tagTaxonomy.add(Tag::KeyHow(), new Tag{Tag::KeyHow(), &tagTaxonomy, Color::MF_TURQUOISE()});
    tagTaxonomy.add(Tag::KeyWhy(), new Tag{Tag::KeyWhy(), &tagTaxonomy, Color::MF_TURQUOISE()});
    tagTaxonomy.add(Tag::KeyWhere(), new Tag{Tag::KeyWhere(), &tagTaxonomy, Color::MF_TURQUOISE()});
    tagTaxonomy.add(Tag::KeyWho(), new Tag{Tag::KeyWho(), &tagTaxonomy, Color::MF_TURQUOISE()});
    taxonomies[tagTaxonomy.getName()] = &tagTaxonomy;

    // taxonomy: outline types
    outlineTypeTaxonomy.setName(KEY_TAXONOMY_OUTLINE_TYPES);
    outlineTypeTaxonomy.setIsA(&thing);
    this->defaultOutlineType = new OutlineType{OutlineType::KeyOutline(), &outlineTypeTaxonomy, Color::MF_GRAY()};;
    outlineTypeTaxonomy.add(OutlineType::KeyOutline(), defaultOutlineType);
    outlineTypeTaxonomy.add(OutlineType::KeyGrow(), new OutlineType(OutlineType::KeyGrow(), &outlineTypeTaxonomy, Color::MF_GRAY()));

    // taxonomy: note types
    noteTypeTaxonomy.setName(KEY_TAXONOMY_NOTE_TYPES);
    noteTypeTaxonomy.setIsA(&thing);
    this->defaultNoteType = new NoteType{NoteType::KeyNote(), &noteTypeTaxonomy, Color::MF_GRAY()};
    noteTypeTaxonomy.add(NoteType::KeyNote(), defaultNoteType);
    noteTypeTaxonomy.add(NoteType::KeyAction(), new NoteType{NoteType::KeyAction(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyConclusion(), new NoteType{NoteType::KeyConclusion(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyExperience(), new NoteType{NoteType::KeyExperience(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyFact(), new NoteType{NoteType::KeyFact(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyIdea(), new NoteType{NoteType::KeyIdea(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyLesson(), new NoteType{NoteType::KeyLesson(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyOpportunity(), new NoteType{NoteType::KeyOpportunity(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyOption(), new NoteType{NoteType::KeyOption(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyQuestion(), new NoteType{NoteType::KeyQuestion(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyResult(), new NoteType{NoteType::KeyResult(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeySolution(), new NoteType{NoteType::KeySolution(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyStrength(), new NoteType{NoteType::KeyStrength(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyTask(), new NoteType{NoteType::KeyTask(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyThreat(), new NoteType{NoteType::KeyThreat(), &noteTypeTaxonomy, Color::MF_GRAY()});
    noteTypeTaxonomy.add(NoteType::KeyWeakness(), new NoteType{NoteType::KeyWeakness(), &noteTypeTaxonomy, Color::MF_GRAY()});

    // relationship types
    RELATIONSHIP_TYPE_IS_A = new RelationshipType(RelationshipType::KeyIsA(), &outlineTypeTaxonomy, Color::MF_GREEN());
    RELATIONSHIP_TYPE_IS_A->setReflexive(true);
    RELATIONSHIP_TYPE_IS_A->setTransitive(true);
    RELATIONSHIP_TYPE_SAME_AS = new RelationshipType(RelationshipType::KeySameAs(), &outlineTypeTaxonomy, Color::MF_GREEN());
    RELATIONSHIP_TYPE_SAME_AS->setReflexive(true);
    RELATIONSHIP_TYPE_SAME_AS->setTransitive(true);
    RELATIONSHIP_TYPE_SAME_AS->setSymetric(true);
    RELATIONSHIP_TYPE_OPPOSITE_OF = new RelationshipType(RelationshipType::KeyOppositeOf(), &outlineTypeTaxonomy, Color::MF_GREEN());
    RELATIONSHIP_TYPE_OPPOSITE_OF->setTransitive(true);
    RELATIONSHIP_TYPE_DEPENDS_ON = new RelationshipType(RelationshipType::KeyDependsOn(), &outlineTypeTaxonomy, Color::MF_GREEN());
    RELATIONSHIP_TYPE_DEPENDS_ON->setTransitive(true);

    relationshipTypeTaxonomy.setName(KEY_TAXONOMY_RELATIONSHIP_TYPES);
    relationshipTypeTaxonomy.setIsA(&thing);
    relationshipTypeTaxonomy.add(RelationshipType::KeyIsA(), RELATIONSHIP_TYPE_IS_A);
    relationshipTypeTaxonomy.add(RelationshipType::KeySameAs(), RELATIONSHIP_TYPE_SAME_AS);
    relationshipTypeTaxonomy.add(RelationshipType::KeyOppositeOf(), RELATIONSHIP_TYPE_OPPOSITE_OF);
    relationshipTypeTaxonomy.add(RelationshipType::KeyDependsOn(), RELATIONSHIP_TYPE_DEPENDS_ON);

    // ontology metamodel ~ relationships among taxonomies and their categories, relationships, ...
    string tripleName{};
    metamodel.push_back(
        new Triple(
             tripleName,
             noteTypeTaxonomy.get(NoteType::KeyStrength()), RELATIONSHIP_TYPE_OPPOSITE_OF, noteTypeTaxonomy.get(NoteType::KeyWeakness())));
    metamodel.push_back(
        new Triple(
             tripleName,
             noteTypeTaxonomy.get(NoteType::KeyOpportunity()), RELATIONSHIP_TYPE_OPPOSITE_OF, noteTypeTaxonomy.get(NoteType::KeyThreat())));
}

Ontology::~Ontology()
{
    if(!tagTaxonomy.empty()) {
        for(auto& t:tagTaxonomy.getClasses()) {
            delete t.second;
        }
        tagTaxonomy.clear();
    }
    if(!outlineTypeTaxonomy.empty()) {
        for(auto& t:outlineTypeTaxonomy.getClasses()) {
            delete t.second;
        }
        outlineTypeTaxonomy.clear();
    }
    if(!noteTypeTaxonomy.empty()) {
        for(auto& t:noteTypeTaxonomy.getClasses()) {
            delete t.second;
        }
        noteTypeTaxonomy.clear();
    }
    if(!relationshipTypeTaxonomy.empty()) {
        for(auto& t:relationshipTypeTaxonomy.getClasses()) {
            delete t.second;
        }
        relationshipTypeTaxonomy.clear();
    }
    if(!metamodel.empty()) {
        for(auto& t:metamodel) {
            delete t;
        }
        metamodel.clear();
    }
}

const Tag* Ontology::findOrCreateTag(const string& key) {
    // by convention tags are in LOWERCASE
    std::string k{};
    stringToLower(key, k);
    auto result = tagTaxonomy.get(k);
    if(!result) {
        result = new Tag(k, &tagTaxonomy, Color::MF_GRAY());
        tagTaxonomy.add(k, result);
    }
    return result;
}

const OutlineType* Ontology::findOrCreateOutlineType(const string& key) {
    auto result = outlineTypeTaxonomy.get(key);
    if(!result) {
        result = new OutlineType(key, &outlineTypeTaxonomy, Color::DARK_GRAY());
        outlineTypeTaxonomy.add(key, result);
    }
    return result;
}

const NoteType* Ontology::findOrCreateNoteType(const std::string& key) {
    auto result = noteTypeTaxonomy.get(key);
    if(!result) {
        result = new NoteType(key, &noteTypeTaxonomy, Color::DARK_GRAY());
        noteTypeTaxonomy.add(key, result);
    }
    return result;
}

void Ontology::load()
{
}

void Ontology::save() const
{
}

} // m8r namespace
