/*
 mind.cpp     MindForger thinking notebook

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
#include "mind.h"

using namespace std;

namespace m8r {

Mind::Mind(Configuration &configuration)
    : config(configuration), memory(configuration)
{
    forget = 0;
}

void Mind::dream(void)
{
    // integrity check: ...

    // memory structure check:
    // - Os w/o description
    // - Os w/o any N
    // - Ns w/o description

    // attachments
    // - orphan attachments (not referenced from any O)
}

void Mind::think(void)
{
    memory.learn();
}

Mind::~Mind()
{
    // - Memory destruct outlines
    // - allNotesCache Notes is just container referencing Memory's Outlines
}

const vector<Note*>& Mind::getMemoryDwell(int pageSize) const
{
    UNUSED_ARG(pageSize);

    return memoryDwell;
}

size_t Mind::getMemoryDwellDepth(void) const
{
    return memoryDwell.size();
}

unique_ptr<vector<Outline*>> Mind::findOutlineByTitleFts(const string& expr) const
{
    // IMPROVE implement regexp and other search options by reusing HSTR code
    // IMPROVE PERF this method is extremely inefficient > use cached map (stack member) evicted on memory modification
    unique_ptr<vector<Outline*>> result{new vector<Outline*>()};
    if(expr.size()) {
        vector<Outline*> outlines = memory.getOutlines();
        for(Outline* outline:outlines) {
            if(!expr.compare(outline->getTitle())) {
                result->push_back(outline);
            }
        }
    }
    return result;
}

vector<Note*>* Mind::findNoteByTitleFts(const string& regexp) const
{
    UNUSED_ARG(regexp);

    return nullptr;
}

void Mind::getOutlineTitles(vector<string>& titles) const
{
    // IMPROVE PERF cache vector (stack member) until and evict on memory modification
    vector<Outline*> outlines = memory.getOutlines();
    for(Outline* outline:outlines) {
        titles.push_back(outline->getTitle());
    }
}

void Mind::findNoteFts(vector<Note*>* result, const string& regexp, const bool ignoreCase, Outline* outline)
{
    string s{};

    // IMPROVE make this faster - do NOT convert to lower case, but compare it in that method > will do less
    if(ignoreCase) {
	// case INSENSITIVE
	s.clear();
	stringToLower(outline->getTitle(), s);
	if(s.find(regexp)!=string::npos) {
	    result->push_back(outline->getOutlineDescriptorAsNote());
	} else {
	    for(string* d:outline->getDescription()) {
		if(d) {
		     s.clear();
		     stringToLower(*d, s);
		     if(s.find(regexp)!=string::npos) {
			 result->push_back(outline->getOutlineDescriptorAsNote());
			 break;
		     }
		}
	    }
	}
	for(Note* note:outline->getNotes()) {
	    s.clear();
	    stringToLower(note->getTitle(), s);
	    if(s.find(regexp)!=string::npos) {
		result->push_back(note);
	    } else {
		for(string* d:note->getDescription()) {
		    if(d) {
			 s.clear();
			 stringToLower(*d, s);
			 if(s.find(regexp)!=string::npos) {
			     result->push_back(note);
			     break;
			 }
		    }
		}
	    }
	}
    } else {
	// case SENSITIVE
	if(outline->getTitle().find(regexp)!=string::npos) {
	    result->push_back(outline->getOutlineDescriptorAsNote());
	} else {
	    for(string* d:outline->getDescription()) {
		if(d && d->find(regexp)!=string::npos) {
		    result->push_back(outline->getOutlineDescriptorAsNote());
		    // avoid multiple matches in the result
		    break;
		}
	    }
	}
	for(Note* note:outline->getNotes()) {
	    if(note->getTitle().find(regexp)!=string::npos) {
		result->push_back(note);
	    } else {
		for(string* d:note->getDescription()) {
		    if(d && d->find(regexp)!=string::npos) {
			result->push_back(note);
			// avoid multiple matches in the result
			break;
		    }
		}
	    }
	}
    }
}

vector<Note*>* Mind::findNoteFts(const string& regexp, const bool ignoreCase, Outline* scope)
{
    if(allNotesCache.size()) {
        allNotesCache.clear();
    }

    vector<Note*>* result = new vector<Note*>();

    string r{};
    if(ignoreCase) {
        stringToLower(regexp, r);
    } else {
        r.append(regexp);
    }

    if(scope) {
        findNoteFts(result, r, ignoreCase, scope);
    } else {
        const vector<m8r::Outline*> outlines = memory.getOutlines();
        for(Outline* outline:outlines) {
            findNoteFts(result, r, ignoreCase, outline);
        }
    }
    return result;
}

vector<Note*>* Mind::getReferencedNotes(const Note& note) const
{
    UNUSED_ARG(note);

    return nullptr;
}

vector<Note*>* Mind::getReferencedNotes(const Note& note, const Outline& outline) const
{
    UNUSED_ARG(note);
    UNUSED_ARG(outline);

    return nullptr;
}

vector<Note*>* Mind::getRefereeNotes(const Note& note) const
{
    UNUSED_ARG(note);

    return nullptr;
}

vector<Note*>* Mind::getRefereeNotes(const Note& note, const Outline& outline) const
{
    UNUSED_ARG(note);
    UNUSED_ARG(outline);

    return nullptr;
}

vector<Note*>* Mind::getTaggedNotes(const vector<Tag*> tags) const
{
    UNUSED_ARG(tags);

    return nullptr;
}

vector<Outline*>* Mind::getOutlinesOfType(const OutlineType& type) const
{
    UNUSED_ARG(type);

    return nullptr;
}

void Mind::getAllNotes(std::vector<Note*>& notes) const
{
    vector<Outline*> outlines = memory.getOutlines();
    for(Outline* o:outlines) {
        for(Note* n:o->getNotes()) {
            notes.push_back(n);
        }
    }
}

vector<Note*>* Mind::getNotesOfType(const NoteType& type) const
{
    UNUSED_ARG(type);

    return nullptr;
}

vector<Note*>* Mind::getNotesOfType(const NoteType& type, const Outline& outline) const
{
    UNUSED_ARG(type);
    UNUSED_ARG(outline);

    return nullptr;
}

vector<Note*>* Mind::getAssociatedNotes(const Note& note) const
{
    UNUSED_ARG(note);

    return nullptr;
}

vector<Note*>* Mind::getAssociatedNotes(const Note& note,
        const Outline& outline) const
{
    UNUSED_ARG(note);
    UNUSED_ARG(outline);

    return nullptr;
}

vector<Note*>* Mind::getAssociatedNotes(const vector<string*> words) const
{
    UNUSED_ARG(words);

    return nullptr;
}

vector<Note*>* Mind::getAssociatedNotes(const vector<string*> words,
        const Outline& outline) const
{
    UNUSED_ARG(words);
    UNUSED_ARG(outline);

    return nullptr;
}

uint8_t Mind::getForgetThreshold(void) const
{
    return forget;
}

vector<Outline*>* Mind::getTaggedOutlines(
        const std::vector<Tag*> labels) const
{
    UNUSED_ARG(labels);

    return nullptr;
}

vector<Tag*>* Mind::getOutlinesTags(void) const
{
    return nullptr;
}

vector<Tag*>* Mind::getTags(void) const
{
    return nullptr;
}

vector<Tag*>* Mind::getNoteTags(const Outline& outline) const
{
    UNUSED_ARG(outline);

    return nullptr;
}

unsigned Mind::getTagCardinality(const Tag& tag) const
{
    UNUSED_ARG(tag);

    return 0;
}

unsigned Mind::getOutlineTagCardinality(const Tag& tag) const
{
    UNUSED_ARG(tag);

    return 0;
}

unsigned Mind::getNoteTagCardinality(const Tag& tag) const
{
    UNUSED_ARG(tag);

    return 0;
}

void Mind::setForgetThreashold(uint8_t forget)
{
    this->forget = forget;
}

string Mind::outlineNew(
    // IMPROVE pass title by reference
    const string* title,
    const OutlineType* outlineType,
    const int8_t importance,
    const int8_t urgency,
    const Tag* tag,
    Stencil* outlineStencil)
{
    string key = memory.createOutlineKey(title);
    Outline* outline{};
    if(outlineStencil) {
        outline = memory.learnOutline(outlineStencil);
        outline->setModified();
    } else {
        outline = new Outline{ontology().getDefaultOutlineType()};
    }

    if(outline) {
        outline->completeProperties(datetimeNow());
        outline->setKey(key);
        if(title && !title->empty()) {
            outline->setTitle(*title);
        }
        if(outlineType) {
            outline->setType(outlineType);
        }
        outline->setImportance(importance);
        outline->setUrgency(urgency);
        if(tag) {
            outline->addTag(tag);
        }
        if(outline->getNotes().empty()) {
            Note* note = new Note{memory.getOntology().getNoteTypes().get(NoteType::KeyNote()), outline};
            note->completeProperties(datetimeNow());
            outline->addNote(note);
        }


        memory.remember(outline);
        onRemembering();
    } else {
        throw MindForgerException("Unable to create new Outline!");
    }

    return outline?outline->getKey():nullptr;
}

bool Mind::outlineForget(string outlineKey)
{
    Outline* o = memory.getOutline(outlineKey);
    if(o) {
        memory.forget(o);
        auto k = memory.createLimboKey(&o->getTitle());
        o->setKey(k);
        moveFile(outlineKey, k);
        return true;
    }
    return false;
}

Note* Mind::noteNew(
        const std::string& outlineKey,
        const uint16_t parentNoteId,
        // IMPROVE pass title by reference
        const std::string* title,
        const NoteType* noteType,
        const Tag* tag,
        Stencil* noteStencil)
{
    Outline* o = memory.getOutline(outlineKey);
    if(o) {
        Note* n = memory.learnNote(noteStencil);
        if(!n) {
            // IMPROVE make note type method parameter w/ a default
            n = new Note(ontology().findOrCreateNoteType(NoteType::KeyNote()),o);
        }
        n->setOutline(o);
        uint16_t offset = (NO_PARENT==parentNoteId?0:parentNoteId);
        if(title) {
            n->setTitle(*title);
        }
        n->setModified();
        n->setModifiedPretty();
        n->completeProperties(n->getModified());
        if(noteType) {
            n->setType(noteType);
        }
        if(tag) {
            n->addTag(tag);
        }

        o->addNote(n, offset);
        return n;
    } else {
        throw MindForgerException("Outline for given key not found!");
    }
}

Outline* Mind::noteForget(Note* note)
{
    Outline* o = note->getOutline();
    if(o) {
        note->getOutline()->forgetNote(note);
        return o;
    } else {
        throw MindForgerException("Unable find Outline from which should be the Note deleted!");
    }
}

bool Mind::noteTop(string outlineKey, uint16_t noteId)
{
    UNUSED_ARG(outlineKey);
    UNUSED_ARG(noteId);

    return false;
}

bool Mind::noteUp(string outlineKey, uint16_t noteId)
{
    UNUSED_ARG(outlineKey);
    UNUSED_ARG(noteId);

    return false;
}

bool Mind::noteDown(string outlineKey, uint16_t noteId)
{
    UNUSED_ARG(outlineKey);
    UNUSED_ARG(noteId);

    return false;
}

bool Mind::noteBottom(string outlineKey, uint16_t noteId)
{
    UNUSED_ARG(outlineKey);
    UNUSED_ARG(noteId);

    return false;
}

bool Mind::notePromote(string outlineKey, uint16_t noteId)
{
    UNUSED_ARG(outlineKey);
    UNUSED_ARG(noteId);

    return false;
}

bool Mind::noteDemote(string outlineKey, uint16_t noteId)
{
    UNUSED_ARG(outlineKey);
    UNUSED_ARG(noteId);

    return false;
}

bool Mind::noteRefactor(
        string fromOutlineKey,
        uint16_t fromNoteId,
        string toOutlineKey,
        uint16_t toParentNoteId)
{
    UNUSED_ARG(fromOutlineKey);
    UNUSED_ARG(fromNoteId);
    UNUSED_ARG(toOutlineKey);
    UNUSED_ARG(toParentNoteId);

    return false;
}

void Mind::onRemembering(void)
{
    allNotesCache.clear();
}

} /* namespace */
