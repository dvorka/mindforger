#!/usr/bin/env bash
#
# MindForger thinking notebook
#
# Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# lupdate is run on app/app.pro - the app is the ONLY subproject w/ translatable
# strings (TRANSLATIONS) and running it on the top level, subdirs mindforger.pro
# only adds a 'no TS files specified' warning for the projects w/o catalogs
cd ../.. && lupdate app/app.pro && cd build

# eof
