/*
 memory.cpp     MindForger thinking notebook

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
#include "memory.h"

#include "../gear/string_utils.h"

using namespace std;

namespace m8r {

Memory::Memory(Configuration& configuration)
    : config(configuration),
      repositoryIndexer(*config.getActiveRepository()),
      ontology(config),
      representation(ontology)
{
    persistence = new FilesystemPersistence{configuration, representation};
    cache = true;
}

vector<Stencil*>& Memory::getStencils(ResourceType type)
{
    switch(type) {
    case ResourceType::OUTLINE:
        return outlineStencils;
    case ResourceType::NOTE:
        return noteStencils;
    default:
        throw MindForgerException{"Unknown stencil type"};
    }
}

void Memory::learn()
{
    repositoryIndexer.updateIndex();

    MF_DEBUG("\nMarkdown files:");
    for(const string* markdownFile:repositoryIndexer.getMarkdownFiles()) {
        MF_DEBUG("\n  '" << *markdownFile << "'");
        Outline *outline = representation.outline(File(*markdownFile));
        outlines.push_back(outline);
        outlinesMap.insert(map<string,Outline*>::value_type(outline->getKey(), outline));
    }

    MF_DEBUG("\nOutline stencils:");
    for(const string* file:repositoryIndexer.getOutlineStencilsFileNames()) {
        Stencil* stencil = new Stencil{*file, ResourceType::OUTLINE};
        persistence->load(stencil);
        outlineStencils.push_back(stencil);
        MF_DEBUG("\n  " << stencil->getFilePath());
    }

    MF_DEBUG("\nNote stencils:");
    for(const string* file:repositoryIndexer.getNoteStencilsFileNames()) {
        Stencil* stencil = new Stencil{*file, ResourceType::NOTE};
        persistence->load(stencil);
        noteStencils.push_back(stencil);
        MF_DEBUG("\n  " << stencil->getFilePath());
    }

    MF_DEBUG("\n");
    // IMPROVE consider repositoryIndexer.clean() to save memory
}

Outline* Memory::learnOutline(Stencil* stencil)
{
    if(stencil && ResourceType::OUTLINE==stencil->getType()) {
        return representation.outline(File(stencil->getFilePath()));
    } else {
        return nullptr;
    }
}

Note* Memory::learnNote(Stencil* stencil)
{
    if(stencil && ResourceType::NOTE==stencil->getType()) {
        return representation.note(File(stencil->getFilePath()));
    } else {
        return nullptr;
    }
}

void Memory::remember(const std::string& outlineKey)
{
    Outline* o;
    if((o=getOutline(outlineKey)) != nullptr) {
        persistence->save(o);
    } else {
        throw MindForgerException{"Save: unable to find outline w/ given key"};
    }
}

void Memory::remember(Outline* outline)
{
    persistence->save(outline);
    if(!getOutline(outline->getKey())) {
        outlines.push_back(outline);
        outlinesMap.insert(map<string,Outline*>::value_type(outline->getKey(), outline));
    }
}

void Memory::forget(Outline* outline)
{
    outlinesMap.erase(outline->getKey());
    limboOutlines.push_back(outline);
    outlines.erase(std::remove(outlines.begin(), outlines.end(), outline), outlines.end());
}

Memory::~Memory()
{
    for(Outline*& outline:outlines) {
        delete outline;
    }
    for(Outline*& outline:limboOutlines) {
        delete outline;
    }
    for(Stencil*& stencil:outlineStencils) {
        delete stencil;
    }
    for(Stencil*& stencil:noteStencils) {
        delete stencil;
    }
    delete persistence;
}

int Memory::getOutlinesCount(void) const
{
    return outlines.size();
}

unsigned Memory::getOutlineMarkdownsSize(void) const
{
    // IMPROVE introduce dirty flag (add/remove outline and recount only then)
    unsigned int result{};
    for(Outline* outline:outlines) {
        result+=outline->getBytesize();
    }
    return result;
}

int Memory::getNotesCount(void) const
{
    // IMPROVE introduce dirty flag (add/remove outline and recount only then)
    unsigned int result{};
    for(Outline* outline:outlines) {
        result+=outline->getNotes().size();
    }
    return result;
}

const vector<Outline*>& Memory::getOutlines(void) const
{
    return outlines;
}

bool compareStrings(const Outline* o1, const Outline* o2)
{
    return o1->getTitle().compare(o2->getTitle()) < 0;
}

void Memory::sortByTitle(vector<Outline*>& os)
{
    std::sort(os.begin(), os.end(), compareStrings);
}

string Memory::createOutlineKey(const string* title)
{
    return persistence->createFileName(config.getMemoryPath(), title, string(FILE_EXTENSION_MARKDOWN));
}

string Memory::createLimboKey(const string* title)
{
    return persistence->createFileName(config.getLimboPath(), title, string(FILE_EXTENSION_MARKDOWN));
}

Outline* Memory::getOutline(const string& key)
{
    map<string,Outline*>::const_iterator entry = outlinesMap.find(key);
    if(entry == outlinesMap.end()) {
        return nullptr;
    } else {
        return entry->second;
    }
}

vector<string*>* Memory::ftsMatch(const string* exactMatchString) const
{
    UNUSED_ARG(exactMatchString);

    return nullptr;
}

vector<string*>* Memory::fts(const string* regexp) const
{
    UNUSED_ARG(regexp);

    return nullptr;
}

const OutlineType* Memory::toOutlineType(const MarkdownAstSectionMetadata& meta)
{
    UNUSED_ARG(meta);

    return ontology.getDefaultOutlineType();
}

} /* namespace */
