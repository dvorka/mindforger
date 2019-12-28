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

# This script copies nice and safe Outlines from personal repository
# to demo repository used for creation of screenshots and videos.

export TIMESTAMP=`date +%Y-%m-%d--%H-%M-%S`
export REPONAME="mf-demo-repository-${TIMESTAMP}"

# IMPORTANT: copy MUST protect MODIFICATION time (for Os w/o MF metadata)

cd ~ && mkdir ${REPONAME}
cd ${REPONAME} && mkdir -v limbo
cp -vrfp ~/mf/stencils .
cp -vrfp ~/mf/memory .

rm -vrf ./memory/career
rm -vrf ./memory/family
rm -vrf ./memory/estate
rm -vrf ./memory/finance

rm -vrf ./memory/kitchen-b.md

# eof
