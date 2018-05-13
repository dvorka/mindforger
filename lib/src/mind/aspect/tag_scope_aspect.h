/*
 tag_scope_aspect.h     MindForger thinking notebook

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
#ifndef M8R_TAG_SCOPE_ASPECT_H
#define M8R_TAG_SCOPE_ASPECT_H

#include <vector>

#include "../../model/outline.h"
#include "aspect.h"

namespace m8r {

class TagScopeAspect : public Aspect
{
private:
    std::vector<const Tag*> tags;

public:
    explicit TagScopeAspect();
    TagScopeAspect(const TagScopeAspect&) = delete;
    TagScopeAspect(const TagScopeAspect&&) = delete;
    TagScopeAspect &operator=(const TagScopeAspect&) = delete;
    TagScopeAspect &operator=(const TagScopeAspect&&) = delete;
    ~TagScopeAspect();

    virtual bool isEnabled() const { return !tags.empty(); }
    bool isOutOfScope(const Outline* o) const;
    bool isOutOfScope(const Note* n) const;
    bool isInScope(const Outline* o) const;
    bool isInScope(const Note* n) const;

    void setTags(const std::vector<Tag*> t) {
        tags.assign(t.begin(), t.end());
    }

private:
    bool inScope(const std::vector<const Tag*>* thingTags) const;
};

}
#endif // M8R_TAG_SCOPE_ASPECT_H
