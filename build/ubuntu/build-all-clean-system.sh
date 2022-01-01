#!/usr/bin/env bash
#
# MindForger knowledge management tool
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# This script does full project bootstrap on clean Ubuntu system.
# Having MindForger Git repository clone, it installs all dependencies
# and builds the project.

SCRIPT_HOME=`pwd`

# Git
cd ../..
git submodule init
git submodule update

# deps install
sudo apt-get install build-essential zlib1g-dev libhunspell-dev libqt5webkit5-dev qttools5-dev-tools qt5-default ccache

# deps build
cd  ${SCRIPT_HOME}/../../deps/cmark-gfm
mkdir build && cd build
cmake -DCMARK_TESTS=OFF -DCMARK_SHARED=OFF ..
cmake --build .

# build
cd ${SCRIPT_HOME}/../..
qmake -r mindforger.pro
make -j 6

# eof
