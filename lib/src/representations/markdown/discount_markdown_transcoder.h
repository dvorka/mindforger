/*
 discount_markdown_transcoder.h     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_DISCOUNT_MARKDOWN_TRANSCODER_H
#define M8R_DISCOUNT_MARKDOWN_TRANSCODER_H

#include "markdown_transcoder.h"

#include "../../config/configuration.h"

namespace m8r {

/**
 * @brief Discount options.
 *
 * See https://www.pell.portland.or.us/~orc/Code/discount/
 */
 enum class DiscountOption {
    NoLinksOption          = 1<<0, /* don't do link processing, block <a> tags  */
    NoImagesOption         = 1<<1, /* don't do image processing, block <img> */
    NoSmartypantsOption    = 1<<2, /* don't run smartypants() */
    NoHtmlOption           = 1<<3, /* don't allow raw html through AT ALL */
    NoSuperscriptOption    = 1<<4, /* don't process a^2 as superscript(<sup>) */
    NoTablesOption         = 1<<5, /* disallow tables */
    NoStrikethroughOption  = 1<<6, /* forbid ~~strikethrough~~ */
    TableOfContentsOption  = 1<<7, /* do table-of-contents processing */
    AutolinkOption         = 1<<8, /* make http://foo.com link even without <>s */
    NoHeaderOption         = 1<<9, /* don't process header blocks */
    NoDivQuoteOption       = 1<<10, /* forbid >%class% blocks */
    NoAlphaListOption      = 1<<11, /* forbid alphabetic lists */
    NoDefinitionListOption = 1<<12, /* forbid definition lists */
    ExtraFootnoteOption    = 1<<13, /* enable markdown extra-style footnotes */
    NoStyleOption          = 1<<14, /* don't extract <style> blocks */
};

/**
 * @brief Discount based Markdown to HTML transcoder.
 *
 * See https://github.com/Orc/discount
 */
class DiscountMarkdownTranscoder : public MarkdownTranscoder
{
private:
    Configuration& config;

    /**
     * @brief MindForger MD 2 HTML options.
     *
     * If MF options don't change, then Discount options doesn't have to be recalculated.
     */
    unsigned int lastMfOptions;
    unsigned int discountOptions;

public:
    explicit DiscountMarkdownTranscoder();
    DiscountMarkdownTranscoder(const DiscountMarkdownTranscoder&) = delete;
    DiscountMarkdownTranscoder(const DiscountMarkdownTranscoder&&) = delete;
    DiscountMarkdownTranscoder &operator=(const DiscountMarkdownTranscoder&) = delete;
    DiscountMarkdownTranscoder &operator=(const DiscountMarkdownTranscoder&&) = delete;
    virtual ~DiscountMarkdownTranscoder();

    virtual std::string* to(
            RepresentationType format,
            const std::string* markdown,
            std::string* html);
};

}
#endif // M8R_DISCOUNT_MARKDOWN_TRANSCODER_H
