/*
 lang-utils.h     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_LANG_UTILS_H
#define M8R_LANG_UTILS_H

// casting to void is defined by C++ standard as a way to discard values/surpress
// compiler warnings:
//   "Any expression can be explicitly converted to type 'cv void'. Such expression value is discarded."
#define UNUSED_ARG(expr) do { (void)(expr); } while (0)

// ROUNDF(3.141592, 100) = 3.14
#define ROUND_FLOAT(f, c) (((float)((int)((f) * (c))) / (c)))

#endif // M8R_LANG_UTILS_H
