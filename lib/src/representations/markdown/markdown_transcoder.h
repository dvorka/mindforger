/*
 markdown_transcoder.h     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_MARKDOWN_TRANSCODER_H
#define M8R_MARKDOWN_TRANSCODER_H

#include <string>

#include "../representation_type.h"

namespace m8r {

// JavaScript based diagrams for HTML
// https://cdnjs.cloudflare.com/ajax/libs/mermaid/8.4.8/mermaid.min.js
//   https://cdnjs.cloudflare.com/ajax/libs/mermaid/8.4.8/mermaid.js
// https://unpkg.com/browse/mermaid@8.4.8/
//   https://unpkg.com/mermaid@8.4.8/dist/mermaid.js
constexpr const auto JS_LIB_MERMAILD_URL = "https://cdnjs.cloudflare.com/ajax/libs/mermaid/8.4.8/mermaid.js";

// CSS cannot be downloaded from raw.githubusercontent.com (probably wrong content type) where it would be auto updated > www.mindforger.com hosting
constexpr const auto EXPORT_DEFAULT_CSS_URL = "http://www.mindforger.com/support/export/css/light.css";

// KaTeX math rendering for standalone HTML export (offline qrc:/ resources are used for in-app rendering instead)
constexpr const auto JS_LIB_KATEX_CSS_URL = "https://cdn.jsdelivr.net/npm/katex@0.18.7/dist/katex.min.css";
constexpr const auto JS_LIB_KATEX_JS_URL = "https://cdn.jsdelivr.net/npm/katex@0.18.7/dist/katex.min.js";
constexpr const auto JS_LIB_KATEX_AUTORENDER_URL = "https://cdn.jsdelivr.net/npm/katex@0.18.7/dist/contrib/auto-render.min.js";

// highlight.js source code syntax highlighting for standalone HTML export (offline qrc:/ resources
// with the full ~90 language bundle are used for in-app rendering instead); highlight.js no longer
// ships a single all-languages build, so the export path is limited to its official "common" languages
// bundle (~35 most popular languages) fetched as one request via jsDelivr's combine feature
constexpr const auto JS_LIB_HLJS_CSS_URL = "https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.11.2/styles/default.min.css";
constexpr const auto JS_LIB_HLJS_JS_URL =
    "https://cdn.jsdelivr.net/combine/"
    "gh/highlightjs/cdn-release@11.11.2/build/highlight.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/xml.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/bash.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/c.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/cpp.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/csharp.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/css.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/markdown.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/diff.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/ruby.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/go.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/graphql.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/ini.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/java.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/javascript.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/json.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/kotlin.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/less.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/lua.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/makefile.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/perl.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/objectivec.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/php.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/php-template.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/plaintext.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/python.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/python-repl.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/r.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/rust.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/scss.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/shell.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/sql.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/swift.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/yaml.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/typescript.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/vbnet.min.js,"
    "gh/highlightjs/cdn-release@11.11.2/build/languages/wasm.min.js";

/**
 * @brief Markdown to HTML options.
 *
 * IMPORTANT:
 * Lower 16 bits are reserved for 3rd party implementations,
 * higher 16 bits are used for transcoder independent options.
 */
enum MdToHtmlOption
{
    // Math/code highlight/diagram via .js libs - can make HTML rendering slow
    MathSupport            = 1<<16,
    CodeHighlighting       = 1<<17,
    DiagramSupport         = 1<<18
};

/**
 * @brief The Markdown transcoder.
 *
 * Interface for implementations allowing conversion of Markdown
 * to HTML and other representations.
 */
class MarkdownTranscoder
{
public:
    explicit MarkdownTranscoder() {}
    MarkdownTranscoder(const MarkdownTranscoder&) = delete;
    MarkdownTranscoder(const MarkdownTranscoder&&) = delete;
    MarkdownTranscoder &operator=(const MarkdownTranscoder&) = delete;
    MarkdownTranscoder &operator=(const MarkdownTranscoder&&) = delete;
    virtual ~MarkdownTranscoder() {}

    /**
     * @brief Convert HTML representation to given representation.
     *
     * @param representationType target representation type.
     * @param markdown input in Markdown format.
     * @param representation output in given representation type.
     */
    virtual std::string* to(
            const RepresentationType representationType,
            const std::string* markdown,
            std::string* representation) = 0;

};

}
#endif // M8R_MARKDOWN_TRANSCODER_H
