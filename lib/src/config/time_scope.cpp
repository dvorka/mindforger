/*
 time_scope.cpp     MindForger thinking notebook

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
#include "time_scope.h"

namespace m8r {

using namespace std;

void TimeScope::toString(const TimeScope& t, string& s)
{
    std::ostringstream os;
    os << (int)t.years << "y" << (int)t.months << "m" << (int)t.days << "d" << (int)t.hours << "h" << (int)t.minutes << "m";
    s.assign(os.str());
}

bool TimeScope::fromString(const std::string& s, TimeScope& t)
{
    if(s.size()) {
        string h{};
        int i=0;
        // years
        while(isdigit(s[i])) {
            h+=s[i++];
        }
        if(h.size() && s[i]=='y') i++; else return false;
        t.years = stoi(h);
        // months
        h.clear();
        while(isdigit(s[i])) {
            h+=s[i++];
        }
        if(h.size() && s[i]=='m') i++; else return false;
        t.months= stoi(h);
        // days
        h.clear();
        while(isdigit(s[i])) {
            h+=s[i++];
        }
        if(h.size() && s[i]=='d') i++; else return false;
        t.days= stoi(h);
        // hours
        h.clear();
        while(isdigit(s[i])) {
            h+=s[i++];
        }
        if(h.size() && s[i]=='h') i++; else return false;
        t.hours= stoi(h);
        // minutes
        h.clear();
        while(isdigit(s[i])) {
            h+=s[i++];
        }
        if(h.size() && s[i]=='m') i++; else return false;
        t.minutes= stoi(h);

        return true;
    } else {
        return false;
    }
}

} // m8r namespace
