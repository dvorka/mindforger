#!/bin/bash
#
# MindForger thinking notebook
#
# Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>
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

# make project w/ QtWebEngine instead QtWebKit

echo "IMPORTANT: build mindforger/deps/discount before running this script!"

echo "Clean..."
rm -vrf ../../app/mindforger.app ../../app/mindforger.dmg
cd ../..
make clean

echo "Build..."
qmake -r -config release mindforger.pro
make -j 8

echo "Package..."
# In order to link QWebEngine correctly, macdeployqt must be run as follows:
#   macdeployqt <TARGET>.app -executable=<TARGET>.app/Contents/MacOS/<TARGET>
# and non-brew (qt.io) macdeployqt MUST be used (specify path and/or put macdeployqt to path):
#   export MACDEPLOY="/Users/dvorka/Qt/5.11.0/clang_64/bin/macdeployqt"
export MACDEPLOY="macdeployqt"
cd app && ${MACDEPLOY} mindforger.app -executable=mindforger.app/Contents/MacOS/mindforger -dmg -always-overwrite

# eof
