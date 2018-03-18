/*
 html_outline_representation.h     MindForger thinking notebook

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
#ifndef M8R_HTML_OUTLINE_REPRESENTATION_H_
#define M8R_HTML_OUTLINE_REPRESENTATION_H_

#include <string>

#include "../../model/note.h"
#include "../markdown/markdown_outline_representation.h"

namespace m8r {

constexpr const auto MF_HTML_HEAD =
   "<!DOCTYPE html>"
   "<html>"
   " <head></head>"
   " <body";

constexpr const auto MF_HTML_TAIL =
   " </body>"
   "</html>";

/**
 * @brief HTML Outline representation.
 *
 * Discount-based HTML Outline representation.
 */
class HtmlOutlineRepresentation
{
private:
    Configuration& config;
    MarkdownOutlineRepresentation markdownRepresentation;

    /**
     * @brief Last MindForger MD 2 HTML options.
     *
     * If MF options don't change, then Discount options doesn't have to be recalculated.
     */
    unsigned int lastMfOptions;
    unsigned int discountOptions;

public:
    explicit HtmlOutlineRepresentation(Ontology& ontology);
    HtmlOutlineRepresentation(const HtmlOutlineRepresentation&) = delete;
    HtmlOutlineRepresentation(const HtmlOutlineRepresentation&&) = delete;
    HtmlOutlineRepresentation &operator=(const HtmlOutlineRepresentation&) = delete;
    HtmlOutlineRepresentation &operator=(const HtmlOutlineRepresentation&&) = delete;
    virtual ~HtmlOutlineRepresentation();

    std::string* to(const std::string* markdown, std::string* html);

    std::string* to(const Outline* outline, std::string* html);
    std::string* toHeader(const Outline* outline, std::string* html);
    std::string* to(const Note* note, std::string* html);
};

} // m8r namespace

#endif /* M8R_HTML_OUTLINE_REPRESENTATION_H_ */
