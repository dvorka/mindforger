/*
 time_scope_aspect.h     MindForger thinking notebook

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
    time_t timepoint;

    /**
     * @brief Relative interval to remember in seconds.
     */
    int relative;

    int years, months, days, hours, minutes;

public:
    explicit TimeScopeAspect();
    TimeScopeAspect(const TimeScopeAspect&) = delete;
    TimeScopeAspect(const TimeScopeAspect&&) = delete;
    TimeScopeAspect &operator=(const TimeScopeAspect&) = delete;
    TimeScopeAspect &operator=(const TimeScopeAspect&&) = delete;
    ~TimeScopeAspect();

    virtual bool isEnabled() const { return relative>0; }
    bool isOutOfScope(const Outline* o) const { return o->getRead()<timepoint; }
    bool isOutOfScope(const Note* n) const { return n->getRead()<timepoint; }
    bool isInScope(const Outline* o) const { return !isOutOfScope(o); }
    bool isInScope(const Note* n) const { return !isOutOfScope(n); }

    /**
     * @brief Set threshold by the relative specification e.g. remember just recent year.
     */
    void setTimePoint(int years, int months, int days, int hours, int minutes)
    {
        this->years = years;
        this->months = months;
        this->days = days;
        this->hours = hours;
        this->minutes = minutes;

        relative =
                minutes*60 +
                hours*60*60 +
                days*60*60*24 +
                months*60*60*24*30 +
                years*60*60*24*365;

        time_t now;
        time(&now);

        timepoint = now-relative;

        MF_DEBUG("R " << relative << std::endl);
    }

    int getYears() const { return years; }
    int getMonths() const { return months; }
    int getDays() const { return days; }
    int getHours() const { return hours; }
    int getMinutes() const { return minutes; }

    void setTimePoint(time_t timepoint);
    std::string getTimePointAsString() const;
    void clearTreshold() { relative=0; }
};

}
#endif // M8R_TIME_SCOPE_ASPECT_H
