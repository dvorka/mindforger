/*
 note.cpp     MindForger thinking notebook

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
#include "note.h"

using namespace std;

namespace m8r {

Note::Note(const NoteType* type, Outline* outline)
    : outline(outline), type(type)
{
    id = 0;
    order = depth = 0;
    created = modified = read = deadline = 0;
    reads = revision = 0;
    progress = 0;
}

const vector<Attachment*>& Note::getAttachments() const
{
    return attachments;
}

void Note::setAttachments(const vector<Attachment*>& attachments)
{
    this->attachments = attachments;
}

time_t Note::getCreated() const
{
    return created;
}

void Note::setCreated(time_t created)
{
    this->created = created;
}

time_t Note::getDeadline() const
{
    return deadline;
}

void Note::setDeadline(time_t deadline)
{
    this->deadline = deadline;
}

u_int16_t Note::getDepth() const
{
    return depth;
}

void Note::setDepth(u_int16_t depth)
{
    this->depth = depth;
}

time_t Note::getModified() const
{
    return modified;
}

void Note::setModified(void)
{
    this->modified = datetimeNow();
}

void Note::setModified(time_t modified)
{
    this->modified = modified;
    setModifiedPretty();
}

const string& Note::getModifiedPretty() const
{
    return modifiedPretty;
}

void Note::setModifiedPretty(void)
{
    this->modifiedPretty = datetimeToPrettyHtml(modified);
}

void Note::setModifiedPretty(const string& modifiedPretty)
{
    this->modifiedPretty = modifiedPretty;
}

u_int16_t Note::getOrder() const
{
    return order;
}

void Note::setOrder(u_int16_t order)
{
    this->order = order;
}

u_int8_t Note::getProgress() const
{
    return progress;
}

void Note::setProgress(u_int8_t progress)
{
    this->progress = progress;
}

time_t Note::getRead() const
{
    return read;
}

void Note::setRead(time_t read)
{
    this->read = read;
}

u_int32_t Note::getReads() const
{
    return reads;
}

void Note::setReads(u_int32_t reads)
{
    this->reads = reads;
}

u_int32_t Note::getRevision() const
{
    return revision;
}

void Note::setRevision(u_int32_t revision)
{
    this->revision = revision;
}

void Note::incRevision(void) {
    revision++;
}

const Tag* Note::getPrimaryTag() const
{
    if(tags.size()) {
        return tags[0];
    } else {
        return nullptr;
    }
}

const vector<const Tag*>& Note::getTags() const
{
    return tags;
}

void Note::addTag(const Tag* tag)
{
    if(tag) {
        tags.push_back(tag);
    }
}

void Note::setTag(const Tag* tag)
{
    if(tag) {
        tags.clear();
        addTag(tag);
    }
}

void Note::setTags(const vector<const Tag*>& tags)
{
    this->tags = tags;
}

const string& Note::getTitle() const
{
    return title;
}

void Note::addTitle(const string& s) {
    title.append(s);
}

void Note::setTitle(const string& title)
{
    this->title = title;
}

const NoteType* Note::getType() const
{
    return type;
}

void Note::clear()
{
    description.clear();
    attachments.clear();
}

Note::~Note()
{
    for(string* d:description) {
        delete d;
    }
    for(Attachment* a:attachments) {
        delete a;
    }
}

const vector<string*>& Note::getDescription() const
{
    return description;
}

void Note::setDescription(const vector<string*>& description)
{
    this->description = description;
}

void Note::addDescription(const vector<string*>& d)
{
    // IMPROVE why not description.push_back(d);
    description.insert(description.end(),d.begin(),d.end());
}

Outline*& Note::getOutline()
{
    return outline;
}

void Note::setOutline(Outline* outline)
{
    this->outline = outline;
}

const string& Note::getOutlineKey(void) const
{
    if(outline) {
        return outline->getKey();
    } else {
        throw MindForgerException("Note's Outline not set!");
    }
}

void Note::addDescriptionLine(string *line)
{
    if(line) {
        description.push_back(line);
    }
}

void Note::setType(const NoteType* type)
{
    this->type = type;
}

void Note::completeProperties(const time_t outlineModificationTime)
{
    // Outline
    //  - invariants:
    //    read > modified > created
    //    reads > writes

    if(!created) {
        if(modified) {
            created = modified;
        } else {
            created = modified = outlineModificationTime;
        }
    } else {
        if(!modified) {
            if(outlineModificationTime > created) {
                modified = outlineModificationTime;
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
        if(outlineModificationTime > created) {
            modified = outlineModificationTime;
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

    setModifiedPretty();

    if(title.empty()) {
        title.assign("Note");
    }
    if(description.empty()) {
        description.push_back(new string{"..."});
    }
}

void Note::demote()
{
    depth++;
}

void Note::promote()
{
    if(depth) depth--;
}

} /* namespace m8r */
