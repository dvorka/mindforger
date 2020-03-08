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

# This script builds: upstream tarball > source deb > binary deb
#
# See:
#   Beginners guide:
#     http://packaging.ubuntu.com/html/packaging-new-software.html
#   Debian maintainers guide:
#     https://www.debian.org/doc/manuals/maint-guide/index.en.html
#     https://www.debian.org/doc/manuals/debmake-doc/index.en.html
#   Debian formal doc:
#     https://www.debian.org/doc/debian-policy/
#

# ############################################################################
# # Checkout MindForger from bazaar and make it #
# ############################################################################

function checkoutMindforger() {
    export MFSRC=$2
    # Create new branch: bzr init && bzr push lp:~ultradvorka/+junk/mindforger
    bzr checkout lp:~ultradvorka/+junk/mindforger

    # delete OLD files from Bazaar directory
    cd mindforger
    mv .bzr ..
    rm -rvf app build deps lib man LICENSE *.md
    mv ../.bzr .

    # copy NEW project files to Bazaar directory
    echo -e "\n# Get MF project files ############################"
    cp -rvf ${MFSRC}/* ${MFSRC}/*.*  .

    # prune MindForger project files: tests, *.o/... build files, ...
    echo -e "\n# MF project cleanup ########################################"
    rm -vrf ./.git ./app/mindforger ./build ./app/test ./lib/test
    # IMPROVE: static libraries lib*.a are NOT deleted to keep cmark-gfm dependency libs
    find . -type f \( -name "*moc_*.cpp" -or -name "*.o" -or -name "*.*~" -or -name ".gitignore" -or -name ".git" \) | while read F; do rm -vf $F; done

    cd ..
}

# ############################################################################
# # Create updated changelog #
# ############################################################################

function createChangelog() {
  export MYTS=`date "+%a, %d %b %Y %H:%M:%S"`
  echo "Changelog timestamp: ${MYTS}"
  echo "mindforger (${MFFULLVERSION}) ${UBUNTUVERSION}; urgency=low" > $1
  echo " " >> $1
  echo "  * ${MFBZRMSG}" >> $1
  echo " " >> $1
  echo " -- Martin Dvorak (Dvorka) <martin.dvorak@mindforger.com>  ${MYTS} +0100" >> $1
  echo "" >> $1
}

# ############################################################################
# # Create upstream tarball #
# ############################################################################

function createTarball() {
  cd ..
  mkdir work
  cd work
  cp -vrf ../${MF} .
  rm -rvf ${MF}/.bzr
  tar zcf ../${MF}.tgz ${MF}
  # .orig.tar.gz is required Debian convention
  cp -vf ../${MF}.tgz ../${MF}.orig.tar.gz
  cd ../${MF}
}

# ############################################################################
# # Release for *ONE* particular Ubuntu version #
# ############################################################################

function releaseForParticularUbuntuVersion() {
    export SCRIPTHOME=`pwd`
    export UBUNTUVERSION=${1}
    export MFVERSION=${2}
    export MFBZRMSG=${3}
    export MFFULLVERSION=${MFVERSION}-0ubuntu1
    export MF=mindforger_${MFVERSION}
    export MFRELEASE=mindforger_${MFFULLVERSION}
    export MFSRC=/home/dvorka/p/mindforger/git/mindforger
    export NOW=`date +%Y-%m-%d--%H-%M-%S`
    export MFBUILD=mindforger-${NOW}

    # DO_MF_DEBUG is disabled by default now
    #if ! grep -q "//#define DO_MF_DEBUG" "${MFSRC}/lib/src/debug.h"
    #then
    #	echo "This script must NOT be run if debug code is enabled - disable DO_MF_DEBUG first"
    #	exit 1
    #fi
    
    # 1) clean up
    echo -e "\n# Cleanup ####################################################"
    rm -rvf *.*~ ./debian

    # 2) checkout MindForger to work directory
    echo -e "\n# Checkout MindForger from Bazaar ############################"
    mkdir ${MFBUILD}
    cd ${MFBUILD}
    checkoutMindforger ${MF} ${MFSRC}

    # 3) prepare Debian resources (changelog, descriptor, ...)
    echo -e "\n# Building deb ###############################################"
    cd mindforger && cp -rvf ${MFSRC}/build/ubuntu/debian .
    createChangelog ./debian/changelog

    echo "4.1) build MF dependencies"
    rm -rvf deps/cmark-gfm/build
    cd deps/cmark-gfm && mkdir -v build && cd build && cmake -DCMARK_TESTS=OFF -DCMARK_SHARED=OFF .. && cmake --build . && cd ../../..
        
    echo "4.2) Qt: generate makefile using qmake"
    echo -e "\n# qmake ######################################################"
    cd ..
    mv mindforger ${MF}
    cd ${MF}
    # qt version MUST be specified as it CANNOT be configured by installing
    # qt5-default package: Debian control file does NOT allow virtual packages
    # like this qt5-default. Instead debian/rules file exports env var w/ Qt choice
    #   .pro file is also extended to have 'make install' target
    qmake -r mindforger.pro
    
    # 5) add new version to LOCAL Bazaar
    echo -e "\n# bazaar add & commit  #######################################"
    bzr add .
    # IMPORTANT: commit UPLOADs branch to server
    bzr commit -m "Update for ${MF} at ${NOW}."

    # 5) create tarball ~ .tgz archive w/ source and required Debian cfg files
    createTarball

    # x) start GPG agent if it's NOT running
    if [ -e "${HOME}/.gnupg/S.gpg-agent" ]
    then
	echo "OK: GPG agent running."
    else
	gpg-agent --daemon
    fi    
    
    # 6) build debs
    echo -e "\n# source & binary debs  ######################################"
    # OPTIONAL: build .deb package (us uc tells that no GPG signing is needed)
    # bzr builddeb -- -us -uc -j7
    # build SIGNED source .deb package
    bzr builddeb --source

    # 7) build binary from source deb on CLEAN system - no deps installed
    echo -e "\n# clean build ~ pbuilder  ####################################"
    cd ../build-area    
    # IMPORTANT pbuilder's caches in /var and /home MUST be on same physical drive
    # BEGIN
    export PBUILDFOLDER=/tmp/mindforger-tmp
    rm -rvf ${PBUILDFOLDER}
    mkdir -p ${PBUILDFOLDER}
    cp -rvf ~/pbuilder/*.tgz ${PBUILDFOLDER}
    # END
    pbuilder-dist ${UBUNTUVERSION} build ${MFRELEASE}.dsc
    
    # 8) upload to Launchpad: push Bazaar and put changes
    echo -e "\n# bzr push .deb to Launchpad #################################"
    # from buildarea/ to ./dist
    cd ../${MF}
    echo "Before bzr push: " `pwd`
    bzr push lp:~ultradvorka/+junk/mindforger
    cd ..
    echo "Before dput push: " `pwd`
    # recently added /ppa to fix the path and package rejections
    # MF PPA w/ 64b build only
    dput ppa:ultradvorka/productivity ${MFRELEASE}_source.changes
    # SKIP: HSTR PPA w/ 64b 32b and ARM builds
    #dput ppa:ultradvorka/ppa ${MFRELEASE}_source.changes
}

# ############################################################################
# # Main #
# ############################################################################

if [ -e "../../.git" ]
then
    echo "This script must NOT be run from Git repository - run it e.g. from ~/p/mindforger/launchpad instead"
    exit 1
fi

export ARG_MAJOR_VERSION=1.52.
export ARG_MINOR_VERSION=0 # minor version is incremented for every Ubuntu version
export ARG_BAZAAR_MSG="MindForger ${ARG_MAJOR_VERSION}${ARG_MINOR_VERSION} release."

# https://wiki.ubuntu.com/Releases
# old: precise quantal saucy precise utopic vivid wily trusty (old GCC) yakkety artful cosmic
# current: (trusty) xenial bionic disco eoan
for UBUNTU_VERSION in xenial bionic disco eoan
do
    echo "Releasing MF for Ubuntu version: ${UBUNTU_VERSION}"
    releaseForParticularUbuntuVersion ${UBUNTU_VERSION} ${ARG_MAJOR_VERSION}${ARG_MINOR_VERSION} "${ARG_BAZAAR_MSG}"
    ARG_MINOR_VERSION=`expr $ARG_MINOR_VERSION + 1`
done

# eof
