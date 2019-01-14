/*
 markdown_section_metadata.cpp     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "markdown_section_metadata.h"

namespace m8r {

MarkdownSectionMetadata::MarkdownSectionMetadata()
{
    created = read = modified = 0;
    revision = reads = 0;
}

time_t MarkdownSectionMetadata::getCreated() const
{
    return created;
}

void MarkdownSectionMetadata::setCreated(time_t created)
{
    this->created = created;
}

time_t MarkdownSectionMetadata::getModified() const
{
    return modified;
}

void MarkdownSectionMetadata::setModified(time_t modified)
{
    this->modified = modified;
}

time_t MarkdownSectionMetadata::getRead() const
{
    return read;
}

void MarkdownSectionMetadata::setRead(time_t read)
{
    this->read = read;
}

u_int32_t MarkdownSectionMetadata::getReads() const
{
    return reads;
}

void MarkdownSectionMetadata::setReads(u_int32_t reads)
{
    this->reads = reads;
}

u_int32_t MarkdownSectionMetadata::getRevision() const
{
    return revision;
}

void MarkdownSectionMetadata::setRevision(u_int32_t revision)
{
    this->revision = revision;
}

MarkdownSectionMetadata::~MarkdownSectionMetadata()
{
}

} // m8r namespace
