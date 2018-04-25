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
  echo -e "mindforger (${HHFULLVERSION}) ${UBUNTUVERSION}; urgency=low" > $1
  echo " " >> $1
  echo -e "  * ${HHBZRMSG}" >> $1
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
  cp -vrf ../${HH} .
  tar zcf ../${HH}.tgz ${HH}
  # .orig.tar.gz is required Debian convention
  cp -vf ../${HH}.tgz ../${HH}.orig.tar.gz
  cd ../${HH}
}

# ############################################################################
# # Build source and binary deb packages #
# ############################################################################

function buildDebPackage() {
    export SCRIPTHOME=`pwd`
    export HHVERSION=$1
    export HHBZRMSG=$2
    #export HHFULLVERSION=${HHVERSION}-1.0 # NMU upload
    export HHFULLVERSION=${HHVERSION}-1    # mantainer upload
    export HH=mindforger_${HHVERSION}
    export HHRELEASE=mindforger-${HHFULLVERSION}
    export HHSRC=/home/dvorka/p/mindforger/git/mindforger
    export NOW=`date +%Y-%m-%d--%H-%M-%S`
    export HHBUILD=mindforger-${NOW}
    export UBUNTUVERSION=unstable

    #
    # 1) create upstream tarball
    #
    # 1.1) get copy of project source
    echo -e "\n# Get MF project files ############################"
    mkdir -p ${HHBUILD}/${HH}
    cd ${HHBUILD}/${HH}
    # copy  project files to current directory
    cp -rvf ${HHSRC}/* ${HHSRC}/*.*  .
    
    # 1.2) prune MindForger project source: tests, *.o/... build files, ...
    echo -e "\n# MF project cleanup ########################################"
    rm -vrf ./.git ./app/mindforger ./build ./app/test ./lib/test
    find . -type f \( -name "*moc_*.cpp" -or -name "*.a" -or -name "*.o" -or -name "*.*~" -or -name ".gitignore" -or -name ".git" \) | while read F; do rm -vf $F; done

    # 1.3) generate makefiles (will be used later to build binary)
    qmake -d mindforger.pro
        
    # 1.4) create tar archive
    createTarball

    #
    # 2) create source deb
    #    
    # 2.1) add Debian control files
    cp -rvf ${HHSRC}/build/debian/debian  .
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
    mkdir -vf deb-binary
    cp -vf *.dsc *.changes *.deb deb-binary

    #
    # 3) create source deb
    #    
    # 3.1) build deb
    # build SIGNED source deb package
    dpkg-buildpackage --source
    mkdir -vf deb-source
    cp -vf *.dsc *.changes *.deb deb-binary    
}

# ############################################################################
# # Main #
# ############################################################################

export ARG_BAZAAR_MSG="Experimental Debian package."
export ARG_VERSION="0.7.1"

buildDebPackage ${ARG_VERSION} ${ARG_BAZAAR_MSG}

# eof
