/*
 markdown_transcoder.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
