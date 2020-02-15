/*
 html_outline_representation.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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

namespace m8r {

using namespace std;

HtmlOutlineRepresentation::HtmlOutlineRepresentation(
        Ontology& ontology,
        RepresentationInterceptor* descriptionInterceptor)
    : config(Configuration::getInstance()),
      exportColors{},
      lf{exportColors},
      markdownRepresentation(ontology, descriptionInterceptor)
{
#if defined  MF_MD_2_HTML_CMARK
    markdownTranscoder = new CmarkGfmMarkdownTranscoder{};
#elif defined MF_MD_2_HTML_DISCOUNT
    markdownTranscoder = new DiscountMarkdownTranscoder{};
#else
    markdownTranscoder = nullptr;
#endif
}

HtmlOutlineRepresentation::~HtmlOutlineRepresentation()
{
    if(markdownTranscoder) {
        delete markdownTranscoder;
    }
}

HtmlOutlineRepresentation::HtmlOutlineRepresentation(
        Ontology& ontology,
        HtmlColorsRepresentation& lf,
        RepresentationInterceptor* descriptionInterceptor)
    : HtmlOutlineRepresentation{ontology, descriptionInterceptor}
{
    this->lf = lf;
}

void HtmlOutlineRepresentation::fgBgTextColorStyle(string& html)
{
    html += "color: ";
    html += lf.getHtmlTextColor();
    html += "; background-color: ";
    html += lf.getHtmlBackgroundColor();
    html += ";";
}

void HtmlOutlineRepresentation::outlineTypeToHtml(const OutlineType* outlineType, string& html)
{
    if(outlineType) {
        html += "<span style='color: ";
        html += outlineType->getColor().asHtml();
        html += "; font-style: italic;'> ";
        html += outlineType->getName();
        html += " </span>";
    }
}

void HtmlOutlineRepresentation::noteTypeToHtml(const NoteType* noteType,string& html)
{
    if(noteType) {
        html += "&nbsp;&nbsp;<span style='color: ";
        html += noteType->getColor().asHtml();
        html += "; font-style: italic;'> ";
        html += noteType->getName();

        // TODO parent-child relationship rendering
        html += " ";
        html += "&#9670;"; // composition
        //html += "&#9671;"; // aggregation
        //html += "&#9651;"; // isa

        html += " </span>";
    }
}

void HtmlOutlineRepresentation::tagsToHtml(const vector<const Tag*>* tags, string& html)
{
    //text += "&nbsp;&nbsp;<table cellspacing='0' border='0' style='color: #ffffff; background-color: #00cc00; font-weight: normal;'><tr><td>urgent</td></tr></table>";
    if(!tags->empty()) {
        for(const Tag* t:*tags) {
            if(!stringistring(string("none"), t->getName())) {
                html += "&nbsp;&nbsp;<span style='color: #ffffff; background-color: ";
                html += t->getColor().asHtml();
                html += "; font-weight: normal;'> ";
                html += t->getName();
                html += "&nbsp;</span>";
            }
        }
    }
}

void HtmlOutlineRepresentation::outlineMetadataToHtml(const Outline* outline, string& html)
{
    if(outline) {
        html += "<span style='color: ";
        html += outline->getType()->getColor().asHtml();
        html += "; font-style: italic;' title='Last read on ";
        html += datetimeToString(outline->getRead());
        html += ", last write on ";
        html += datetimeToString(outline->getModified());
        html += "'> with ";
        html += std::to_string(outline->getReads());
        html += " reads and ";
        html += std::to_string(outline->getRevision());
        html += " writes</span>";
    }
}

void HtmlOutlineRepresentation::header(string& html, string* basePath, bool standalone, int yScrollTo)
{
    if(!config.isUiHtmlTheme()) {
        // RAW THEME
        html.assign(
                    "<!DOCTYPE html>\n"
                    "<html>"
                    "<body style='");
        fgBgTextColorStyle(html);
        html += "'><pre>";
    } else {
        // LIGHT/DARK HTML THEME
        html.assign(
            "<!DOCTYPE html>\n"
            "<html>"
            "<head>");
#ifdef DO_MF_DEBUG
        html += "\n";
#endif

        // RELATIVE URLS: html@base to ensure relative links/images resolution
        if(basePath) {
            html += "<base href=\"file://";
            html += *basePath;
            html += "/\">";
#ifdef DO_MF_DEBUG
        html += "\n";
#endif
        }

        // TODO SCROLLING: scrolling bridge
        //html += "<script type=\"text/javascript\">window.onscroll = function() { synchronizer.webViewScrolled(); };</script>";
#ifdef DO_MF_DEBUG
        //html += "\n";
#endif


        // DIAGRAMS: mermaid.js
        // - CDN: https://cdnjs.com/libraries/mermaid
        // - download from: https://unpkg.com/mermaid@7.1.0/dist/
        // - live demo: https://mermaidjs.github.io/mermaid-live-editor
        if(standalone) {
            html += "<script type=\"text/javascript\" src=\"";
            html += JS_LIB_MERMAILD_URL;
            html += "\"></script>";
        } else {
            switch(config.getUiEnableDiagramsInMd()) {
            case Configuration::JavaScriptLibSupport::ONLINE:
                html += "<script type=\"text/javascript\" src=\"";
                html += JS_LIB_MERMAILD_URL;
                html += "\"></script>";
#ifdef DO_MF_DEBUG
                html += "\n";
#endif
                break;
            case Configuration::JavaScriptLibSupport::OFFLINE:
                html += "<script type=\"text/javascript\" src=\"qrc:/js/mermaid.js\"></script>";
#ifdef DO_MF_DEBUG
                html += "\n";
#endif
                break;
            default:
                break;
            }
        }

        // MATH: MathJax.js
        // - doc: http://docs.mathjax.org/en/latest/start.html
        // - CDN: https://cdnjs.com/libraries/mathjax
        // - check newMathJax variable in the script above e.g. https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.1/MathJax.js?config=TeX-AMS-MML_HTMLorMML
        if(standalone || config.isUiEnableMathInMd()) {
            html += "<script type=\"text/x-mathjax-config\">MathJax.Hub.Config({tex2jax: {inlineMath: [['$','$'], ['\\\\(','\\\\)']]}});</script>";
#ifdef DO_MF_DEBUG
            html += "\n";
#endif
            html += "<script type=\"text/javascript\" src=\"https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.4/latest.js?config=TeX-MML-AM_CHTML\"></script>";
#ifdef DO_MF_DEBUG
            html += "\n";
#endif
        }

        // SYNTAX HIGHLIGHTING: offline Highlight.js (CME)
        if(standalone) {
            html += "<link rel=\"stylesheet\" href=\"http://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.4.0/styles/default.min.css\">";
            html += "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/9.4.0/highlight.min.js\"></script>";
            html += "<script>hljs.initHighlightingOnLoad();</script>";
        } else if(config.isUiEnableSrcHighlightInMd()) {
            html += "<link rel=\"stylesheet\" href=\"qrc:/html-css/highlight.css\"/>";
#ifdef DO_MF_DEBUG
            html += "\n";
#endif
            html += "<script type=\"text/javascript\" src=\"qrc:/js/highlight.js\"></script>";
#ifdef DO_MF_DEBUG
            html += "\n";
#endif
            html += "<script type=\"text/javascript\">hljs.initHighlightingOnLoad();</script>";
#ifdef DO_MF_DEBUG
            html += "\n";
#endif
        }

        // UI THEME CSS:
        html += "<link rel=\"stylesheet\" href=\"";
        if(standalone) {
            html += EXPORT_DEFAULT_CSS_URL;
        } else {
            html += config.getUiHtmlCssPath();
        }
        html += "\"/>";
#ifdef DO_MF_DEBUG
        html += "\n";
#endif

        if(yScrollTo > 0) {
            html += "<script type=\"text/javascript\">window.addEventListener('load', function () { window.scrollTo(0, (document.body.scrollHeight/100) * ";
            html += std::to_string(yScrollTo);
            html += "); });</script>";
        }

        html +=
            "</head>"
            "<body>";
    }
}

void HtmlOutlineRepresentation::footer(string& html)
{
    if(!config.isUiHtmlTheme()) {
        html += "</pre>";
    }
    html +=
        "</body>"
        "</html>";
}

string* HtmlOutlineRepresentation::to(const string* markdown, string* html, string* basePath, bool standalone, int yScrollTo)
{
    if(!config.isUiHtmlTheme()) {
        header(*html, basePath, standalone, yScrollTo);
        html->append(*markdown);
        footer(*html);
    } else {
        // create HTML body using Discount first
        html->clear();
        header(*html, basePath, standalone, yScrollTo);

        if(markdown->size() > 0) {
#ifdef MF_NO_MD_2_HTML
            // Discount: make sure that MD ends with new line, otherwise there would be missing characters in output HTML
            //if(markdown->at(markdown->size()-1) != '\n') {
            //    markdown->append("\n");
            //}

            html->append("<pre>");
            html->append(*markdown);
            html->append("</pre>");
#else
            markdownTranscoder->to(RepresentationType::HTML, markdown, html);
#endif
        }

        footer(*html);
    }

#ifdef MF_DEBUG_HTML
    // debug generated HTML
    MF_DEBUG("=== BEGIN HTML ===" << endl << *html << endl << "=== END HTML ===" << endl);
#endif
    return html;
}

string* HtmlOutlineRepresentation::to(const Outline* outline, string* html, bool standalone, int yScrollTo)
{
    // IMPROVE markdown can be processed by Mind to be enriched with various links and relationships
    string* markdown = markdownRepresentation.to(outline);    
    to(markdown, html, nullptr, standalone, yScrollTo);
    delete markdown;
    return html;
}

string* HtmlOutlineRepresentation::toHeader(Outline* outline, string* html, bool standalone, bool autolinking, int yScrollTo)
{
    if(!config.isUiHtmlTheme()) {
        header(*html, nullptr, standalone, 0);
        string markdown{"# "};
        markdown += outline->getName();
        markdown += "\n";
        markdown += outline->getOutlineDescriptorAsNote()->getDescriptionAsString();
        html->append(markdown);
        footer(*html);
    } else {
        string htmlHeader{};
        htmlHeader.reserve(1000);

        // table
        htmlHeader =
                "<body>" // body tag is later replaced in Discount generated HTML > must be present in the header
                "<table style='width: 100%; border-collapse: collapse; border: none;'>"
                "<tr style='border-collapse: collapse; border: none;'>"
                "<td style='border-collapse: collapse; border: none;'>"
                "<h2>";
        htmlHeader += outline->getName();
        htmlHeader += "</h2>";

        // O type
        outlineTypeToHtml(outline->getType(), htmlHeader);

        // tags, reads/writes and timestamps
        // IMPROVE show rs/ws/... only if it's MF repository (hide it otherwise) + configuration allows to hide it in all cases
        outlineMetadataToHtml(outline, htmlHeader);
        htmlHeader +=
                "</td>"
                "<td style='width: 50px; border-collapse: collapse; border: none;'>";
        if(outline->getProgress()) {
            htmlHeader += "<h1>";
            htmlHeader += std::to_string(outline->getProgress());
            htmlHeader += "%&nbsp;&nbsp;</h1>";
        }
        htmlHeader +=
                "</td>"
                "<td style='width: 50px; border-collapse: collapse; border: none;'>"
                "<table style='font-size: 100%; border-collapse: collapse; border: none;'>"
                "<tr style='border-collapse: collapse; border: none;'>";
        if(outline->getImportance() || outline->getUrgency()) {
            if(outline->getImportance() > 0) {
                for(int i=0; i<=4; i++) {
                    htmlHeader += "<td style='border-collapse: collapse; border: none;'>";
                    if(outline->getImportance()>i) {
                        htmlHeader += "&#9733;";
                    } else {
                        htmlHeader += "&#9734;";
                    }
                    htmlHeader += "</td>";
                }
            } else {
                for(int i=0; i<5; i++) {
                    htmlHeader +=
                            "<td style='border-collapse: collapse; border: none;'>"
                            "&#9734;"
                            "</td>";
                }
            }
            htmlHeader +=
                    "</tr>"
                    "<tr style='border-collapse: collapse; border: none;'>";
            if(outline->getUrgency()>0) {
                for(int i=0; i<=4; i++) {
                    if(outline->getUrgency()>i) {
                        htmlHeader +=
                                "<td style='border-collapse: collapse; border: none;'>"
                                "&#x25D5;"
                                //"&#x29D7;" // sand clocks
                                "</td>";
                    } else {
                        htmlHeader +=
                                "<td style='border-collapse: collapse; border: none;'>"
                                "&#x25F4;"
                                //"&#x29D6;" // sand clocks
                                "</td>";
                    }
                }
            } else {
                for(int i=0; i<5; i++) {
                    htmlHeader +=
                            "<td style='border-collapse: collapse; border: none;'>"
                            "&#x29D6;"
                            "</td>";
                }
            }
        }
        htmlHeader +=
                "</tr></table>"
                "</td>"
                "</tr></table>";

        // O tags
        tagsToHtml(outline->getTags(), htmlHeader);
        htmlHeader += "<br/>";

        // HTML completion
        string outlineMd{};
        string path, file;
        pathToDirectoryAndFile(outline->getKey(), path, file);
        if(autolinking) {
            markdownRepresentation.toDescription(
                        outline->getOutlineDescriptorAsNote(),
                        &outlineMd,
                        autolinking);
        } else {
            outlineMd.append(outline->getOutlineDescriptorAsNote()->getDescriptionAsString());
        }
        to(&outlineMd, html, &path, false, yScrollTo);
        // inject custom HTML header
        html->replace(
                    html->find("<body>"), // <body> element index
                    6, // <body> element length (chars to be replaced)
                    htmlHeader); // new string
    }

#ifdef MF_DEBUG_HTML
    // debug generated HTML
    MF_DEBUG("=== BEGIN HEADER HTML ===" << endl << *html << endl << "=== END HEADER HTML ===" << endl);
#endif

    return html;
}

string* HtmlOutlineRepresentation::to(const Note* note, string* html, bool autolinking, int yScrollTo)
{
    string* markdown = new string{};
    markdown->reserve(MarkdownOutlineRepresentation::AVG_NOTE_SIZE);
    markdownRepresentation.to(note, markdown, true, autolinking);

    string path, file;
    pathToDirectoryAndFile(note->getOutlineKey(), path, file);
    to(markdown, html, &path, false, yScrollTo);
    delete markdown;
    return html;
}

} // m8r namespace
