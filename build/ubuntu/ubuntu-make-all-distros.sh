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

# Launchpad release checklist:
# - update version in source code
# - update release message in this file
# - update version in this file
# - check target Ubuntu distributions in this file
# - check minor version in this file
# - run this script from ~/p/mindforger/launchpad

export HHBZRMSG="Initial commit."
export VERSION=1.0.
export MINOR=1
export BUILDDIR=/tmp/mindforger-launchpad

# Prepare build copy in temp directory.
function prepareBuild() {
    rm -vrf ${BUILDDIR}
    mkdir -p ${BUILDDIR} && ./ubuntu-sync-scripts.sh || exit 1
}

# precise quantal saucy precise utopic / trusty vivid wily xenial yakkety 
for DISTRO in trusty
do
  prepareBuild
  cd ${BUILDDIR} || exit 1
  ./ubuntu-make-distro.sh ${DISTRO} ${VERSION}${MINOR} "${HHBZRMSG}"
  MINOR=`expr $MINOR + 1`
done

# eof
