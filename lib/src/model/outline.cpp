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

const NoteType Outline::NOTE_4_OUTLINE_TYPE{"Outline", Color::RED()};

Outline::Outline(const OutlineType* type)
    : memoryLocation(OutlineMemoryLocation::NORMAL), type(type)
{
    created = modified = read = 0;
    reads = revision = 0;
    importance = urgency = progress = 0;
    bytesize = 0;

    outlineDescriptorAsNote = new Note(&NOTE_4_OUTLINE_TYPE, this);
}

void Outline::completeProperties(const time_t fileModificationTime)
{
    // Outline
    //  - invariants:
    //    read > modified > created
    //    reads > writes

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

    if(title.empty()) {
        title.assign("Outline");
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

void Outline::setTags(const vector<const Tag*>& tags)
{
    this->tags = tags;
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

void Outline::setModified(void)
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

void Outline::setModifiedPretty(void)
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

Outline::~Outline() {
    for(string* d:description) {
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

void Outline::addNote(Note* note)
{
    note->setOutline(this);
    notes.push_back(note);
}

void Outline::addNote(Note* note, int offset)
{
    note->setOutline(this);
    notes.insert(notes.begin()+offset, note);
}

void Outline::forgetNote(Note* note)
{
    if(note && notes.size()) {
        auto d = note->getDepth();
        for(size_t i=0; i<notes.size(); i++) {
            if(notes[i] == note) {
                auto begin = i++;
                while(i<notes.size() && notes[i]->getDepth()>d) {
                    delete notes[i];
                    i++;
                }
                // because erase deletes [begin,end)
                notes.erase(notes.begin()+begin, notes.begin()+i);

                delete note;

                return;
            }
        }

        notes.erase(std::remove(notes.begin(), notes.end(), note), notes.end());
    }
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

const string& Outline::getTitle() const
{
    return title;
}

void Outline::setBytesize(unsigned int bytesize)
{
    this->bytesize = bytesize;
}

unsigned int Outline::getBytesize() const
{
    return bytesize;
}

void Outline::setTitle(const string& title)
{
    this->title = title;
}

void Outline::setMemoryLocation(OutlineMemoryLocation memoryLocation)
{
    this->memoryLocation = memoryLocation;
}

Note* Outline::getOutlineDescriptorAsNote(void)
{
    outlineDescriptorAsNote->setTitle(title);
    outlineDescriptorAsNote->setDescription(description);
    return outlineDescriptorAsNote;
}

} /* namespace m8r */
