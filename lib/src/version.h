/*
 version.h     MindForger thinking notebook

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
#ifndef M8R_VERSION_H
#define M8R_VERSION_H

namespace m8r {

// constexpr char const* MINDFORGER_VERSION = "18.1.5 alpha";
// C++ character const/constexpr concatenation patterns via templates are crazy > using C-style define

// versionning scheme: https://semver.org/
#define MINDFORGER_VERSION_MAJOR "1"
#define MINDFORGER_VERSION_MINOR "48"
#define MINDFORGER_VERSION_REVISION "0"

#define MINDFORGER_VERSION MINDFORGER_VERSION_MAJOR "." MINDFORGER_VERSION_MINOR "." MINDFORGER_VERSION_REVISION

}
#endif // M8R_VERSION_H
