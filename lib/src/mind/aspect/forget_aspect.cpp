/*
 forget_aspect.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "forget_aspect.h"

namespace m8r {

using namespace std;

ForgetAspect::ForgetAspect()
{
    threshold = 0;
    relative = 0;
}

ForgetAspect::~ForgetAspect()
{
}

string ForgetAspect::getThresholdAsString() const
{
    string s{""};
    if(relative) {
        if(years) {
            s.append(std::to_string(years));
            s.append("y");
        }
        if(months) {
            s.append(std::to_string(months));
            s.append("m");
        }
        if(days) {
            s.append(std::to_string(days));
            s.append("d");
        }
        if(hours) {
            s.append(std::to_string(hours));
            s.append("h");
        }
        if(minutes) {
            s.append(std::to_string(minutes));
            s.append("y");
        }

        return std::move(s);
    } else {
        return s;
    }
}

} // m8r namespace
