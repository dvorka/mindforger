/*
 markdown_outline_metadata.cpp     MindForger thinking notebook

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
#include "markdown_outline_metadata.h"

#include "../../mind/ontology/ontology.h"

namespace m8r {

MarkdownOutlineMetadata::MarkdownOutlineMetadata(const Ontology& ontology)
    : MarkdownSectionMetadata()
{
    importance = urgency = progress = 0;
    outlineType = ontology.getDefaultOutlineType();
}

int8_t MarkdownOutlineMetadata::getImportance() const
{
    return importance;
}

void MarkdownOutlineMetadata::setImportance(int8_t importance)
{
    this->importance = importance;
}

const OutlineType* MarkdownOutlineMetadata::getOutlineType() const
{
    return outlineType;
}

void MarkdownOutlineMetadata::setOutlineType(const OutlineType* outlineType)
{
    this->outlineType = outlineType;
}

int8_t MarkdownOutlineMetadata::getProgress() const
{
    return progress;
}

void MarkdownOutlineMetadata::setProgress(int8_t progress)
{
    this->progress = progress;
}

int8_t MarkdownOutlineMetadata::getUrgency() const
{
    return urgency;
}

void MarkdownOutlineMetadata::setUrgency(int8_t urgency)
{
    this->urgency = urgency;
}

MarkdownOutlineMetadata::~MarkdownOutlineMetadata()
{
}

} // m8r namespace
