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
      ontology(),
      representation(ontology)
{
    persistence = new FilesystemPersistence{representation};
    cache = true;
    mindScope = nullptr;
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
    aware = true;

    repositoryIndexer.index(config.getActiveRepository());

#ifdef DO_M8R_DEBUG
    MF_DEBUG(endl << "LEARNING repository in mode " << config.getActiveRepository()->getMode() << ":");
    auto begin = chrono::high_resolution_clock::now();
#endif

    if(config.getActiveRepository()->getMode() == Repository::RepositoryMode::REPOSITORY) {
        MF_DEBUG(endl << "Markdown files:");
        for(const string* markdownFile:repositoryIndexer.getMarkdownFiles()) {
            Outline* outline = representation.outline(File(*markdownFile));
            MF_DEBUG(endl << "  '" << *markdownFile << "' format " << (outline->getFormat()==MarkdownDocument::Format::MINDFORGER?"MF":"MD"));

            // fix O type according to repository type
            switch(config.getActiveRepository()->getType()) {
            case Repository::RepositoryType::MINDFORGER:
                outline->setFormat(MarkdownDocument::Format::MINDFORGER);
                break;
            case Repository::RepositoryType::MARKDOWN:
                outline->setFormat(MarkdownDocument::Format::MARKDOWN);
                break;
            }

            if(outline->isVirgin()) {
                MF_DEBUG(endl << "    VIRGIN ~ most probably wrongly parsed > SKIPPING it");
                delete outline;
            } else {
                outlines.push_back(outline);
                outlinesMap.insert(map<string,Outline*>::value_type(outline->getKey(), outline));
            }
        }

        MF_DEBUG(endl << "Outline stencils:");
        for(const string* file:repositoryIndexer.getOutlineStencilsFileNames()) {
            Stencil* stencil = new Stencil{*file, ResourceType::OUTLINE};
            persistence->load(stencil);
            outlineStencils.push_back(stencil);
            MF_DEBUG(endl << "  " << stencil->getFilePath());
        }

        MF_DEBUG(endl << "Note stencils:");
        for(const string* file:repositoryIndexer.getNoteStencilsFileNames()) {
            Stencil* stencil = new Stencil{*file, ResourceType::NOTE};
            persistence->load(stencil);
            noteStencils.push_back(stencil);
            MF_DEBUG(endl << "  " << stencil->getFilePath());
        }

        MF_DEBUG(endl);
        // IMPROVE consider repositoryIndexer.clean() to save memory
    } else {
        MF_DEBUG(endl << "Single markdown file: " << repositoryIndexer.getMarkdownFiles().size());
        if(repositoryIndexer.getMarkdownFiles().size() == 1) {
            const string* markdownFile = *repositoryIndexer.getMarkdownFiles().begin();
            Outline* outline = representation.outline(File(*markdownFile));
            MF_DEBUG(endl << "  '" << *markdownFile << "' format " << (outline->getFormat()==MarkdownDocument::Format::MINDFORGER?"MF":"MD"));

            // MD file format determines repository type
            repositoryIndexer.getRepository()->setMode(Repository::RepositoryMode::FILE);
            if(outline->getFormat() == MarkdownDocument::Format::MINDFORGER) {
                repositoryIndexer.getRepository()->setType(Repository::RepositoryType::MINDFORGER);
            } else {
                repositoryIndexer.getRepository()->setType(Repository::RepositoryType::MARKDOWN);
            }

            if(outline->isVirgin()) {
                MF_DEBUG(endl << "    VIRGIN ~ most probably wrongly parsed > SKIPPING it");
                delete outline;
            } else {
                outlines.push_back(outline);
                outlinesMap.insert(map<string,Outline*>::value_type(outline->getKey(), outline));
            }

            MF_DEBUG(endl);
        } // else wrong number of files (typically none)
    }

#ifdef DO_M8R_DEBUG
    auto end = chrono::high_resolution_clock::now();
    MF_DEBUG("LEARNED in " << chrono::duration_cast<chrono::microseconds>(end-begin).count()/1000.0 << "ms" << endl);
#endif
}

void Memory::amnesia()
{
    aware = false;

    repositoryIndexer.clear();

    // IMPROVE reset ontology i.e. clear custom types & keep only default ontology
    // ontology.reset();

    for(Outline*& outline:outlines) {
        delete outline;
    }
    outlines.clear();
    outlinesMap.clear();

    for(Outline*& outline:limboOutlines) {
        delete outline;
    }
    limboOutlines.clear();

    for(Stencil*& stencil:outlineStencils) {
        delete stencil;
    }
    outlineStencils.clear();

    for(Stencil*& stencil:noteStencils) {
        delete stencil;
    }
    noteStencils.clear();
}

Outline* Memory::createOutline(Stencil* stencil)
{
    if(stencil && ResourceType::OUTLINE==stencil->getType()) {
        return representation.outline(File(stencil->getFilePath()));
    } else {
        return nullptr;
    }
}

Note* Memory::createNote(Stencil* stencil)
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
        o->makeModified();
        o->checkAndFixProperties();
        persistence->save(o);
    } else {
        throw MindForgerException{"Save: unable to find outline w/ given key"};
    }
}

void Memory::remember(Outline* outline)
{
    if(config.getActiveRepository()->getType() == Repository::RepositoryType::MINDFORGER) {
        outline->setFormat(MarkdownDocument::Format::MINDFORGER);
    } else {
        outline->setFormat(MarkdownDocument::Format::MARKDOWN);
    }

    outline->checkAndFixProperties();
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

unsigned Memory::getOutlinesCount() const
{
    return outlines.size();
}

unsigned Memory::getOutlineMarkdownsSize() const
{
    // IMPROVE introduce dirty flag (add/remove outline and recount only then)
    unsigned int result{};
    for(Outline* outline:outlines) {
        result+=outline->getBytesize();
    }
    return result;
}

unsigned Memory::getNotesCount() const
{
    // IMPROVE PERFORMANCE cache this number & introduce dirty flag (add/remove outline and recount only then)
    unsigned int result{};
    for(Outline* outline:outlines) {
        result+=outline->getNotes().size();
    }
    return result;
}

const vector<Outline*>& Memory::getOutlines() const
{
    return outlines;
}

bool compareStrings(const Outline* o1, const Outline* o2)
{
    return o1->getName().compare(o2->getName()) < 0;
}

void Memory::sortByName(vector<Outline*>& os)
{
    std::sort(os.begin(), os.end(), compareStrings);
}

string Memory::createOutlineKey(const string* name)
{
    return persistence->createFileName(config.getMemoryPath(), name, string(FILE_EXTENSION_MD_MD));
}

string Memory::createLimboKey(const string* name)
{
    return persistence->createFileName(config.getLimboPath(), name, string(FILE_EXTENSION_MD_MD));
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

void Memory::getAllNotes(vector<Note*>& notes) const
{
    for(Outline* o:outlines) {
        for(Note* n:o->getNotes()) {
            if(mindScope) {
                if(mindScope->isInScope(n)) {
                    notes.push_back(n);
                }
            } else {
                notes.push_back(n);
            }
        }
    }
}

const OutlineType* Memory::toOutlineType(const MarkdownAstSectionMetadata& meta)
{
    UNUSED_ARG(meta);

    return ontology.getDefaultOutlineType();
}

} // m8r namespace
