/*
 discount_markdown_transcoder.cpp     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "discount_markdown_transcoder.h"

#if not defined MF_NO_MD_2_HTML
  extern "C" {
  #include "../../../../deps/discount/mkdio.h"
  }
#endif

namespace m8r {

using namespace std;

DiscountMarkdownTranscoder::DiscountMarkdownTranscoder()
    : config(Configuration::getInstance())
{
    lastMfOptions = discountOptions = 0;
}

DiscountMarkdownTranscoder::~DiscountMarkdownTranscoder()
{
}

std::string* DiscountMarkdownTranscoder::to(RepresentationType format, const string* markdown, string* html)
{
#if not defined MF_NO_MD_2_HTML
    if(format == RepresentationType::HTML) {
        MMIOT* doc = nullptr;

        // options
        unsigned int mfOptions = config.getMd2HtmlOptions();
        if(mfOptions!=lastMfOptions) {
            lastMfOptions = mfOptions;
            discountOptions
                = MKD_TOC
                    | MKD_NOSTYLE
                    | MKD_FENCEDCODE // this enables ``` and ~~~
                    | MKD_LATEX // this helps to enable mathjax
                    | MKD_AUTOLINK // enable making http://acme.com HTML link
                    | MKD_NOSUPERSCRIPT // if enabled it BREAKS MathJax
                    | (mfOptions&DiscountOption::NoStrikethroughOption?MKD_NOSTRIKETHROUGH:0)
                    | (mfOptions&DiscountOption::NoAlphaListOption?MKD_NOALPHALIST:0)
                    | (mfOptions&DiscountOption::NoDefinitionListOption?MKD_NODLIST:0)
                    | (mfOptions&DiscountOption::NoSmartypantsOption?MKD_NOPANTS:0)
                    | (mfOptions&DiscountOption::ExtraFootnoteOption?MKD_EXTRA_FOOTNOTE:0);
        }

        // compile to AST - a tree of paragraphs
        doc = mkd_string(markdown->c_str(), markdown->size(), discountOptions);
        mkd_compile(doc, discountOptions);

        char* body = nullptr;
        if(doc) {
            // compile AST to HTML string
            int bodySize = mkd_document(doc, &body);

            MF_DEBUG("Markdown by Discount " << bodySize << "B HTML" << endl);
            if(bodySize && body) {
                html->append(body);
                // body is not memory leak - it's freed automatically
            }

            mkd_cleanup(doc);
        }
    } else {
        // fallback to identity() ~ Markdown
        html->append(*markdown);
    }
#else
    html->append(*markdown);
#endif //MF_NO_MD_2_HTML
    return html;
}

} // m8r namespace
