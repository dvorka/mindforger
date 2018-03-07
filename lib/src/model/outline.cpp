/*
 outline.cpp     MindForger thinking notebook

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
#include "outline.h"

using namespace std;

namespace m8r {

// IMPROVE this type is not bound to any parent Clazz in Ontology
const NoteType Outline::NOTE_4_OUTLINE_TYPE{"Outline", nullptr, Color::RED()};

Outline::Outline(const OutlineType* type)
    : Thing{},
      memoryLocation(OutlineMemoryLocation::NORMAL),
      format(Markdown::Format::MINDFORGER), type(type)
{
    forgetThreshold = created = modified = read = 0;
    reads = revision = 0;
    importance = urgency = progress = 0;
    bytesize = 0;
    flags = 0;

    outlineDescriptorAsNote = new Note(&NOTE_4_OUTLINE_TYPE, this);
}

Outline::~Outline() {
    for(string* d:description) {
        delete d;
    }
    for(string* d:preamble) {
        delete d;
    }
    for(Note* note:notes) {
        delete note;
    }

    // description is shared between this outline and Note
    if(outlineDescriptorAsNote) {
        outlineDescriptorAsNote->clear();
        delete outlineDescriptorAsNote;
    }
}

void Outline::resetClonedOutline(Outline* o)
{
    o->setReads(1);
    o->setRevision(0);
    o->setModified();
    o->setCreated(o->modified);
    o->modified++;
    o->setRead(o->modified);
    o->setModifiedPretty();
    o->completeProperties(o->getModified());
}

Outline::Outline(const Outline& o)
    : Thing{},
      memoryLocation(OutlineMemoryLocation::NORMAL), format(o.format), type(o.type)
{
    key.clear();

    // IMPROVE i18n
    name = "Copy of " + o.name;
    if(o.description.size()) {
        for(string* s:o.description) {
            description.push_back(new string(*s));
        }
    }
    if(o.preamble.size()) {
        for(string* s:o.preamble) {
            preamble.push_back(new string(*s));
        }
    }

    if(o.notes.size()) {
        Note* clone;
        for(Note* n:o.notes) {
            clone = new Note(*n);
            resetClonedNote(clone);
            notes.push_back(clone);
        }
    }

    // created/modified/... to be reset = o.created;
    resetClonedOutline(this);

     // reset threshold to avoid situations when all Ns are forgotten > user is confused
    forgetThreshold = 0;

    importance = o.importance;
    urgency = o.urgency;
    progress = o.progress;
    bytesize = o.bytesize;

    if(o.tags.size()) {
        tags.insert(tags.end(), o.tags.begin(), o.tags.end());
    }

    outlineDescriptorAsNote = nullptr;
}


void Outline::completeProperties(const time_t fileModificationTime)
{
    // Outline
    //  - invariants:
    //    read > modified > created
    //    reads >= writes

    if(!created) {
        if(modified) {
            created = modified;
        } else {
            created = modified = fileModificationTime;
        }
    } else {
        if(!modified) {
            if(fileModificationTime > created) {
                modified = fileModificationTime;
            } else {
                modified = created;
            }
        } else {
            if(created > modified) {
                created = modified;
            }
        }
    }

    if(!modified) {
        if(fileModificationTime > created) {
            modified = fileModificationTime;
        } else {
            modified = created;
        }
    } else {
        if(created > modified) {
            created = modified;
        }
    }

    if(!read) {
        read = modified;
    }
    if(read < modified) {
        read = modified;
    }

    if(!revision) {
        if(!reads) {
            revision = reads = 1;
        } else {
            revision = reads;
        }
    }

    if(!reads) {
        reads = revision;
    }

    if(revision > reads) {
        reads = revision;
    }

    if(name.empty()) {
        name.assign("Outline");
    }

    setModifiedPretty();

    // Notes
    if(notes.size()) {
        for(Note* n:notes) {
            n->completeProperties(modified);
            n->setModifiedPretty();
        }
    }
}

bool Outline::isVirgin() const
{
    if(notes.empty() &&
       !name.compare("Outline") &&
       importance==0 &&
       urgency==0 &&
       progress==0
    ) {
        return true;
    } else {
        return false;
    }
}


void Outline::notifyChange(Note* note)
{
    modified = datetimeNow();
    revision++;

    note->setModified(modified);
    note->setModifiedPretty(datetimeToPrettyHtml(modified));
    note->incRevision();
}

time_t Outline::getCreated() const
{
    return created;
}

void Outline::setCreated(time_t created)
{
    this->created = created;
}

void Outline::setImportance(int8_t importance)
{
    this->importance = importance;
}

int8_t Outline::getImportance() const
{
    return importance;
}

const string& Outline::getKey() const
{
    return key;
}

void Outline::setKey(const string key)
{
    this->key = key;
}

const Tag* Outline::getPrimaryTag() const
{
    if(tags.size()) {
        return tags[0];
    } else {
        return nullptr;
    }
}

const vector<const Tag*>& Outline::getTags() const
{
    return tags;
}

void Outline::setTags(const vector<const Tag*>* tags)
{
    this->tags.clear();
    if(tags) {
        for(const Tag* t:*tags) {
            addTag(t);
        }
    }
}

void Outline::setTag(const Tag* tag)
{
    tags.clear();
    tags.push_back(tag);
}

time_t Outline::getModified() const
{
    return modified;
}

void Outline::makeModified()
{
    setModified();
    setModifiedPretty();
    incRevision();
}

void Outline::setModified()
{
    this->modified = datetimeNow();
}

void Outline::setModified(time_t modified)
{
    this->modified = modified;
}

const string& Outline::getModifiedPretty() const
{
    return modifiedPretty;
}

void Outline::setModifiedPretty()
{
    this->modifiedPretty = datetimeToPrettyHtml(modified);
}

void Outline::setModifiedPretty(const string& modifiedPretty)
{
    this->modifiedPretty = modifiedPretty;
}

const vector<Note*>& Outline::getNotes() const
{
    return notes;
}

size_t Outline::getNotesCount() const
{
    return notes.size();
}

void Outline::setNotes(const vector<Note*>& notes)
{
    this->notes = notes;
}

int8_t Outline::getProgress() const
{
    return progress;
}

void Outline::setProgress(int8_t progress)
{
    this->progress = progress;
}

u_int32_t Outline::getRevision() const
{
    return revision;
}

void Outline::setRevision(u_int32_t revision)
{
    this->revision = revision;
}

void Outline::incRevision()
{
    revision++;
}

const OutlineType* Outline::getType() const
{
    // return by reference
    return type;
}

void Outline::setType(const OutlineType* type)
{
    this->type = type;
}

int8_t Outline::getUrgency() const
{
    return urgency;
}

void Outline::addTag(const Tag* tag)
{
    tags.push_back(tag);
}

void Outline::setUrgency(int8_t urgency)
{
    this->urgency = urgency;
}

u_int32_t m8r::Outline::getReads() const
{
    return reads;
}

time_t m8r::Outline::getRead() const
{
    return read;
}

void Outline::setRead(time_t read)
{
    this->read = read;
}

void Outline::setReads(u_int32_t reads)
{
    this->reads = reads;
}

OutlineMemoryLocation Outline::getMemoryLocation() const
{
    return memoryLocation;
}

void Outline::resetClonedNote(Note* n)
{
    n->setOutline(this);
    n->setReads(1);
    n->setRevision(0);
    n->setModified();
    n->setModified(n->getModified()+1);
    n->setRead(n->getModified());
    n->setModifiedPretty();
    n->completeProperties(n->getModified());
}

Note* Outline::cloneNote(const Note* clonedNote)
{
    int offset = getNoteOffset(clonedNote);
    if(offset != -1) {
        Note* newNote;

        vector<Note*> children{};
        getNoteChildren(clonedNote, &children);
        offset += 1+children.size();
        if(children.size()) {
            if((unsigned int)offset < notes.size()) {
                for(Note* n:children) {
                    newNote = new Note(*n);
                    resetClonedNote(newNote);
                    addNote(newNote, offset);
                }
            } else {
                for(Note* n:children) {
                    newNote = new Note(*n);
                    resetClonedNote(newNote);
                    newNote->setOutline(this);
                    notes.push_back(newNote);
                }
            }
        }

        newNote = new Note(*clonedNote);
        resetClonedNote(newNote);
        addNote(newNote, offset);

        return newNote;
    } else {
        return nullptr;
    }
}

void Outline::addNote(Note* note)
{
    note->setOutline(this);
    notes.push_back(note);
}

void Outline::addNote(Note* note, int offset)
{
    note->setOutline(this);
    if((unsigned int)offset > notes.size()-1) {
        notes.push_back(note);
    } else {
        notes.insert(notes.begin()+offset, note);
    }
}

void Outline::addNotes(std::vector<Note*>& notesToAdd, int offset)
{
    if(notesToAdd.size()) {
        makeModified();
        for(int i=notesToAdd.size()-1; i>=0; i--) {
            notesToAdd[i]->makeModified();
            addNote(notesToAdd[i], offset);
        }
    }
}

int Outline::getNoteOffset(const Note* note) const
{
    if(!notes.empty()) {
        if(notes.size()==1) {
            return 0;
        } else {
            // IMPROVE this is SLOW O(n) - consider keeping order of note within it as a field
            auto it = std::find(notes.begin(), notes.end(), note);
            if(it != notes.end()) {
                return std::distance(notes.begin(), it);
            }
        }
    }
    return -1;
}

void Outline::getNoteChildren(const Note* note, vector<Note*>* children, Outline::Patch* patch)
{
    if(note) {
        if(note == notes[notes.size()-1]) {
            if(patch) {
                patch->start=notes.size()-1;
                patch->count=0;
            }
            return;
        } else {
            // IMPROVE this is SLOW o(n) - consider keeping order of note within it as a field
            auto offset = std::find(notes.begin(), notes.end(), note);
            if(offset != notes.end()) {
                auto index = std::distance(notes.begin(), offset);
                for(unsigned int i=index+1; i<notes.size(); i++) {
                    if(notes[i]->getDepth() > note->getDepth()) {
                        if(children) {
                            children->push_back(notes[i]);
                        }
                    } else {
                        if(patch) {
                            patch->start=index;
                            patch->count=i-index;
                        }
                        return;
                    }
                }
                if(patch) {
                    patch->start=index;
                    patch->count=notes.size()-1-index;
                }
                return;
            } else {
                // note not in vector
                if(patch) {
                    patch->start=patch->count=0;
                }
                return;
            }
        }
    }
}

void Outline::getNotePathToRoot(const size_t offset, std::vector<int> parents)
{
    if(offset && offset<notes.size()) {
        int parentDepth = notes[offset]->getDepth()-1;
        for(int i=offset; i>=0; i--) {
            if(notes[offset]->getDepth() == parentDepth) {
                parents.insert(parents.begin(),offset);
                parentDepth--;
            }
        }
    }
}

void Outline::removeNote(Note* note, bool deallocate)
{
    if(note && notes.size()) {
        auto d = note->getDepth();
        for(size_t i=0; i<notes.size(); i++) {
            if(notes[i] == note) {
                auto begin = i++;
                while(i<notes.size() && notes[i]->getDepth()>d) {
                    if(deallocate) {
                        delete notes[i];
                    }
                    i++;
                }
                // because erase deletes [begin,end)
                notes.erase(notes.begin()+begin, notes.begin()+i);

                if(deallocate) {
                    delete note;
                }

                return;
            }
        }

        notes.erase(std::remove(notes.begin(), notes.end(), note), notes.end());
    }
}

int Outline::getOffsetOfAboveNoteSibling(Note* note, int& offset)
{
    offset = getNoteOffset(note);
    if(offset != Outline::NO_OFFSET) {
        if(offset) {
            for(int o=offset-1; o>=0; o--) {
                if(notes[o]->getDepth() == note->getDepth()) {
                    if(offset == o) {
                        return NO_SIBLING;
                    } else {
                        return o;
                    }
                }
                if(notes[o]->getDepth() < note->getDepth()) {
                    return NO_SIBLING;
                }
            }
        }
    }
    return NO_SIBLING;
}

int Outline::getOffsetOfBelowNoteSibling(Note* note, int& offset)
{
    offset = getNoteOffset(note);
    if(offset != Outline::NO_OFFSET) {
        if((unsigned int)offset < notes.size()-1) {
            for(unsigned int o=offset+1; o<notes.size(); o++) {
                if(notes[o]->getDepth() == note->getDepth()) {
                    if((unsigned int)offset == o) {
                        return NO_SIBLING;
                    } else {
                        return o;
                    }
                }
                if(notes[o]->getDepth() < note->getDepth()) {
                    return NO_SIBLING;
                }
            }
        }
    }
    return NO_SIBLING;
}

void Outline::promoteNote(Note* note, Outline::Patch* patch)
{
    if(note) {
        if(note->getDepth()) {
            vector<Note*> children{};
            getNoteChildren(note, &children, patch);
            note->promote();
            note->makeModified();
            for(Note* n:children) {
                n->promote();
                // IMPROVE consider whether children should be marked as modified or no n->makeModified();
            }
            makeModified();
            if(patch) {
                patch->diff = Outline::Patch::Diff::CHANGE;
            }
            return;
        }
    }
    if(patch) {
        patch->diff = Outline::Patch::Diff::NO;
    }
}

void Outline::demoteNote(Note* note, Outline::Patch* patch)
{
    if(note) {
        if(note->getDepth() < MAX_NOTE_DEPTH) {
            vector<Note*> children{};
            getNoteChildren(note, &children, patch);
            note->demote();
            note->makeModified();
            for(Note* n:children) {
                n->demote();
                // IMPROVE consider whether children should be marked as modified or no n->makeModified();
            }
            makeModified();
            if(patch) {
                patch->diff = Outline::Patch::Diff::CHANGE;
            }
            return;
        }
    }
    if(patch) {
        patch->diff = Outline::Patch::Diff::NO;
    }
}

// IMPROVE move to up and first are almost the same - introduce method that has sibling offset as parameter
void Outline::moveNoteToFirst(Note* note, Outline::Patch* patch)
{
    if(note) {
        int no, noteOffset = NO_OFFSET;

        // loop to find the first sibling
        int so, siblingOffset = NO_OFFSET;
        Note* n = note;
        while((so = getOffsetOfAboveNoteSibling(n, no)) != NO_SIBLING) {
            if(noteOffset == NO_OFFSET) noteOffset = no;
            siblingOffset = so;
            n = note->getOutline()->getNotes()[siblingOffset];
        }

        if(siblingOffset != NO_SIBLING) {
            vector<Note*> children{};
            getNoteChildren(note, &children);
            if(patch) {
                // upper tier to patch [sibling's offset, note's last child]
                patch->diff = Outline::Patch::Diff::MOVE;
                patch->start = siblingOffset;
                patch->count = noteOffset+children.size() - siblingOffset;
            }
            // modify outline: cut & insert
            if(children.size()) {
                notes.erase(notes.begin()+noteOffset, notes.begin()+noteOffset+children.size()+1);
                for(int c=children.size()-1; c>=0; c--) {
                    // bottom up insert
                    notes.insert(notes.begin()+siblingOffset, children[c]);
                }
            } else {
                notes.erase(notes.begin()+noteOffset);
            }
            notes.insert(notes.begin()+siblingOffset, note);
            return;
        } else {
            if(patch) {
                patch->diff = Outline::Patch::Diff::NO;
                return;
            }
        }
    } else {
        if(patch) {
            patch->diff = Outline::Patch::Diff::NO;
        }
    }
}

void Outline::moveNoteUp(Note* note, Outline::Patch* patch)
{
    if(note) {
        int noteOffset;
        int siblingOffset = getOffsetOfAboveNoteSibling(note, noteOffset);
        if(siblingOffset != NO_SIBLING) {
            vector<Note*> children{};
            getNoteChildren(note, &children);
            if(patch) {
                // upper tier to patch [sibling's offset, note's last child]
                patch->diff = Outline::Patch::Diff::MOVE;
                patch->start = siblingOffset;
                patch->count = noteOffset+children.size() - siblingOffset;
            }
            // modify outline: cut & insert
            if(children.size()) {
                notes.erase(notes.begin()+noteOffset, notes.begin()+noteOffset+children.size()+1);
                for(int c=children.size()-1; c>=0; c--) {
                    // bottom up insert
                    notes.insert(notes.begin()+siblingOffset, children[c]);
                }
            } else {
                notes.erase(notes.begin()+noteOffset);
            }
            notes.insert(notes.begin()+siblingOffset, note);
            return;
        } else {
            if(patch) {
                patch->diff = Outline::Patch::Diff::NO;
                return;
            }
        }
    } else {
        if(patch) {
            patch->diff = Outline::Patch::Diff::NO;
        }
    }
}

void Outline::moveNoteDown(Note* note, Outline::Patch* patch)
{
    if(note) {
        int noteOffset;
        int siblingOffset = getOffsetOfBelowNoteSibling(note, noteOffset);
        if(siblingOffset != NO_SIBLING) {
            Note* sibling = notes[siblingOffset];
            vector<Note*> siblingChildren{};
            getNoteChildren(sibling, &siblingChildren);
            if(patch) {
                // upper tier to patch [note's original offset,sibling's last child]
                patch->diff = Outline::Patch::Diff::MOVE;
                patch->start = noteOffset;
                patch->count = siblingOffset+siblingChildren.size() - noteOffset;
            }
            // modify outline: cut & insert (COPY moved Note below sibling, DELETE original Note)
            vector<Note*> children{};
            getNoteChildren(note, &children);
            int belowSiblingIndex = siblingOffset+siblingChildren.size()+1;
            notes.insert(notes.begin()+belowSiblingIndex, note);
            if(children.size()) {
                for(int c=children.size()-1; c>=0; c--) {
                    // bottom up insert
                    notes.insert(notes.begin()+belowSiblingIndex+1, children[c]);
                }
            }
            notes.erase(notes.begin()+noteOffset, notes.begin()+noteOffset+children.size()+1);
            return;
        } else {
            if(patch) {
                patch->diff = Outline::Patch::Diff::NO;
                return;
            }
        }
    } else {
        if(patch) {
            patch->diff = Outline::Patch::Diff::NO;
        }
    }
}

void Outline::moveNoteToLast(Note* note, Outline::Patch* patch)
{
    if(note) {
        int no, noteOffset = NO_OFFSET;

        // loop to find the first sibling
        int so, siblingOffset = NO_OFFSET;
        Note* n = note;
        while((so = getOffsetOfBelowNoteSibling(n, no)) != NO_SIBLING) {
            if(noteOffset == NO_OFFSET) noteOffset = no;
            siblingOffset = so;
            n = note->getOutline()->getNotes()[siblingOffset];
        }

        if(siblingOffset != NO_SIBLING) {
            Note* sibling = notes[siblingOffset];
            vector<Note*> siblingChildren{};
            getNoteChildren(sibling, &siblingChildren);
            if(patch) {
                // upper tier to patch [note's original offset,sibling's last child]
                patch->diff = Outline::Patch::Diff::MOVE;
                patch->start = noteOffset;
                patch->count = siblingOffset+siblingChildren.size() - noteOffset;
            }
            // modify outline: cut & insert (COPY moved Note below sibling, DELETE original Note)
            vector<Note*> children{};
            getNoteChildren(note, &children);
            int belowSiblingIndex = siblingOffset+siblingChildren.size()+1;
            notes.insert(notes.begin()+belowSiblingIndex, note);
            if(children.size()) {
                for(int c=children.size()-1; c>=0; c--) {
                    // bottom up insert
                    notes.insert(notes.begin()+belowSiblingIndex+1, children[c]);
                }
            }
            notes.erase(notes.begin()+noteOffset, notes.begin()+noteOffset+children.size()+1);
            return;
        } else {
            if(patch) {
                patch->diff = Outline::Patch::Diff::NO;
                return;
            }
        }
    } else {
        if(patch) {
            patch->diff = Outline::Patch::Diff::NO;
        }
    }
}

const vector<string*>& Outline::getPreamble() const
{
    return preamble;
}

string Outline::getPreambleAsString() const
{
    // IMPROVE cache narrowed preamble for performance
    string result{};
    if(preamble.size()) {
        for(string *s:preamble) {
            result.append(*s);
            // IMPROVE endl
            result.append("\n");
        }
    }
    return result;
}

void Outline::addPreambleLine(string *line)
{
    if(line) {
        preamble.push_back(line);
    }
}

void Outline::setPreamble(const vector<string*>& preamble)
{
    this->preamble = preamble;
}

const vector<string*>& Outline::getDescription() const
{
    return description;
}

string Outline::getDescriptionAsString() const
{
    // IMPROVE cache narrowed description for performance
    string result{};
    if(description.size()) {
        for(string *s:description) {
            result.append(*s);
            result.append("\n");
        }
    }
    return result;
}

void Outline::addDescriptionLine(string *line)
{
    if(line) {
        description.push_back(line);
    }
}

void Outline::setDescription(const vector<string*>& description)
{
    this->description = description;
}

void Outline::clearDescription()
{
    this->description.clear();
}

void Outline::setBytesize(unsigned int bytesize)
{
    this->bytesize = bytesize;
}

unsigned int Outline::getBytesize() const
{
    return bytesize;
}

void Outline::setMemoryLocation(OutlineMemoryLocation memoryLocation)
{
    this->memoryLocation = memoryLocation;
}

Note* Outline::getOutlineDescriptorAsNote()
{
    outlineDescriptorAsNote->setName(name);
    outlineDescriptorAsNote->setDescription(description);
    return outlineDescriptorAsNote;
}

bool Outline::isApiaryBlueprint()
{
    if(preamble.size() && preamble[0]->size()>7 && stringStartsWith(*preamble[0],"FORMAT:") ) {
        return true;
    } else {
        return false;
    }
}


} // m8r namespace
