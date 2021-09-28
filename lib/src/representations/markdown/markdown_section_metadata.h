/*
 markdown_section_metadata.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_MARKDOWN_SECTION_METADATA_H_
#define M8R_MARKDOWN_SECTION_METADATA_H_

#include <cstdlib>
#include <ctime>
#include "../../definitions.h"

namespace m8r {

class MarkdownSectionMetadata {
protected:
    time_t created;
    time_t modified;
    u_int32_t revision;
    time_t read;
    u_int32_t reads;

public:
    MarkdownSectionMetadata();
    MarkdownSectionMetadata(const MarkdownSectionMetadata&) = delete;
    MarkdownSectionMetadata(const MarkdownSectionMetadata&&) = delete;
    MarkdownSectionMetadata &operator=(const MarkdownSectionMetadata&) = delete;
    MarkdownSectionMetadata &operator=(const MarkdownSectionMetadata&&) = delete;
    virtual ~MarkdownSectionMetadata();

    time_t getCreated() const;
    void setCreated(time_t created);
    time_t getModified() const;
    void setModified(time_t modified);
    time_t getRead() const;
    void setRead(time_t read);
    u_int32_t getReads() const;
    void setReads(u_int32_t reads);
    u_int32_t getRevision() const;
    void setRevision(u_int32_t revision);
};

} // m8r namespace

#endif /* M8R_MARKDOWN_SECTION_METADATA_H_ */
