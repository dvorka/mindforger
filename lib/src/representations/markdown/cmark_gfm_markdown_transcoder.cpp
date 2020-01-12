/*
 cmark_gfm_markdown_transcoder.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "cmark_gfm_markdown_transcoder.h"
// cmark-gfm headers must NOT be included in header - Win builds fail
#ifdef MF_MD_2_HTML_CMARK
  #include <cmark-gfm.h>
  #include <cmark-gfm-core-extensions.h>
  #include <registry.h>
  #include <parser.h>
#endif // MF_MD_2_HTML_CMARK

namespace m8r {

using namespace std;

CmarkGfmMarkdownTranscoder::CmarkGfmMarkdownTranscoder() : config(Configuration::getInstance())
{
    cmarkOptions = lastMfOptions = 0;
}

CmarkGfmMarkdownTranscoder::~CmarkGfmMarkdownTranscoder()
{
}

string* CmarkGfmMarkdownTranscoder::to(RepresentationType format, const string* markdown, string* html)
{
    // options
    unsigned int mfOptions = config.getMd2HtmlOptions();
    if(mfOptions != lastMfOptions) {
        lastMfOptions = mfOptions;
    }
#ifdef MF_MD_2_HTML_CMARK
    if(format == RepresentationType::HTML) {
        //TODO VH: move to some general init code?
        cmark_gfm_core_extensions_ensure_registered();
        cmark_mem *mem = cmark_get_default_mem_allocator();
        //TODO VH: use extensions per config
        cmark_llist *syntax_extensions = cmark_list_syntax_extensions(mem);
        //TODO VH: parse options
        cmark_parser *parser = cmark_parser_new(CMARK_OPT_DEFAULT | CMARK_OPT_UNSAFE);
        for (cmark_llist *tmp = syntax_extensions; tmp; tmp = tmp->next) {
            cmark_parser_attach_syntax_extension(parser, (cmark_syntax_extension *)tmp->data);
        }
        cmark_parser_feed(parser, markdown->c_str(), markdown->size());

        cmark_node *doc = cmark_parser_finish(parser);
        //cmark_node *doc = cmark_parse_document (markdown->c_str(), markdown->size(), CMARK_OPT_DEFAULT | CMARK_OPT_UNSAFE);
        if (doc) {
            char *rendered_html = cmark_render_html_with_mem(doc, CMARK_OPT_DEFAULT | CMARK_OPT_UNSAFE, parser->syntax_extensions, mem);
            if (rendered_html) {
                html->append(rendered_html);
                free(rendered_html);
            }
            cmark_node_free(doc);
        }
        cmark_llist_free(mem, syntax_extensions);
        cmark_parser_free(parser);
    }
    else {
        html->append(*markdown);
    }
#else
    html->append(*markdown);
#endif
    return html;
}

} // m8r namespace
