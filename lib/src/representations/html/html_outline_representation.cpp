/*
 html_outline_representation.cpp     MindForger thinking notebook

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
#include "html_outline_representation.h"

extern "C" {
#include <mkdio.h>
}

using namespace std;

namespace m8r {

HtmlOutlineRepresentation::HtmlOutlineRepresentation(Ontology& ontology)
    : config(Configuration::getInstance()), markdownRepresentation(ontology)
{
    lastMfOptions = discountOptions = 0;
}

HtmlOutlineRepresentation::~HtmlOutlineRepresentation()
{
}

std::string* HtmlOutlineRepresentation::to(const std::string* markdown, std::string* html)
{

    MMIOT* doc = nullptr;
    if(markdown->size() > 0) {
        // ensure MD ends with new line, otherwise there would be missing characters in output HTML
        if(markdown->at(markdown->size()-1) != '\n') {
            markdown += '\n';
        }

        // options
        unsigned int mfOptions = config.getMd2HtmlOptions();
        if(mfOptions!=lastMfOptions) {
            lastMfOptions = mfOptions;
            discountOptions
                = MKD_TOC
                    | MKD_NOSTYLE
                    | (mfOptions&Configuration::MdToHtmlOption::AutolinkOption?MKD_AUTOLINK:0)
                    | (mfOptions&Configuration::MdToHtmlOption::NoStrikethroughOption?MKD_NOSTRIKETHROUGH:0)
                    | (mfOptions&Configuration::MdToHtmlOption::NoAlphaListOption?MKD_NOALPHALIST:0)
                    | (mfOptions&Configuration::MdToHtmlOption::NoDefinitionListOption?MKD_NODLIST:0)
                    | (mfOptions&Configuration::MdToHtmlOption::NoSmartypantsOption?MKD_NOPANTS:0)
                    | (mfOptions&Configuration::MdToHtmlOption::ExtraFootnoteOption?MKD_EXTRA_FOOTNOTE:0)
                    | (mfOptions&Configuration::MdToHtmlOption::NoSuperscriptOption?MKD_NOSUPERSCRIPT:0);
        }

        // compile to AST - a tree of paragraph
        doc = mkd_string(markdown->c_str(), markdown->size(), discountOptions);
        mkd_compile(doc, discountOptions);
    }

    // TODO check if out is leak OR reused array
    char* out{};
    // compile AST to HTML string
    mkd_document(doc, &out);
    html->assign(out);

    return html;
}

string* HtmlOutlineRepresentation::to(const Outline* outline, string* html)
{
    // IMPROVE markdown can be processed by Mind to be enriched with various links and relationships
    string* markdown = markdownRepresentation.to(outline);
    return to(markdown, html);
}

string* HtmlOutlineRepresentation::toHeader(const Outline* outline, string* html)
{
    string* markdown = markdownRepresentation.toHeader(outline);
    return to(markdown, html);
}

string* HtmlOutlineRepresentation::to(const Note* note, string* html)
{
    string* markdown = markdownRepresentation.to(note);
    return to(markdown, html);
}

} // m8r namespace
