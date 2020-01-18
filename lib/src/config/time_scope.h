/*
 time_scope.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_TIME_SCOPE_H
#define M8R_TIME_SCOPE_H

#include <string>
#include <sstream>

#include <sys/types.h>

#include "../debug.h"
#include "../config/config.h"

namespace m8r {

struct TimeScope {

    static bool fromString(const std::string& s, TimeScope& t);

    u_int8_t years;
    u_int8_t months;
    u_int8_t days;
    u_int8_t hours;
    u_int8_t minutes;

    /**
     * @brief Relative interval in seconds.
     */
    int relativeSecs;

    explicit TimeScope()
    {
        reset();
    }

    explicit TimeScope(u_int8_t years, u_int8_t months, u_int8_t days, u_int8_t hours, u_int8_t minutes)
    {
        this->years = years;
        this->months = months;
        this->days = days;
        this->hours = hours;
        this->minutes = minutes;

        recalculateRelativeSecs();
    }

    TimeScope(const TimeScope& t)
    {
        this->years = t.years;
        this->months = t.months;
        this->days = t.days;
        this->hours = t.hours;
        this->minutes = t.minutes;

        recalculateRelativeSecs();
    }

    TimeScope& operator=(const TimeScope& t)
    {
        this->years = t.years;
        this->months = t.months;
        this->days = t.days;
        this->hours = t.hours;
        this->minutes = t.minutes;

        recalculateRelativeSecs();

        return *this;
    }

    TimeScope(const TimeScope&& t)
    {
        this->years = t.years;
        this->months = t.months;
        this->days = t.days;
        this->hours = t.hours;
        this->minutes = t.minutes;

        recalculateRelativeSecs();
    }

    TimeScope& operator=(const TimeScope&&) = delete;

    void recalculateRelativeSecs()
    {
        relativeSecs =
                minutes*60 +
                hours*60*60 +
                days*60*60*24 +
                months*60*60*24*30 +
                years*60*60*24*365;
    }

    void reset()
    {
        years=months=days=hours=minutes=0;
        relativeSecs=0;
    }

    void toString(std::string& s) const;
};

}
#endif // M8R_TIME_SCOPE_H
