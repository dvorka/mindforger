#!/usr/bin/env bash
#
# MindForger thinking notebook
#
# Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>
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

export MINDFORGER_VERSION="2.0.0"

# aligned with Ubuntu 18.4
export QT_VERSION="5.9.9"
# MindForger 1.52.0 released for macOS Qt version
# export QT_VERSION="5.11.0"
# BUG: unable to set security - images are NOT loaded with Qt 5.15.2
# export QT_VERSION="5.15.2"


export PATH="/Users/dvorka/Qt/${QT_VERSION}/clang_64/bin":${PATH}

# when built using QtCreator
# export MF_MACOS_BUILD_DIR="/Users/dvorka/p/mindforger/git/build-mindforger-Desktop_Qt_5_9_9_clang_64bit-Debug/app"
# when built using .sh scripts
export MF_MACOS_BUILD_DIR="/Users/dvorka/p/mindforger/git/mindforger/app"

# export MACDEPLOY="macdeployqt"
export MACDEPLOY="/Users/dvorka/Qt/${QT_VERSION}/clang_64/bin/macdeployqt"

# eof
