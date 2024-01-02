/*
 mind_scope_aspect.h     MindForger thinking notebook

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
#ifndef M8R_MIND_SCOPE_ASPECT_H
#define M8R_MIND_SCOPE_ASPECT_H

#include "time_scope_aspect.h"
#include "tag_scope_aspect.h"

namespace m8r {

/**
 * @brief Composite aspect that combines Mind aspects (time, tags, ...) into one.
 *
 * This implementation has aspects hardcoded (and ANDed) to achieve a reasonable
 * performance, but (if desired) it can be generalized to a bool expression
 * combining various aspects or even general aspect query (builder).
 */
class MindScopeAspect
{
    TimeScopeAspect& timeScope;
    TagsScopeAspect& tagsScope;

public:
    explicit MindScopeAspect(TimeScopeAspect& time, TagsScopeAspect& tags);
    MindScopeAspect(const MindScopeAspect&) = delete;
    MindScopeAspect(const MindScopeAspect&&) = delete;
    MindScopeAspect &operator=(const MindScopeAspect&) = delete;
    MindScopeAspect &operator=(const MindScopeAspect&&) = delete;
    ~MindScopeAspect();

    virtual bool isEnabled() const {
        return timeScope.isEnabled() || tagsScope.isEnabled();
    }
    bool isOutOfScope(const Outline* o) const {
        if(timeScope.isEnabled()) {
            if(timeScope.isOutOfScope(o)) {
                return true;
            }
        }
        if(tagsScope.isEnabled()) {
            if(tagsScope.isOutOfScope(o)) {
                return true;
            }
        }
        return false;
    }
    bool isOutOfScope(const Note* n) const {
        if(timeScope.isEnabled()) {
            if(timeScope.isOutOfScope(n)) {
                return true;
            }
        }
        // tags are not used w/ Ns
        return false;
    }
    bool isInScope(const Outline* o) const {
        if(timeScope.isEnabled()) {
            if(timeScope.isOutOfScope(o)) {
                return false;
            }
        }
        if(tagsScope.isEnabled()) {
            if(tagsScope.isOutOfScope(o)) {
                return false;
            }
        }
        return true;
    }
    bool isInScope(const Note* n) const {
        if(timeScope.isEnabled()) {
            if(timeScope.isOutOfScope(n)) {
                return false;
            }
        }
        // tags are not used w/ Ns
        return true;
    }
};

}
#endif // M8R_MIND_SCOPE_ASPECT_H
