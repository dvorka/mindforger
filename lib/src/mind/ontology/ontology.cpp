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

Ontology::Ontology(const Configuration& configuration)
    : config(configuration),
      thing("Thing"),
      outlineTypeTaxonomy("OutlineTypes", thing),
      noteTypeTaxonomy("NoteTypes", thing),
      relationshipTypeTaxonomy("RelationshipTypes", thing),
      tagsTaxonomy("Tags", thing)

{
    // taxonomy: tags
    tagsTaxonomy->add(Tag::KeyCool(), new Tag{Tag::KeyCool(), taxonomy, Color::MF_BLUE()});
    tagsTaxonomy->add(Tag::KeyImportant(), new Tag{Tag::KeyImportant(), taxonomy, Color::MF_RED()});
    tagsTaxonomy->add(Tag::KeyLater(), new Tag{Tag::KeyLater(), taxonomy, Color::MF_PURPLE()});
    tagsTaxonomy->add(Tag::KeyObsolete(), new Tag{Tag::KeyObsolete(), taxonomy, Color::MF_GRAY()});
    tagsTaxonomy->add(Tag::KeyPersonal(), new Tag{Tag::KeyPersonal(), taxonomy, Color::MF_GREEN()});
    tagsTaxonomy->add(Tag::KeyProblem(), new Tag{Tag::KeyProblem(), taxonomy, Color::MF_BLACK()});
    tagsTaxonomy->add(Tag::KeyTodo(), new Tag{Tag::KeyTodo(), taxonomy, Color::MF_YELLOW()});
    tagsTaxonomies.put(taxonomy->geName(),taxonomy);




    // relationship types
    // relationships






    // taxonomy: outline types
    this->defaultOutlineType = new OutlineType{OutlineType::KeyOutline(), Color::MF_GRAY()};;
    outlineTypes.put(OutlineType::KeyOutline(), defaultOutlineType);
    outlineTypes.put(OutlineType::KeyGrow(), new OutlineType(OutlineType::KeyGrow(), Color::MF_GRAY()));

    // taxonomy: note types
    this->defaultNoteType = new NoteType{NoteType::KeyNote(), Color::MF_GRAY()};
    noteTypes.put(NoteType::KeyNote(), defaultNoteType);
    noteTypes.put(NoteType::KeyAction(), new NoteType{NoteType::KeyAction(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyConclusion(), new NoteType{NoteType::KeyConclusion(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyExperience(), new NoteType{NoteType::KeyExperience(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyFact(), new NoteType{NoteType::KeyFact(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyIdea(), new NoteType{NoteType::KeyIdea(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyLesson(), new NoteType{NoteType::KeyLesson(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyOpportunity(), new NoteType{NoteType::KeyOpportunity(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyOption(), new NoteType{NoteType::KeyOption(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyQuestion(), new NoteType{NoteType::KeyQuestion(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyResult(), new NoteType{NoteType::KeyResult(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeySolution(), new NoteType{NoteType::KeySolution(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyStrength(), new NoteType{NoteType::KeyStrength(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyTask(), new NoteType{NoteType::KeyTask(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyThreat(), new NoteType{NoteType::KeyThreat(), Color::MF_GRAY()});
    noteTypes.put(NoteType::KeyWeakness(), new NoteType{NoteType::KeyWeakness(), Color::MF_GRAY()});
}

Ontology::~Ontology()
{
    delete thing;
    thing = nullptr;

    if(!tags.empty()) {
        for(auto& t:tags) {
            delete t.second;
        }
        tags.clear();
    }
    if(!outlineTypes.empty()) {
        for(auto& t:outlineTypes) {
            delete t.second;
        }
        outlineTypes.clear();
    }
    if(!noteTypes.empty()) {
        for(auto& t:noteTypes) {
            delete t.second;
        }
        noteTypes.clear();
    }
}

const Tag* Ontology::findOrCreateTag(const std::string& key) {
    // by convention tags are in LOWERCASE
    std::string k{};
    stringToLower(key, k);
    auto result = tags.get(k);
    if(!result) {
        result = new Tag(k, Color::MF_GRAY());
        tags.put(k,result);
    }
    return result;
}

const OutlineType* Ontology::findOrCreateOutlineType(const std::string& key) {
    auto result = outlineTypes.get(key);
    if(!result) {
        result = new OutlineType(key, Color::DARK_GRAY());
        outlineTypes.put(key,result);
    }
    return result;
}

const NoteType* Ontology::findOrCreateNoteType(const std::string& key) {
    auto result = noteTypes.get(key);
    if(!result) {
        result = new NoteType(key, Color::DARK_GRAY());
        noteTypes.put(key,result);
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
