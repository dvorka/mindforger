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

# Method:
#   upstream tarball > source deb > binary deb
#
# See:
#   Beginners guide:
#     https://wiki.debian.org/IntroDebianPackaging
#   Debian maintainers guide:
#     https://www.debian.org/doc/manuals/maint-guide/index.en.html
#     https://www.debian.org/doc/manuals/debmake-doc/index.en.html
#   Debian formal doc:
#     https://www.debian.org/doc/debian-policy/
#

if [ -e "../.git" ]
then
  echo "This script must NOT be run from Git repository - run it e.g. from ~/p/mindforger/debian instead"
  exit 1
fi

# ############################################################################
# # Create updated changelog #
# ############################################################################

function createChangelog() {
  # Debian tooling changelog hints: 
  # - signature line MUST have one whitespace prefix
  # - signature line MUST have double space between email and timestamp
  # - traling lines must have exactly one space

  export MYTS=`date "+%a, %d %b %Y %H:%M:%S"`
  echo "Changelog timestamp: ${MYTS}"
  echo -e "mindforger (${MFFULLVERSION}) ${UBUNTUVERSION}; urgency=low" > $1
  echo " " >> $1
  echo -e "  * ${MFBZRMSG}" >> $1
  echo " " >> $1
  echo " -- Martin Dvorak (Dvorka) <martin.dvorak@mindforger.com>  ${MYTS} +0100" >> $1
}

# ############################################################################
# # Create upstream tarball #
# ############################################################################

function createTarball() {
  cd ..
  mkdir work
  cd work
  cp -vrf ../${MF} .
  tar zcf ../${MF}.tgz ${MF}
  # .orig.tar.gz is required Debian convention
  cp -vf ../${MF}.tgz ../${MF}.orig.tar.gz
  cd ../${MF}
}

# ############################################################################
# # Build source and binary deb packages #
# ############################################################################

function buildDebPackage() {
    export SCRIPTHOME=`pwd`
    export MFVERSION=$1
    export MFBZRMSG=$2
    #export MFFULLVERSION=${MFVERSION}-1.0 # NMU upload
    export MFFULLVERSION=${MFVERSION}-1    # mantainer upload
    export MF=mindforger_${MFVERSION}
    export MFRELEASE=mindforger-${MFFULLVERSION}
    export MFSRC="/home/dvorka/p/mindforger/git/mindforger"
    export NOW=`date +%Y-%m-%d--%H-%M-%S`
    export MFBUILD=mindforger-${NOW}
    export UBUNTUVERSION=unstable

    # DO_M8R_DEBUG is disabled by default now
    #if ! grep -q "//#define DO_M8R_DEBUG" "${MFSRC}/lib/src/debug.h"
    #then
    #	echo "This script must NOT be run if debug code is enabled - disable DO_M8R_DEBUG first"
    #	exit 1
    #fi
    
    #
    # 1) create upstream tarball
    #
    # 1.1) get copy of project source
    echo -e "\n# Get MF project files ############################"
    mkdir -p ${MFBUILD}/${MF}
    cd ${MFBUILD}/${MF}
    # copy  project files to current directory
    cp -rvf ${MFSRC}/* ${MFSRC}/*.*  .
    
    # 1.2) prune MindForger project source: tests, *.o/... build files, ...
    echo -e "\n# MF project cleanup ########################################"
    rm -vrf ./.git ./app/mindforger ./build ./app/test ./lib/test
    # IMPROVE: static libraries lib*.a are NOT deleted to keep cmark-gfm dependency libs
    find . -type f \( -name "*moc_*.cpp" -or -name "*.o" -or -name "*.*~" -or -name ".gitignore" -or -name ".git" \) | while read F; do rm -vf $F; done

    # 1.3) generate makefiles (will be used later to build binary)
    qmake -r mindforger.pro
        
    # 1.4) create tar archive
    createTarball

    #
    # 2) create source deb
    #    
    # 2.1) add Debian control files
    cp -rvf ${MFSRC}/build/debian/debian  .
    createChangelog ./debian/changelog

    # x) start GPG agent if it's NOT running
    if [ -e "${HOME}/.gnupg/S.gpg-agent" ]
    then
	echo "OK: GPG agent running."
    else
	gpg-agent --daemon
    fi    

    DEBEMAIL="martin.dvorak@mindforger.com"
    DEBFULLNAME="Martin Dvorak"
    export DEBEMAIL DEBFULLNAME

    # 2.2) build binary deb
    # https://www.debian.org/doc/manuals/maint-guide/build.en.html
    dpkg-buildpackage -us -uc
    pushd .. && mkdir -v deb-binary && cp -vf *.dsc *.changes *.deb deb-binary && popd

    #
    # 3) create source deb
    #    
    # 3.1) build deb
    # build source deb package
    dpkg-buildpackage -S
    pushd .. && mkdir -v deb-source && cp -vf *.dsc *.changes *.deb deb-source && popd
}

# ############################################################################
# # Main #
# ############################################################################

export ARG_VERSION="1.52.0"
export ARG_BAZAAR_MSG="MindForger ${ARG_VERSION} release."

# Debian releases: https://www.debian.org/releases/
#   6/7/8/9/10: squeeze wheezy jessie stretch buster

buildDebPackage ${ARG_VERSION} ${ARG_BAZAAR_MSG}

# eof
