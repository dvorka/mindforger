/*
 apple_utils.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_APPLE_UTILS_H
#define M8R_APPLE_UTILS_H

#include "../../lib/src/debug.h"

#include <QtWidgets>

/* Apple macOS utils:
 *
 *  - OS keyboard, naming, filesystem, process, ... conventions.
 *  - OS specifics.
 *
 * Keyboard modifiers:
 *
 * - Control (or Ctrl) ⌃
 * - Option (or Alt) ⌥
 * - Command (or Cmd ~ Win key) ⌘
 * - Shift ⇧
 * - Caps Lock ⇪
 * - Fn
 *
 * Common keyboard shortcuts:
 *
 * - https://support.apple.com/en-us/HT201236
 *
 * Conditional compilation:
 *
 * - ifdef __APPLE__ / if defined(__APPLE__)
 *
 *
 *
 * Qt
 *
 * enum Qt::Modifier:
 *   This enum provides shorter names for the keyboard modifier
 *   keys supported by Qt. On macOS, the CTRL value corresponds
 *   to the Command keys on the keyboard, and the META value
 *   corresponds to the Control keys.
 *
 * Qt KEY_CTRL -> macOS command (CMD)
 * Qt KEY_META -> macOS control (CTRL)
 * Qt KEY_ALT  -> macos option  (OPTION)
 *
 */
namespace m8r {

} // m8r namespace
#endif // M8R_APPLE_UTILS_H
