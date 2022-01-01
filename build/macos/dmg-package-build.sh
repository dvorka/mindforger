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

. ./env.sh

rm -vf ${MF_MACOS_BUILD_DIR}/*.dmg

function packageMindForger {
    echo "Packaging MindForger as Disk iMaGe .dmg ..."
    # in order to link QWebEngine correctly, macdeployqt must be run as follows:
    #   macdeployqt <TARGET>.app -executable=<TARGET>.app/Contents/MacOS/<TARGET>
    # and non-brew (qt.io) macdeployqt MUST be used (specify path and/or put macdeployqt to path):
    cd "${MF_MACOS_BUILD_DIR}" && ${MACDEPLOY} mindforger.app -executable=mindforger.app/Contents/MacOS/mindforger -dmg -always-overwrite

    export DMG_TIMESTAMP=`date +%Y%m%d.%H%M%S`
    export DMG_FILENAME="mindforger-1.53.0-${DMG_TIMESTAMP}-intel.dmg"
    
    echo "Find .dmg package in $(pwd)/${DMG_FILENAME} ..."
    mv -v "mindforger.dmg" "${DMG_FILENAME}"
    ls -l *.dmg
}

packageMindForger

# eof
