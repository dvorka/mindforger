/*
 link.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_LINK_H
#define M8R_LINK_H

#include <string>

namespace m8r {

/**
 * @brief Link between O/N and O/N.
 *
 * Os and Ns have Links and Relationships.
 *
 * Link is name/URL pair (simplified explicit relationship) that MAY be
 * resolved to Relationship whenever link target URL points to O or N.
 * Links are resolved by Mind when ALL Os are loaded i.e. targets are known.
 *   User works with Links, Mind resolves them to Relationships and uses them
 * for inferencing and in Navigators i.e. when Mind sleeps, there are NO
 * Relationships only Links.
 *   Change to Links triggers reindexation actions in Mind (O/N forgotten/deleted,
 * moved, ...).
 */
class Link {
private:
    std::string name;
    std::string url;

public:
    explicit Link(const std::string& name, const std::string& url)
        : name(name), url(url) {}

    std::string& getName() { return name; }
    std::string& getUrl() { return url; }
};

}
#endif // M8R_LINK_H
