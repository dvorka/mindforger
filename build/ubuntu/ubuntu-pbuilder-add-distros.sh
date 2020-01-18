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

# https://wiki.ubuntu.com/Releases
# old: precise quantal saucy precise utopic vivid wily trusty (old GCC) yakkety artful cosmic
# current: trusty xenial bionic disco eoan
for DISTRO in disco eoan
do
    sudo pbuilder --create ${DISTRO}
    rm -vf ~/pbuilder/${DISTRO}-base.tgz
    cp /var/cache/pbuilder/base.tgz ~/pbuilder/${DISTRO}-base.tgz
done

# eof
