/*
 markdown_document_representation.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "markdown_document_representation.h"

using namespace std;

namespace m8r {

MarkdownDocumentRepresentation::MarkdownDocumentRepresentation(Ontology& ontology)
    : ontology{ontology}
{
}

MarkdownDocumentRepresentation::~MarkdownDocumentRepresentation()
{
}

Outline* MarkdownDocumentRepresentation::to(
    const string& documentPath,
    const string& outlinePath
) {
    Outline* o = new Outline{ontology.findOrCreateOutlineType(OutlineType::KeyPdf())};

    o->setKey(outlinePath);

    string outlineDir{};
    string outlineFilename{documentPath};
    if(documentPath.size()) {
        pathToDirectoryAndFile(documentPath, outlineDir, outlineFilename);
    }
    if(outlineFilename.size()) {
        o->setName(outlineFilename);
    } else {
        o->setName(documentPath);
    }

    o->addTag(ontology.findOrCreateTag("pdf"));
    o->addTag(ontology.findOrCreateTag("library-document"));

    o->addDescriptionLine(
        new string{
            "This is a notebook for the document: "
            "[" + documentPath + "](" + documentPath + ")"});
    o->addDescriptionLine(new string{""});
    o->addDescriptionLine(new string{"---"});
    o->addDescriptionLine(new string{""});
    o->addDescriptionLine(new string{
        "This notebook represents above document in MindForger. This Notebook "
        "was created automatically on indexation of a library "
        "and may contain document text (if available) to enable full-text "
        "search, associations and content mining."});
    o->addDescriptionLine(new string{""});
    o->addDescriptionLine(new string{
        "Add notes with your remarks, thoughts and ideas to this notebook."
        }
    );
    o->addDescriptionLine(new string{""});
    o->addDescriptionLine(new string{
        "Please **DO NOT EDIT** the first row of this description with "
        "the document path - notebook must stay interlinked with the document."
        }
    );
    o->addDescriptionLine(new string{""});

    // set O modification time identical to the document
    o->setCreated(fileModificationTime(&documentPath));
    o->setModified(o->getCreated());
    o->setModifiedPretty();

    o->checkAndFixProperties();

    return o;
}

} // m8r namespace
