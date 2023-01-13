/*
 markdown_document_representation.h     MindForger thinking notebook

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
#ifndef M8R_MARKDOWN_DOCUMENT_REPRESENTATION_H
#define M8R_MARKDOWN_DOCUMENT_REPRESENTATION_H

#include <string>
#include <sstream>

#include "../../mind/ontology/ontology.h"
#include "../../model/outline.h"

namespace m8r {

class MarkdownDocumentRepresentation
{
private:
    Ontology& ontology;

public:
    explicit MarkdownDocumentRepresentation(Ontology& ontology);
    MarkdownDocumentRepresentation(const MarkdownDocumentRepresentation&) = delete;
    MarkdownDocumentRepresentation(const MarkdownDocumentRepresentation&&) = delete;
    MarkdownDocumentRepresentation &operator=(const MarkdownDocumentRepresentation&) = delete;
    MarkdownDocumentRepresentation &operator=(const MarkdownDocumentRepresentation&&) = delete;
    ~MarkdownDocumentRepresentation();

    Outline* to(
        const std::string& documentPath,
        const std::string& filePath
    );
};

}
#endif // M8R_MARKDOWN_DOCUMENT_REPRESENTATION_H
