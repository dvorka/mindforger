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
#     http://packaging.ubuntu.com/html/packaging-new-software.html
#   Debian maintainers guide:
#     https://www.debian.org/doc/manuals/maint-guide/index.en.html
#     https://www.debian.org/doc/manuals/debmake-doc/index.en.html
#   Debian formal doc:
#     https://www.debian.org/doc/debian-policy/
#

if [ -e "../.git" ]
then
  echo "This script must NOT be run from Git repository - run it e.g. from ~/p/mindforger/launchpad instead"
  exit 1
fi

# ############################################################################
# # Checkout MindForger from bazaar and make it #
# ############################################################################

function checkoutMindforger() {
    export HHSRC=$2
    # Create new branch: bzr init && bzr push lp:~ultradvorka/+junk/mindforger
    bzr checkout lp:~ultradvorka/+junk/mindforger

    # delete OLD files from Bazaar directory
    cd mindforger
    mv .bzr ..
    rm -rvf app build deps lib man LICENSE *.md
    mv ../.bzr .

    # copy NEW project files to Bazaar directory
    echo -e "\n# Get MF project files ############################"
    cp -rvf ${HHSRC}/* ${HHSRC}/*.*  .

    # prune MindForger project files: tests, *.o/... build files, ...
    echo -e "\n# MF project cleanup ########################################"
    rm -vrf ./.git ./app/mindforger ./build ./app/test ./lib/test
    find . -type f \( -name "*moc_*.cpp" -or -name "*.a" -or -name "*.o" -or -name "*.*~" -or -name ".gitignore" -or -name ".git" \) | while read F; do rm -vf $F; done

    cd ..
}

# ############################################################################
# # Create updated changelog #
# ############################################################################

function createChangelog() {
  export MYTS=`date "+%a, %d %b %Y %H:%M:%S"`
  echo "Changelog timestamp: ${MYTS}"
  echo "mindforger (${HHFULLVERSION}) ${UBUNTUVERSION}; urgency=low" > $1
  echo " " >> $1
  echo "  * ${HHBZRMSG}" >> $1
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
  cp -vrf ../${HH} .
  rm -rvf ${HH}/.bzr
  tar zcf ../${HH}.tgz ${HH}
  # .orig.tar.gz is required Debian convention
  cp -vf ../${HH}.tgz ../${HH}.orig.tar.gz
  cd ../${HH}
}

# ############################################################################
# # Release for *ONE* particular Ubuntu release #
# ############################################################################

function releaseForParticularUbuntuVersion() {
    export SCRIPTHOME=`pwd`
    export UBUNTUVERSION=$1
    export HHVERSION=$2
    export HHBZRMSG=$3
    export HHFULLVERSION=${HHVERSION}-0ubuntu1
    export HH=mindforger_${HHVERSION}
    export HHRELEASE=mindforger_${HHFULLVERSION}
    #export HHSRC=/home/dvorka/p/mindforger/git/mindforger
    export HHSRC=/home/dvorka/p/mindforger/launchpad/EXPERIMENTS/mindforger
    export NOW=`date +%Y-%m-%d--%H-%M-%S`
    export HHBUILD=mindforger-${NOW}

    # 1) clean up
    echo -e "\n# Cleanup ####################################################"
    rm -rvf *.*~ ./debian

    # 2) checkout MindForger to work directory
    echo -e "\n# Checkout MindForger from Bazaar ############################"
    mkdir ${HHBUILD}
    cd ${HHBUILD}
    checkoutMindforger ${HH} ${HHSRC}

    # 3) prepare Debian resources (changelog, descriptor, ...)
    echo -e "\n# Building deb ###############################################"
    cd mindforger && cp -rvf ${HHSRC}/build/ubuntu/debian .
    createChangelog ./debian/changelog

    # 4) Qt: generate makefile using qmake
    echo -e "\n# qmake ######################################################"
    cd ..
    mv mindforger ${HH}
    cd ${HH}
    # qt version MUST be specified as it CANNOT be configured by installing
    # qt5-default package: Debian control file does NOT allow virtual packages
    # like this qt5-default. Instead debian/rules file exports env var w/ Qt choice
    #   .pro file is also extended to have 'make install' target
    qmake -r mindforger.pro
    
    # 5) add new version to LOCAL Bazaar
    echo -e "\n# bazaar add & commit  #######################################"
    bzr add .
    # IMPORTANT: commit UPLOADs branch to server
    bzr commit -m "Update for ${HH} at ${NOW}."

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
    # build BINARY? deb package (us uc tells that no GPG signing is needed)
    bzr builddeb -- -us -uc
    # build SIGNED source deb package
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
    pbuilder-dist ${UBUNTUVERSION} build ${HHRELEASE}.dsc

    # 8) upload to Launchpad: push Bazaar and put changes
    echo -e "\n# bzr push .deb to Launchpad #################################"
    # from buildarea/ to ./dist
    cd ../${HH}
    echo "Before bzr push: " `pwd`
    bzr push lp:~ultradvorka/+junk/mindforger
    cd ..
    echo "Before dput push: " `pwd`
    # recently added /ppa to fix the path and package rejections
    # MF PPA w/ 64b build only
    dput ppa:ultradvorka/productivity ${HHRELEASE}_source.changes
    # HSTR PPA w/ 64b 32b and ARM builds
    #dput ppa:ultradvorka/ppa ${HHRELEASE}_source.changes
}

# ############################################################################
# # Main #
# ############################################################################

export ARG_BAZAAR_MSG="Experimental packaging."
export ARG_MAJOR_VERSION=0.7.
export ARG_MINOR_VERSION=6 # minor version is icremented for every Ubuntu version

# https://wiki.ubuntu.com/Releases
# old: precise quantal saucy precise utopic vivid wily yakkety
# current: trusty xenial artful beaver
for UBUNTU_VERSION in xenial
do
    echo "Releasing MF for Ubuntu version: ${UBUNTU_VERSION}"
    releaseForParticularUbuntuVersion ${UBUNTU_VERSION} ${ARG_MAJOR_VERSION}${ARG_MINOR_VERSION} "${ARG_BAZAAR_MSG}"
    MINOR=`expr $MINOR + 1`
done

# eof
