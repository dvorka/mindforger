/*
 time_scope_aspect.h     MindForger thinking notebook

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
#ifndef M8R_TIME_SCOPE_ASPECT_H
#define M8R_TIME_SCOPE_ASPECT_H

#include <ctime>
#include <string>

#include "../../model/outline.h"
#include "aspect.h"
#include "../../debug.h"

namespace m8r {

class TimeScopeAspect : public Aspect
{
private:

    /**
     * @brief Show only Os/Ns that were modified after this point in time.
     *
     * Anything what is older than timepoint is *not* shown to user (when
     * viewing O/N), but it can be recalled and it's maintained in memory.
     */
    time_t timePoint;

    TimeScope timeScope;

public:
    explicit TimeScopeAspect();
    TimeScopeAspect(const TimeScopeAspect&) = delete;
    TimeScopeAspect(const TimeScopeAspect&&) = delete;
    TimeScopeAspect &operator=(const TimeScopeAspect&) = delete;
    TimeScopeAspect &operator=(const TimeScopeAspect&&) = delete;
    ~TimeScopeAspect();

    virtual bool isEnabled() const { return timeScope.relativeSecs>0; }
    bool isOutOfScope(const Outline* o) const { return o->getRead()<timePoint; }
    bool isOutOfScope(const Note* n) const { return n->getRead()<timePoint; }
    bool isInScope(const Outline* o) const { return !isOutOfScope(o); }
    bool isInScope(const Note* n) const { return !isOutOfScope(n); }

    /**
     * @brief Set time scope by RELATIVE specification e.g. remember just recent year.
     */
    void setTimeScope(const TimeScope& timeScope)
    {
        this->timeScope = timeScope;

        time_t now;
        time(&now);

        timePoint = now-timeScope.relativeSecs;
    }
    TimeScope& getTimeScope() { return timeScope; }
    std::string getTimeScopeAsString();
    void resetTimeScope() { timeScope.reset(); }

    void setTimePoint(time_t timePoint);
};

}
#endif // M8R_TIME_SCOPE_ASPECT_H
