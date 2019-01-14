/*
 markdown_transcoder.h     MindForger thinking notebook

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
#ifndef M8R_MARKDOWN_TRANSCODER_H
#define M8R_MARKDOWN_TRANSCODER_H

#include <string>

#include "../representation_type.h"

namespace m8r {

/**
 * @brief The Markdown transcoder.
 *
 * Interface for implementations allowing conversion of Markdown
 * to HTML and other representations.
 */
class MarkdownTranscoder
{    
public:
    explicit MarkdownTranscoder();
    MarkdownTranscoder(const MarkdownTranscoder&) = delete;
    MarkdownTranscoder(const MarkdownTranscoder&&) = delete;
    MarkdownTranscoder &operator=(const MarkdownTranscoder&) = delete;
    MarkdownTranscoder &operator=(const MarkdownTranscoder&&) = delete;
    ~MarkdownTranscoder();

    std::string* to(
            const RepresentationType representationType,
            const std::string* markdown,
            std::string* representation);

};

}
#endif // M8R_MARKDOWN_TRANSCODER_H
