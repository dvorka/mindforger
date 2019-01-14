/*
 markdown_note_metadata.h     MindForger thinking notebook

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
#ifndef M8R_MARKDOWN_NOTE_METADATA_H_
#define M8R_MARKDOWN_NOTE_METADATA_H_

#include <vector>

#include "../../model/note_type.h"
#include "../../mind/ontology/ontology.h"
#include "markdown_section_metadata.h"

namespace m8r {

class Ontology;

/**
 * @brief Markdown Note metadata.
 */
class MarkdownNoteMetadata : public MarkdownSectionMetadata
{
private:
    std::vector<char*> tags;
    const NoteType* noteType;

public:
    explicit MarkdownNoteMetadata(Ontology& ontology);
    MarkdownNoteMetadata(const MarkdownNoteMetadata&) = delete;
    MarkdownNoteMetadata(const MarkdownNoteMetadata&&) = delete;
    MarkdownNoteMetadata &operator=(const MarkdownNoteMetadata&) = delete;
    MarkdownNoteMetadata &operator=(const MarkdownNoteMetadata&&) = delete;
    virtual ~MarkdownNoteMetadata();

    const NoteType* getNoteType() const;
    void setNoteType(const NoteType* noteType);
    const std::vector<char*>& getTags() const;
    void setTags(const std::vector<char*>& tags);
};

} // m8r namespace

#endif /* M8R_MARKDOWN_NOTE_METADATA_H_ */
