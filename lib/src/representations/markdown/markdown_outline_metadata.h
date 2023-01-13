/*
 markdown_outline_metadata.h     MindForger thinking notebook

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
#ifndef M8R_MARKDOWN_OUTLINE_METADATA_H_
#define M8R_MARKDOWN_OUTLINE_METADATA_H_

#include "../../model/outline_type.h"
#include "../../mind/ontology/ontology.h"
#include "../../gear/datetime_utils.h"
#include "../../gear/string_utils.h"
#include "markdown_section_metadata.h"

namespace m8r {

class Ontology;

/**
 * @brief Markdown outline metadata.
 */
class MarkdownOutlineMetadata : public MarkdownSectionMetadata
{
private:
    std::vector<char*> tags;
    const OutlineType* outlineType;

    int8_t importance;
    int8_t urgency;
    int8_t progress;

public:
    explicit MarkdownOutlineMetadata(const Ontology& ontology);
    MarkdownOutlineMetadata(const MarkdownOutlineMetadata&) = delete;
    MarkdownOutlineMetadata(const MarkdownOutlineMetadata&&) = delete;
    MarkdownOutlineMetadata &operator=(const MarkdownOutlineMetadata&) = delete;
    MarkdownOutlineMetadata &operator=(const MarkdownOutlineMetadata&&) = delete;
    virtual ~MarkdownOutlineMetadata();

    int8_t getImportance() const;
    void setImportance(int8_t importance);
    const OutlineType* getOutlineType() const;
    void setOutlineType(const OutlineType* outlineType);
    int8_t getProgress() const;
    void setProgress(int8_t progress);
    int8_t getUrgency() const;
    void setUrgency(int8_t urgency);
};

} // m8r namespace

#endif /* M8R_MARKDOWN_OUTLINE_METADATA_H_ */
