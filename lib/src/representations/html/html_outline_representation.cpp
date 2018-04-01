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
// IMPROVE this expects mkdio.h to be installed in the system - should I rather link Git submodule mkdio.h?
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

void HtmlOutlineRepresentation::header(string& html)
{
    if(!config.isUiHtmlTheme()) {
        // IMPROVE possibly dark/light variant might be needed (check theme name)
        html.assign("<html><body style='color: ");
        if(config.getUiThemeName().find(UI_THEME_LIGHT) != string::npos) {
            html.append("#000>");
        } else {
            html.append("#FFF");
        }
        html.append("'><pre>");
    } else {
        html.assign("<html><head>");
        html.append("<script type=\"text/javascript\">window.onscroll = function() { synchronizer.webViewScrolled(); }; </script>\n");

        // math via MathJax.js
        if(lastMfOptions&Configuration::MdToHtmlOption::MathSupport) {
            // set inline flag
            if(lastMfOptions&Configuration::MdToHtmlOption::MathInlineSupport) {
                html.append("<script type=\"text/x-mathjax-config\">MathJax.Hub.Config({tex2jax: {inlineMath: [['$','$'], ['\\\\(','\\\\)']]}});</script>");
            }
            // TODO make this qrc:resource
            html.append("<script type=\"text/javascript\" src=\"http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML\"></script>\n");
        }

        // diagrams via mermaid.js
        if(lastMfOptions&Configuration::MdToHtmlOption::DiagramSupport) {
            html.append("<link rel=\"stylesheet\" href=\"qrc:/scripts/mermaid/mermaid.css\">\n");
            html.append("<script src=\"qrc:/scripts/mermaid/mermaid.full.min.js\"></script>\n");
        }

        // source code highlighting via Highlight.js
        if(lastMfOptions&Configuration::MdToHtmlOption::CodeHighlighting) {
            html.append("<link rel=\"stylesheet\" href=\"");
            html.append(config.getUiHtmlCssPath());
            html.append("\">\n");
            html.append("<script src=\"qrc:/scripts/highlight.js/highlight.pack.js\"></script>\n");
            html.append("<script>hljs.initHighlightingOnLoad();</script>\n");
        }
        html.append("</head><body>");
    }
}

void HtmlOutlineRepresentation::footer(string& html)
{
    if(!config.isUiHtmlTheme()) {
        html.append("</pre>");
    }
    html.append("</body></html>");
}

string* HtmlOutlineRepresentation::to(const string* markdown, string* html)
{
    if(!config.isUiHtmlTheme()) {
        header(*html);
        html->append(*markdown);
        footer(*html);
    } else {
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
                      // IMPROVE I don't want TOC, should this be switched ON?
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
        char* body{};
        // compile AST to HTML string
        mkd_document(doc, &body);

        // assemble HTML
        html->clear();
        header(*html);
        html->append(body);
        footer(*html);
    }

    //MF_DEBUG("===" << *html << "=1==" << endl);

    return html;
}

string* HtmlOutlineRepresentation::to(const Outline* outline, string* html)
{
    // IMPROVE markdown can be processed by Mind to be enriched with various links and relationships
    string* markdown = markdownRepresentation.to(outline);    
    to(markdown, html);
    delete markdown;
    return html;
}

string* HtmlOutlineRepresentation::toHeader(const Outline* outline, string* html)
{
    string* markdown = markdownRepresentation.toHeader(outline);
    to(markdown, html);
    delete markdown;
    return html;
}

string* HtmlOutlineRepresentation::to(const Note* note, string* html)
{
    string* markdown = markdownRepresentation.to(note);
    to(markdown, html);
    delete markdown;
    return html;
}

} // m8r namespace
