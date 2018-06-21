/*
 markdown_ast_node.cpp     MindForger thinking notebook

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
#include "markdown_ast_node.h"

using namespace std;

namespace m8r {

/*
 * MarkdownAstNode
 */

MarkdownAstNode::MarkdownAstNode(const MarkdownAstNodeType type)
{
    this->type = type;
    this->text = nullptr;
}

MarkdownAstNodeType MarkdownAstNode::getType() const
{
    return type;
}

void MarkdownAstNode::setType(MarkdownAstNodeType type)
{
    this->type = type;
}

MarkdownAstNode::~MarkdownAstNode()
{
    if(text!=nullptr) {
        delete text;
        text = nullptr;
    }
}

/*
 * MarkdownAstNodeSection
 */

MarkdownAstNodeSection::MarkdownAstNodeSection()
    : MarkdownAstNode{MarkdownAstNodeType::SECTION}
{
    depth = 0;
    flags = 0;
    text = nullptr;
    body = new vector<string*>{};
}

MarkdownAstNodeSection::MarkdownAstNodeSection(string *text)
    : MarkdownAstNodeSection{}
{
    this->text = text;
}

u_int16_t MarkdownAstNodeSection::getDepth() const
{
    return depth;
}

void MarkdownAstNodeSection::setDepth(u_int16_t depth)
{
    this->depth = depth;
}

void MarkdownAstNodeSection::setBody(vector<string*>* body)
{
    if(this->body!=nullptr) {
        delete this->body;
        this->body=nullptr;
    }
    this->body = body;
}

MarkdownAstNodeSection::~MarkdownAstNodeSection()
{
    if(body!=nullptr) {
        for(string*& b:*body) {
            if(b!=nullptr) {
                delete b;
            }
        }
        delete body;
        body = nullptr;
    }
}

MarkdownAstSectionMetadata& MarkdownAstNodeSection::getMetadata()
{
    return metadata;
}

time_t MarkdownAstSectionMetadata::getCreated() const
{
    return created;
}

void MarkdownAstSectionMetadata::setCreated(time_t created)
{
    this->created = created;
}

int8_t MarkdownAstSectionMetadata::getImportance() const
{
    return importance;
}

void MarkdownAstSectionMetadata::setImportance(int8_t importance)
{
    this->importance = importance;
}

time_t MarkdownAstSectionMetadata::getModified() const
{
    return modified;
}

void MarkdownAstSectionMetadata::setModified(time_t modified)
{
    this->modified = modified;
}

int8_t MarkdownAstSectionMetadata::getProgress() const
{
    return progress;
}

void MarkdownAstSectionMetadata::setProgress(int8_t progress)
{
    this->progress = progress;
}

time_t MarkdownAstSectionMetadata::getRead() const
{
    return read;
}

void MarkdownAstSectionMetadata::setRead(time_t read)
{
    this->read = read;
}

u_int32_t MarkdownAstSectionMetadata::getReads() const
{
    return reads;
}

void MarkdownAstSectionMetadata::setReads(u_int32_t reads)
{
    this->reads = reads;
}

u_int32_t MarkdownAstSectionMetadata::getRevision() const
{
    return revision;
}

void MarkdownAstSectionMetadata::setRevision(u_int32_t revision)
{
    this->revision = revision;
}

const string* MarkdownAstSectionMetadata::getType() const
{
    return type;
}

void MarkdownAstSectionMetadata::setType(const string* type)
{
    this->type = type;
}

int8_t MarkdownAstSectionMetadata::getUrgency() const
{
    return urgency;
}

MarkdownAstSectionMetadata::MarkdownAstSectionMetadata()
{
    deadline = created = read = modified = 0;
    importance = urgency = progress = 0;
    revision = reads = 0;
    type = nullptr;
}

MarkdownAstSectionMetadata::~MarkdownAstSectionMetadata()
{
    if(tags.size()) {
        for(string* s:tags) delete s;
    }
    if(type) {
        delete type;
    }
}

const vector<string*>& MarkdownAstSectionMetadata::getTags() const
{
    return tags;
}

void MarkdownAstSectionMetadata::setTags(vector<string*>* tags)
{
    if(tags && tags->size()) {
        // IMPROVE std::move()
        for(string* t:*tags) {
            this->tags.push_back(std::move(t));
        }
    }
}

void MarkdownAstSectionMetadata::setUrgency(int8_t urgency)
{
    this->urgency = urgency;
}

const string* MarkdownAstSectionMetadata::getPrimaryTag() const
{
    if(tags.size()) {
        return tags[0];
    } else {
        return nullptr;
    }
}

TimeScope& MarkdownAstSectionMetadata::getTimeScope()
{
    return timeScope;
}

void MarkdownAstSectionMetadata::setTimeScope(const TimeScope& timeScope)
{
    this->timeScope = timeScope;
}

time_t MarkdownAstSectionMetadata::getDeadline() const
{
    return deadline;
}

void MarkdownAstSectionMetadata::setDeadline(time_t deadline)
{
    this->deadline = deadline;
}

const std::vector<Link*>& MarkdownAstSectionMetadata::getLinks() const
{
    return links;
}

void MarkdownAstSectionMetadata::setLinks(std::vector<Link*>* links)
{
    if(links) {
        this->links = std::move(*links);
    } else {
        this->links.clear();
    }
}

} // m8r namespace

