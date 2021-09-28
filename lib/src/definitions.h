/*
 definitions.h     MindForger type, include, ... definitions

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
#ifndef M8R_CONFIG_H
#define M8R_CONFIG_H

#ifdef _WIN32
  #include <Windows.h>
  //typedef uint8_t   u_int8_t;
  //typedef uint16_t  u_int16_t;
  //typedef uint32_t  u_int32_t;
  typedef short int   u_int8_t;
  typedef int u_int16_t;
  typedef long int  u_int32_t;
  typedef unsigned int uint;
  #include "../../../deps/dirent/dirent.h"
  #include <io.h>
  #include <direct.h>
#else
  #include <dirent.h>
  #include <sys/dir.h>
  #include <unistd.h>
#endif // _WIN32

#endif // M8R_CONFIG_H
