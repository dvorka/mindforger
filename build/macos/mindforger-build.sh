#!/usr/bin/env bash
#
# MindForger thinking notebook
#
# Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>
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

echo "Running CLEAN MindForger build..."

. ./env.sh

SCRIPT_HOME="$(pwd)"
export SCRIPT_HOME

# cmark-gfm
echo "cmark-gfm is built by qmake (no need for pre-build)"
# ./cmark-gfm-build.sh

# MindForger
cd "${SCRIPT_HOME}/../.." || exit 1
rm -rvf ${MF_MACOS_BUILD_DIR}/*.dmg ${MF_MACOS_BUILD_DIR}/mindforger ${MF_MACOS_BUILD_DIR}/mindforger.app
make clean
qmake -r mindforger.pro
make -j 7

echo "Binary can be found in GIT/mindforger/app/mindforger.app/Contents/MacOS"
cd "${SCRIPT_HOME}" || exit 1

# eof
