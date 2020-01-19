#!/bin/bash
#
# MindForger thinking notebook
#
# Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>
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

# IMPROVE: make release version parameter to all build scripts and set it here

export SCRIPT_HOME=`pwd`

# Ubuntu
cp -vf ubuntu/ubuntu-launchpad-releases.sh /home/dvorka/p/mindforger/launchpad
cd /home/dvorka/p/mindforger/launchpad && ./ubuntu-launchpad-releases.sh

# tarball
cd ${SCRIPT_HOME}
cp -vf tarball/tarball-build.sh /home/dvorka/p/mindforger/release
cd /home/dvorka/p/mindforger/release && ./tarball-build.sh

# .deb
cd ${SCRIPT_HOME}
cp -vf debian/debian-make-deb.sh /home/dvorka/p/mindforger/debian
cd /home/dvorka/p/mindforger/debian && ./debian-make-deb.sh

# .rpm (SSH to running Fedora - copy there and back)

# eof
