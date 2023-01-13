/*
 html_outline_representation.h     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <vector>

#include "../../config/configuration.h"
#include "../../model/note.h"
#include "../unicode.h"
#include "../markdown/markdown_outline_representation.h"
#include "../markdown/markdown_transcoder.h"
#if defined  MF_MD_2_HTML_CMARK
  #include "../markdown/cmark_gfm_markdown_transcoder.h"
#endif

namespace m8r {

/**
 * @brief The HTML colors representation.
 *
 * Interface that provides colors for themed HTML rendering.
 */
class HtmlColorsRepresentation
{
public:
    virtual std::string& getHtmlTextColor() = 0;
    virtual std::string& getHtmlBackgroundColor() = 0;
};

class HtmlExportColorsRepresentation : public HtmlColorsRepresentation
{
public:

    static void organizerTypeToHtml(
        const Organizer* organizer,
        std::string& html
    );

public:
    explicit HtmlExportColorsRepresentation() {}
    HtmlExportColorsRepresentation(const HtmlExportColorsRepresentation&) = delete;
    HtmlExportColorsRepresentation(const HtmlExportColorsRepresentation&&) = delete;
    HtmlExportColorsRepresentation& operator =(const HtmlExportColorsRepresentation&) = delete;
    HtmlExportColorsRepresentation& operator =(const HtmlExportColorsRepresentation&&) = delete;
    virtual ~HtmlExportColorsRepresentation() {}

    virtual std::string& getHtmlTextColor() { static std::string TEXT_COLOR = std::string{"#000000"}; return TEXT_COLOR; }
    virtual std::string& getHtmlBackgroundColor() { static std::string BACKGROUND_COLOR = std::string{"#FFFFFF"}; return BACKGROUND_COLOR; }
};

/**
 * @brief HTML Outline representation.
 */
class HtmlOutlineRepresentation
{
private:
    // Performance hints:
    //  - += is ~2x faster than append() (depends on cpp lib implementation)
    //  - pre-allocation of the string using reserver() is critical to avoid slow re-allocations
    //  - IMPROVE consider caching of re-used HTML snippets

    Configuration& config;
    HtmlExportColorsRepresentation exportColors;
    HtmlColorsRepresentation& lf;    
    MarkdownOutlineRepresentation markdownRepresentation;
    MarkdownTranscoder* markdownTranscoder;

public:
    /**
     * @brief Html O representation.
     *
     * Representation interceptor is the head of the interceptor chain.
     */
    explicit HtmlOutlineRepresentation(
            Ontology& ontology,
            HtmlColorsRepresentation& lf,
            RepresentationInterceptor* descriptionInterceptor);
    explicit HtmlOutlineRepresentation(
            Ontology& ontology,
            RepresentationInterceptor* descriptionInterceptor);
    HtmlOutlineRepresentation(const HtmlOutlineRepresentation&) = delete;
    HtmlOutlineRepresentation(const HtmlOutlineRepresentation&&) = delete;
    HtmlOutlineRepresentation &operator=(const HtmlOutlineRepresentation&) = delete;
    HtmlOutlineRepresentation &operator=(const HtmlOutlineRepresentation&&) = delete;
    virtual ~HtmlOutlineRepresentation();

    std::string* to(
        const std::string* markdown,
        std::string* html,
        std::string* basePath=nullptr,
        bool standalone=false,
        int yScrollTo=0
    );

    /**
     * @brief Export Outline to HTML.
     *
     * Outline is rendered to be shown in MF, standalone option indicates HTML export.
     *
     * @param outline O whose HTML representation will be created.
     * @param html Resulting HTML representation.
     * @param standalone Is it O export?
     * @param autolinking Inject (auto) links.
     * @param whole Rendere whole O or just header.
     * @param metadata Render (tags, timestamps, reads/writes) metadata.
     *        No metadata (fast) rendering is used by live preview.
     * @param yScrollTo Inject JavaScript which scrolls HTML to given % on page load.
     * @return Pointer to O HTML representation.
     */
    std::string* to(
        Outline* outline,
        std::string* html,
        bool standalone=false,
        bool autolinking=false,
        bool whole=false,
        bool metadata=false,
        int yScrollTo=0
    );
    std::string* to(
        const Note* note,
        std::string* html,
        bool autolinking=false,
        int yScrollTo=0
    );

    /**
     * @brief Append "color: 0x...; background-color: 0x...;"
     */
    void fgBgTextColorStyle(std::string& html);
    void outlineTypeToHtml(const OutlineType* outlineType, std::string& html);
    void noteTypeToHtml(const NoteType* noteType, std::string& html);
    void organizerTypeToHtml(const Organizer* organizer, std::string& html);
    void tagsToHtml(const std::vector<const Tag*>* tags, std::string& html);
    void outlineMetadataToHtml(const Outline* outline, std::string& html);

    MarkdownOutlineRepresentation& getMarkdownRepresentation() { return markdownRepresentation; }

private:
    void header(std::string& html, std::string* basePath, bool standalone, int yScrollTo);
    void footer(std::string& html);

    std::string* toNoMeta(Outline* outline, std::string* html, bool standalone, int yScrollTo);
};

} // m8r namespace

#endif /* M8R_HTML_OUTLINE_REPRESENTATION_H_ */
