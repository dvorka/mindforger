/*
 markdown_configuration_representation.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "markdown_configuration_representation.h"

namespace m8r {

using namespace std;

MarkdownConfigurationRepresentation::MarkdownConfigurationRepresentation()
{
    config = Configuration::getInstance();
}

MarkdownConfigurationRepresentation::~MarkdownConfigurationRepresentation()
{
}

void MarkdownConfigurationRepresentation::configuration(const File& file)
{
    Markdown md{&file.name};
    md.from();
    vector<MarkdownAstNodeSection*>* ast = md.moveAst();

    Outline* o = outline(ast);
    o->setFormat(md.getFormat());
    if(o) {
        o->setKey(*md.getFilePath());
        o->setBytesize(md.getFileSize());
        o->completeProperties(md.getModified());
        o->setModifiedPretty(datetimeToPrettyHtml(o->getModified()));
    }
    return o;
}

// AST to configuration transcoding aims to be as ROBUST as possible to handle
// eventual user typos and incorrect formatting
void MarkdownConfigurationRepresentation::configuration(vector<MarkdownAstNodeSection*>* ast)
{
    Outline *outline = new Outline{ontology.getDefaultOutlineType()};
    if(ast) {
        size_t off = 0;
        if(ast->size()) {
            MarkdownAstNodeSection* astNode = ast->at(off);

            // preamble
            if(astNode->isPreambleSection()) {
                vector<string*>* body = ast->at(off)->moveBody();
                if(body!=nullptr) {
                    // IMPROVE use body as is
                    for(string*& bodyItem:*body) {
                        outline->addPreambleLine(bodyItem);
                    }
                    delete body;
                }
                if(ast->size()>1) {
                    astNode = ast->at(++off);
                } else {
                    astNode = nullptr;
                }
            }

            if(astNode) {
                // O's section
                if(astNode->isPostDeclaredSection()) outline->setPostDeclaredSection();
                if(astNode->isTrailingHashesSection()) outline->setTrailingHashesSection();
                if(astNode->getText()!=nullptr) {
                    // IMPROVE pull pointer > do NOT copy
                    outline->setName(*(astNode->getText()));
                }
                const string* s = astNode->getMetadata().getType();
                if(s) {
                    const OutlineType* outlineType;
                    // IMPROVE consider string normalization to make parsing more robust
                    //std::transform(s.begin(), s.end(), s.begin(), ::tolower);
                    //s[0] = toupper(s[0])
                    if((outlineType=ontology.getOutlineTypes().get(*s))==nullptr) {
                        outlineType = ontology.getDefaultOutlineType();
                    }
                    outline->setType(outlineType);
                }
                outline->setCreated(astNode->getMetadata().getCreated());
                outline->setModified(astNode->getMetadata().getModified());
                outline->setRevision(astNode->getMetadata().getRevision());
                outline->setRead(astNode->getMetadata().getRead());
                outline->setReads(astNode->getMetadata().getReads());
                outline->setImportance(astNode->getMetadata().getImportance());
                outline->setUrgency(astNode->getMetadata().getUrgency());
                outline->setProgress(astNode->getMetadata().getProgress());

                if(astNode->getMetadata().getTags().size()) {
                    const Tag* t;
                    for(string* s:astNode->getMetadata().getTags()) {
                        t = ontology.findOrCreateTag(*s);
                        outline->addTag(t);
                    }
                }

                vector<string*>* body = ast->at(off)->moveBody();
                if(body!=nullptr) {
                    // IMPROVE use body as is
                    for(string*& bodyItem:*body) {
                        outline->addDescriptionLine(bodyItem);
                    }
                    delete body;
                }
            }
        }

        // Ns sections
        if(ast->size() > off+1) {
            note(ast, off+1, outline);
        }

        // delete AST
        for(MarkdownAstNodeSection* node:*ast) {
            if(node!=nullptr) {
                delete node;
            }
        }
        delete ast;
    }

    return outline;
}

string* MarkdownConfigurationRepresentation::to()
{
    string* md = new string{};
    md->reserve(AVG_OUTLINE_SIZE);
    return to(outline, md);
}

string* MarkdownConfigurationRepresentation::to(string* md)
{
    stringstream s{};
    // IMPROVE build more in compile time and less in runtime
    s <<
         "# MindForger Configuration" << endl <<
         endl <<
         "This is MindForger configuration file (Markdown hosted DSL)." << endl <<
         "See documentation for configuration options details." << endl <<
         endl <<
         "# Settings" << endl <<
         "Application settings:" << endl <<
         endl <<
         "* Theme: " << uiThemeName << endl <<
         "    * Examples: dark, light" << endl <<
         "* Time scope: " << "0y0m0d0h0m" << endl <<
         "    * Examples: 2y0m0d0h0m (recent 2 years), 0y3m15d0h0m (recent 3 months and 15 days)" << endl <<
         endl <<
         "# Repositories" << endl <<
         endl <<
         "If MindForger detects MindForger repository structure, then the directory is" << endl <<
         "threated as knowledge base, else it's used as a Markdown directory (e.g. metadata"  << endl <<
         "are not stored to Markdown files)." << endl <<
         endl <<
         "* Active repository: ~/mindforger-repository" << endl <<
         "* Repository: ~/mindforger-repository" << endl <<
         endl;
}

} // m8r namespace
