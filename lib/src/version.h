/*
 version.h     MindForger thinking notebook

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
#ifndef M8R_VERSION_H
#define M8R_VERSION_H

#include "debug.h"

namespace m8r {

// versionning scheme: https://semver.org/
#include "app_info.h"

#ifdef DO_MF_DEBUG
  #define MINDFORGER_NIGHTLY " (" __DATE__ " " __TIME__ ")"
#else
  #define MINDFORGER_NIGHTLY ""
#endif

#define MINDFORGER_VERSION MINDFORGER_VERSION_MAJOR "." MINDFORGER_VERSION_MINOR "." MINDFORGER_VERSION_REVISION MINDFORGER_NIGHTLY

}
#endif // M8R_VERSION_H
