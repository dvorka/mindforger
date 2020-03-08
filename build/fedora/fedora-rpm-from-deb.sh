#!/bin/bash
#
# MindForger knowledge management tool
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# Method: binary deb -alien-> -fixes-> rpm

# This script is available from http://www.mindforger.com/fedora/fedora-rpm-from-deb.sh
# to be easily available in VMs

# This script must be run w/ ROOT priviledges (alien)

export MFVERSION="1.52.0"
export MFPRJNAME="mindforger-${MFVERSION}"
export AMD64NAME="mindforger_${MFVERSION}-1_amd64"
export I386NAME="mindforger_${MFVERSION}-1_i386"
export NOARCHNAME="mindforger_${MFVERSION}-1_noarch"

# IMPORTANT: this script must be run as root

# cleanup
rm -rvf ${MFPRJNAME} *.rpm

# a) everything works just fine
# alien -r mindforger_0.7.1-1_amd64.deb

# b) alien generates RPM that conflicts w/ other RPMs
#    https://www.electricmonk.nl/log/2017/02/23/how-to-solve-rpms-created-by-alien-having-file-conflicts/

alien -r -g -v "${AMD64NAME}.deb"

# remove the following lines:
#%dir "/"
#%dir "/usr/bin/"
sed -i 's#%dir "/"##' ${MFPRJNAME}/${MFPRJNAME}-2.spec
sed -i 's#%dir "/usr/bin/"##' ${MFPRJNAME}/${MFPRJNAME}-2.spec
sed -i 's#%dir "/usr/lib/"##' ${MFPRJNAME}/${MFPRJNAME}-2.spec

# recreate RPM
cd ${MFPRJNAME}
export MFRPMROOT=`pwd`

# bin build
# --target=x86_64 
# --target=i386
rpmbuild --target=x86_64 --buildroot ${MFRPMROOT}/ -bb ${MFPRJNAME}-2.spec
# noarch would be for SOURCE deb
#rpmbuild --target=noarch --buildroot ${MFRPMROOT}/ -bb ${MFPRJNAME}-2.spec

# sudo dnf install mindforger.rpm

# end
