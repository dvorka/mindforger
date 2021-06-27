/*
 time_scope.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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

bool TimeScope::fromString(const std::string& s, TimeScope& t)
{
    if(s.size()) {
        string h{};
        int i=0;
        // years
        while(isdigit(s[i])) {
            h+=s[i++];
        }
        if(h.size() && s[i]=='y') i++; else { t.reset(); return false; }
        t.years = stoi(h);
        // months
        h.clear();
        while(isdigit(s[i])) {
            h+=s[i++];
        }
        if(h.size() && s[i]=='m') i++; else { t.reset(); return false; }
        t.months= stoi(h);
        // days
        h.clear();
        while(isdigit(s[i])) {
            h+=s[i++];
        }
        if(h.size() && s[i]=='d') i++; else { t.reset(); return false; }
        t.days= stoi(h);
        // hours
        h.clear();
        while(isdigit(s[i])) {
            h+=s[i++];
        }
        if(h.size() && s[i]=='h') i++; else { t.reset(); return false; }
        t.hours= stoi(h);
        // minutes
        h.clear();
        while(isdigit(s[i])) {
            h+=s[i++];
        }
        if(h.size() && s[i]=='m') i++; else { t.reset(); return false; }
        t.minutes= stoi(h);

        return true;
    } else {
        t.reset();
        return false;
    }
}

void TimeScope::toString(string& s) const
{
    std::ostringstream os;
    os << (int)years << "y" << (int)months << "m" << (int)days << "d" << (int)hours << "h" << (int)minutes << "m";
    s.assign(os.str());
}

} // m8r namespace
