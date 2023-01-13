/*
 datetime_utils.h     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_DATETIME_UTILS_H_
#define M8R_DATETIME_UTILS_H_

#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>
#ifdef _WIN32
#include "../../../deps/strptime/strptime.h"
#endif //_WIN32
#include "string_utils.h"

namespace m8r {

/*
 * Helper functions provided by this library run in GMTIME.
 *
 * MindForger internals and persistence runs on GMTIME to ensure
 * smooth data exchange (machines in different time zones) and
 * to avoid conversions.
 *
 * It is presumed that user facing/presentation tier (e.g. UI)
 * converts GMTIME to local time.
 */

time_t datetimeNow();
time_t datetimeSeconds(struct tm* datetime);
struct tm *datetimeFrom(const char* s);
struct tm *datetimeFrom(const char* s, struct tm* datetime);
char *datetimeTo(const struct tm *datetime, char* result);
std::string datetimeToString(const time_t ts);
std::string datetimeToPrettyHtml(const time_t ts);
std::string datetimeToPrettyHtml(const time_t* seconds);

} // m8r namespace

#endif /* M8R_DATETIME_UTILS_H_ */
