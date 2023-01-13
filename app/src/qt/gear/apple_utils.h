/*
 apple_utils.h     MindForger thinking notebook

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
#ifndef M8R_APPLE_UTILS_H
#define M8R_APPLE_UTILS_H

#include "../../lib/src/debug.h"

#include <QtWidgets>

/* Apple macOS utils:
 *
 *  - OS keyboard, naming, filesystem, process, ... conventions.
 *  - OS specifics.
 *
 * Keyboard modifiers (common):
 *
 * - Control (or Ctrl) ⌃
 * - Option (or Alt) ⌥
 * - Command (or Cmd ~ Win key) ⌘
 * - Shift ⇧
 * - Caps Lock ⇪
 *
 * Common keyboard shortcuts:
 *
 * - https://support.apple.com/en-us/HT201236
 *
 * Conditional compilation:
 *
 * - ifdef __APPLE__ / if defined(__APPLE__)
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
 * Keyboard unicode & entities (all):
 *
 * HTML Entity     GLYPH  NAME
 * &#63743;              Apple
 * &#8984;         ⌘     Command, Cmd, Clover, (formerly) Apple
 * &#8963;         ⌃      Control, Ctl, Ctrl
 * &#8997;         ⌥      Option, Opt, (Windows) Alt
 * &#8679;         ⇧      Shift
 * &#8682;         ⇪      Caps lock
 * &#9167;         ⏏      Eject
 * &#8617;         ↩      Return, Carriage Return
 * &#8629; &crarr; ↵      Return, Carriage Return
 * &#9166;         ⏎      Return, Carriage Return
 * &#8996;         ⌤      Enter
 * &#9003;         ⌫     Delete, Backspace
 * &#8998;         ⌦     Forward Delete
 * &#9099;         ⎋     Escape, Esc
 * &#8594; &rarr;  →      Right arrow
 * &#8592; &larr;  ←     Left arrow
 * &#8593; &uarr;  ↑      Up arrow
 * &#8595; &darr;  ↓      Down arrow
 * &#8670;         ⇞      Page Up, PgUp
 * &#8671;         ⇟      Page Down, PgDn
 * &#8598;         ↖     Home
 * &#8600;         ↘     End
 * &#8999;         ⌧     Clear
 * &#8677;         ⇥     Tab, Tab Right, Horizontal Tab
 * &#8676;         ⇤     Shift Tab, Tab Left, Back-tab
 * &#9250;         ␢      Space, Blank
 * &#9251;         **␣**  Space, Blank
 *
 * Platform headers:
 * /Library/Developer/CommandLineTools/SDKs/MacOSX10.15.sdk/
 *   System/Library/Frameworks/
 *     Foundation.framework/
 *       Headers/
 *         NSUserDefaults.h ... settings
 */
namespace m8r {

} // m8r namespace
#endif // M8R_APPLE_UTILS_H
