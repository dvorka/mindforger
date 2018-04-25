#!/bin/bash
#
# MindForger knowledge management tool
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# Method: binary deb -alien-> -fixes-> rpm

# This script is available from http://www.mindforger.com/fedora/fedora-rpm-from-deb.sh
# to be easily available in VMs

export PRJNAME="mindforger_0.7.1-1_amd64"

# IMPORTANT: this script must be run as root

# a) everything works just fine
# alien -r mindforger_0.7.1-1_amd64.deb

# b) alien generates RPM that conflicts w/ other RPMs
#    https://www.electricmonk.nl/log/2017/02/23/how-to-solve-rpms-created-by-alien-having-file-conflicts/

alien -r -g -v "${DEBNAME}.deb"

# remove the following lines:
#%dir "/"
#%dir "/usr/bin/"
sed -i 's#%dir "/"##' ${PRJNAME}/${PRJNAME}-1.spec
sed -i 's#%dir "/usr/bin/"##' ${PRJNAME}/${PRJNAME}-1.spec

# recreate RPM
cd ${PRJNAME}
rpmbuild --target=noarch --buildroot /full/path/to/${PRJNAME}/ -bb cfgtrack-$(REL_VERSION)-2.spec

# end
