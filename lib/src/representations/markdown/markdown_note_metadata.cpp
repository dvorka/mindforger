/*
 markdown_note_metadata.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "markdown_note_metadata.h"

using namespace std;

namespace m8r {

MarkdownNoteMetadata::MarkdownNoteMetadata(Ontology& ontology)
    : MarkdownSectionMetadata()
{
    noteType = ontology.getDefaultNoteType();
}

const NoteType* MarkdownNoteMetadata::getNoteType() const
{
    return noteType;
}

void MarkdownNoteMetadata::setNoteType(const NoteType* noteType)
{
    this->noteType = noteType;
}

const vector<char*>& MarkdownNoteMetadata::getTags() const
{
    return tags;
}

void MarkdownNoteMetadata::setTags(const vector<char*>& tags)
{
    this->tags = tags;
}

MarkdownNoteMetadata::~MarkdownNoteMetadata()
{
}

} // m8r namespace

