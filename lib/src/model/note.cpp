/*
 note.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
    : ThingInTime{},
      outline(outline),
      type(type)
{
    depth = 0;
    created = modified = read = deadline = 0;
    reads = revision = 0;
    progress = 0;
    flags = 0;
    aiAaMatrixIndex = -1;
}

Note::Note(const Note& n)
    : ThingInTime{},
      outline(nullptr),
      type(n.type)
{
    name = n.name;
    autolinkName();
    if(n.description.size()) {
        for(string* s:n.description) {
            description.push_back(new string(*s));
        }
    }

    depth = n.depth;
    created = n.created;
    modified = n.modified;
    read = n.read;
    deadline = n.deadline;
    reads = n.reads;
    revision = n.revision;
    progress = n.progress;
    // share old N's similarity assessment
    aiAaMatrixIndex = n.aiAaMatrixIndex;

    if(n.tags.size()) {
        tags.insert(tags.end(), n.tags.begin(), n.tags.end());
    }

    flags = n.flags;
}

Note::~Note()
{
    for(string* d:description) {
        delete d;
    }
    for(Link* l:links) {
        delete l;
    }
}

string Note::getMangledName() const
{
    string result = name;
    if(result.size()) {
        // non-alpha or non-num to -
        for(size_t i=0; i<result.size(); i++) {
            if(!isalnum(result[i])) {
                result[i] = '-';
            }
        }
        // remove leading and trailing -
        while(result.find("-") == 0) {
            result.erase(0, 1);
        }
        if(result.size()) {
            size_t len = result.size();
            while(result.rfind("-") == --len) {
                result.erase(len, len + 1);
            }
        }
        if(result.size()) {
            // to lower case
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        }
    }
    return result;
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

void Note::makeModified()
{
    setModified();
    setModifiedPretty();
    incRevision();

    if(outline) outline->makeModified();
}

void Note::setModified()
{
    ThingInTime::setModified();
}

void Note::setModified(time_t modified)
{
    ThingInTime::setModified(modified);

    setModifiedPretty();
}

const string& Note::getModifiedPretty() const
{
    return modifiedPretty;
}

void Note::setModifiedPretty()
{
    this->modifiedPretty = datetimeToPrettyHtml(modified);
}

void Note::setModifiedPretty(const string& modifiedPretty)
{
    this->modifiedPretty = modifiedPretty;
}

const string& Note::getReadPretty() const
{
    return readPretty;
}

void Note::setReadPretty()
{
    this->readPretty = datetimeToPrettyHtml(read);
}

void Note::setReadPretty(const string& readPretty)
{
    this->readPretty = readPretty;
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
    setReadPretty();
}

void Note::makeRead()
{
    setRead(datetimeNow());
    incReads();
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

void Note::incRevision() {
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

const vector<const Tag*>* Note::getTags() const
{
    return &tags;
}

void Note::addTag(const Tag* tag)
{
    if(tag && !this->hasTag(tag)) {
        this->tags.push_back(tag);
    }
}

void Note::setTag(const Tag* tag)
{
    if(tag) {
        tags.clear();
        addTag(tag);
    }
}

void Note::setTags(const vector<const Tag*>* tags)
{
    this->tags.clear();
    if(tags) {
        for(const Tag* t:*tags) {
            addTag(t);
        }
    }
}

void Note::addName(const string& s) {
    name += s;
    autolinkName();
}

const NoteType* Note::getType() const
{
    return type;
}

void Note::clear()
{
    description.clear();
}

const vector<string*>& Note::getDescription() const
{
    return description;
}

string Note::getDescriptionAsString(const std::string& separator) const
{    
    // IMPROVE cache narrowed description for performance & return it by reference
    string result{};
    if(description.size()) {
        for(string *s:description) {
            result += *s;
            result += separator;
        }
    }
    return result;
}

void Note::setDescription(const vector<string*>& description)
{
    this->description = description;
}

void Note::moveDescription(std::vector<std::string*>& target)
{
    if(description.size()) {
        // IMPROVE find a more efficient method - perhaps an algorithm function
        for(auto& s:description) {
            target.push_back(s);
        }
        description.clear();
    }
}

void Note::clearDescription()
{
    this->description.clear();
}

void Note::addDescription(const vector<string*>& d)
{
    // IMPROVE why not description.push_back(d);
    description.insert(description.end(),d.begin(),d.end());
}

Outline* Note::getOutline() const
{
    return outline;
}

void Note::setOutline(Outline* outline)
{
    this->outline = outline;
}

string& Note::getOutlineKey() const
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
    }

    if(!read) {
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

    if(description.empty()) {
        description.push_back(new string{""});
    }

    checkAndFixProperties();
    setModifiedPretty();
}

void Note::checkAndFixProperties()
{
    if(revision > reads) {
        reads = revision;
    }
    if(modified > read) {
        read = modified;
    }
    if(created > modified) {
        created = modified;
    }

    if(name.empty()) {
        name.assign("Note");
        autolinkName();
    }

    MF_ASSERT_FUTURE_TIMESTAMPS(created, read, modified, outline->getKey() << " # " << name, name);
}

string& Note::getKey()
{
    key.clear();
    key.append(outline->getKey());
    key.append("#");
    key.append(getMangledName());

    return key;
}

void Note::addLink(Link* link)
{
    if(link) {
        links.push_back(link);
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

void Note::makeDirty()
{
    if(outline) outline->makeDirty();
}

bool Note::isReadOnly() const {
    return outline->isReadOnly();
}

} // m8r namespace
