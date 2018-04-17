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
#include "../../../../deps/discount/mkdio.h"
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

void HtmlOutlineRepresentation::header(string& html, string* basePath)
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

        if(basePath) {
            html.append("<base href=\"file://");
            html.append(*basePath);
            html.append("/\">");
#ifdef DO_M8F_DEBUG
        html.append("\n");
#endif
        }

        // SCROLLING: scrolling bridge
        html.append("<script type=\"text/javascript\">window.onscroll = function() { synchronizer.webViewScrolled(); }; </script>");
#ifdef DO_M8F_DEBUG
        html.append("\n");
#endif

        // MATH: via MathJax.js
        // - link to the latest version: http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML
        // - check newMathJax variable in the script above e.g. https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.1/MathJax.js
        if(lastMfOptions&Configuration::MdToHtmlOption::MathSupport) {
            // set inline flag
            if(lastMfOptions&Configuration::MdToHtmlOption::MathInlineSupport) {
                html.append("<script type=\"text/x-mathjax-config\">MathJax.Hub.Config({tex2jax: {inlineMath: [['$','$'], ['\\\\(','\\\\)']]}});</script>");
#ifdef DO_M8F_DEBUG
        html.append("\n");
#endif
            }
            if(config.isUiAllowOnlineJavascriptLibs()) {
                html.append("<script type=\"text/javascript\" src=\"https://cdn.mathjax.org/mathjax/2.7-latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML\"></script>");
            } else {
                // qrc:resource to avoid loading from Internet
                html.append("<script type=\"text/javascript\" src=\"qrc:/js/mathjax.js\"></script>");
            }
#ifdef DO_M8F_DEBUG
        html.append("\n");
#endif
        }

        // DIAGRAMS: mermaid.js
        // - download from: https://unpkg.com/mermaid@7.1.0/dist/
        // - try https://mermaidjs.github.io/scripts/mermaid.min.js
        // - live demo: https://mermaidjs.github.io/mermaid-live-editor
        if(lastMfOptions&Configuration::MdToHtmlOption::DiagramSupport) {
            if(config.isUiAllowOnlineJavascriptLibs()) {
                html.append("<script src=\"https://mermaidjs.github.io/scripts/mermaid.min.js\"></script>");
            } else {
                html.append("<script src=\"qrc:/js/mermaid.js\"></script>");
            }
#ifdef DO_M8F_DEBUG
        html.append("\n");
#endif
        }

        // SYNTAX HIGHLIGHTING: for source code via Highlight.js
        // - CME        
        if(lastMfOptions&Configuration::MdToHtmlOption::CodeHighlighting) {
            html.append("<link rel=\"stylesheet\" href=\"qrc:/html-css/highlight.css\">");
#ifdef DO_M8F_DEBUG
        html.append("\n");
#endif
            html.append("<script src=\"qrc:/js/highlight.js\"></script>");
#ifdef DO_M8F_DEBUG
        html.append("\n");
#endif
            html.append("<script>hljs.initHighlightingOnLoad();</script>");
#ifdef DO_M8F_DEBUG
        html.append("\n");
#endif
        }

        // THEME:
        html.append("<link rel=\"stylesheet\" href=\"");
        html.append(config.getUiHtmlCssPath());
        html.append("\">");
#ifdef DO_M8F_DEBUG
    html.append("\n");
#endif

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

string* HtmlOutlineRepresentation::to(const string* markdown, string* html, string* basePath)
{
    if(!config.isUiHtmlTheme()) {
        header(*html, basePath);
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
                        | MKD_FENCEDCODE // this enables ``` and ~~~
                        | MKD_LATEX // this helps to enable mathjax
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
        header(*html, basePath);
        html->append(body);
        footer(*html);
    }

    // debug generated HTML
    MF_DEBUG("=== BEGIN HTML ===" << endl << *html << endl << "=== END HTML ===" << endl);

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
    string path, file;
    pathToDirectoryAndFile(note->getOutlineKey(), path, file);
    to(markdown, html, &path);
    delete markdown;
    return html;
}

} // m8r namespace
