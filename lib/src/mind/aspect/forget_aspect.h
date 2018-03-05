/*
 forget_aspect.h     MindForger thinking notebook

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
#ifndef M8R_FORGET_ASPECT_H
#define M8R_FORGET_ASPECT_H

#include <ctime>
#include <string>

#include "aspect.h"

namespace m8r {

class ForgetAspect : public Aspect
{
private:

    /**
     * @brief Show only Os/Ns that were modified after this point in time.
     *
     * Anything what is older than now-forget is *not* shown to user, but
     * kept in and maintained in mind.
     */
    time_t threshold;

    /**
     * @brief Relative interval to remember in seconds.
     */
    int relative;

    int years, months, days, hours, minutes;

public:
    explicit ForgetAspect();
    ForgetAspect(const ForgetAspect&) = delete;
    ForgetAspect(const ForgetAspect&&) = delete;
    ForgetAspect &operator=(const ForgetAspect&) = delete;
    ForgetAspect &operator=(const ForgetAspect&&) = delete;
    ~ForgetAspect();

    virtual bool isEnabled() const { return relative>0; }
    bool isForgotten(time_t t) const { return t < threshold; }
    bool isRemembered(time_t t) const { return t >= threshold; }

    /**
     * @brief Set threshold by the relative specification e.g. remember just recent year.
     */
    void setThreshold(int years, int months, int days, int hours, int minutes)
    {
        years=years;
        months=months;
        days=days;
        hours=hours;
        minutes=minutes;

        relative =
                minutes*60 +
                hours*60*60 +
                days*60*60*24 +
                months*60*60*24*30 +
                years*60*60*24*365;

        time_t now;
        time(&now);

        threshold = now-relative;
    }

    int getYears() const { return years; }
    int getMonths() const { return months; }
    int getDays() const { return days; }
    int getHours() const { return hours; }
    int getMinutes() const { return minutes; }

    void setThreshold(time_t threshold);
    std::string getThresholdAsString() const;
    void clearTreshold() { relative=0; }
};

}
#endif // M8R_FORGET_ASPECT_H
