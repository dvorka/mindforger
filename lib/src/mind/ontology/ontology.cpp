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
      // IMPROVE from default constructor to NORMAL constructor
      thing(KEY_THING, Clazz::ROOT_CLASS)//,
//      outlineTypeTaxonomy(),
//      noteTypeTaxonomy(),
//      relationshipTypeTaxonomy(),
//      tagTaxonomy()
{
    // taxonomy: tags
//    tagsTaxonomy->add(Tag::KeyCool(), new Tag{Tag::KeyCool(), taxonomy, Color::MF_BLUE()});
//    tagsTaxonomy->add(Tag::KeyImportant(), new Tag{Tag::KeyImportant(), taxonomy, Color::MF_RED()});
//    tagsTaxonomy->add(Tag::KeyLater(), new Tag{Tag::KeyLater(), taxonomy, Color::MF_PURPLE()});
//    tagsTaxonomy->add(Tag::KeyObsolete(), new Tag{Tag::KeyObsolete(), taxonomy, Color::MF_GRAY()});
//    tagsTaxonomy->add(Tag::KeyPersonal(), new Tag{Tag::KeyPersonal(), taxonomy, Color::MF_GREEN()});
//    tagsTaxonomy->add(Tag::KeyProblem(), new Tag{Tag::KeyProblem(), taxonomy, Color::MF_BLACK()});
//    tagsTaxonomy->add(Tag::KeyTodo(), new Tag{Tag::KeyTodo(), taxonomy, Color::MF_YELLOW()});
//    tagsTaxonomies.put(taxonomy->geName(),taxonomy);




    // relationship types
    // relationships






//    // taxonomy: outline types
//    this->defaultOutlineType = new OutlineType{OutlineType::KeyOutline(), Color::MF_GRAY()};;
//    outlineTypes.put(OutlineType::KeyOutline(), defaultOutlineType);
//    outlineTypes.put(OutlineType::KeyGrow(), new OutlineType(OutlineType::KeyGrow(), Color::MF_GRAY()));

//    // taxonomy: note types
//    this->defaultNoteType = new NoteType{NoteType::KeyNote(), Color::MF_GRAY()};
//    noteTypes.put(NoteType::KeyNote(), defaultNoteType);
//    noteTypes.put(NoteType::KeyAction(), new NoteType{NoteType::KeyAction(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyConclusion(), new NoteType{NoteType::KeyConclusion(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyExperience(), new NoteType{NoteType::KeyExperience(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyFact(), new NoteType{NoteType::KeyFact(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyIdea(), new NoteType{NoteType::KeyIdea(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyLesson(), new NoteType{NoteType::KeyLesson(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyOpportunity(), new NoteType{NoteType::KeyOpportunity(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyOption(), new NoteType{NoteType::KeyOption(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyQuestion(), new NoteType{NoteType::KeyQuestion(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyResult(), new NoteType{NoteType::KeyResult(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeySolution(), new NoteType{NoteType::KeySolution(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyStrength(), new NoteType{NoteType::KeyStrength(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyTask(), new NoteType{NoteType::KeyTask(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyThreat(), new NoteType{NoteType::KeyThreat(), Color::MF_GRAY()});
//    noteTypes.put(NoteType::KeyWeakness(), new NoteType{NoteType::KeyWeakness(), Color::MF_GRAY()});
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
