/*
 forget_aspect.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "time_scope_aspect.h"

namespace m8r {

using namespace std;

TimeScopeAspect::TimeScopeAspect()
    : timeScope{}
{
    timePoint = 0;
}

TimeScopeAspect::~TimeScopeAspect()
{
}

string TimeScopeAspect::getTimeScopeAsString()
{
    string result{};
    timeScope.toString(result);
    return std::move(result);
}

} // m8r namespace
