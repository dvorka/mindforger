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

# ubuntu-make-distro.sh [ubuntu version] [mindforger version] [change description]
#   - ./ubuntu-make-distro.sh raring 1.9.5 "Fixed #25."
#   - this script to be run from ./launchpad or other dir that allows:
#     ../git/mindforger

if [ -e "../.git" ]
then
  echo "This script must NOT be run from Git repository - run it e.g. from ~/p/mindforger/launchpad instead"
  exit 1
fi

# ############################################################################
# # UNUSED Init MindForger #
# ############################################################################

function initMindforger() {
  cp -rvf ${HHSRC} .
  cd ./mindforger/dist && ./1-dist.sh && rm -vrf ../debian && cd ../..
  cd ${1}
  mv mindforger ${HH}
  tar zcf ${HH}.tgz ${HH}
  rm -rvf ${HH}
  bzr dh-make -v mindforger ${HHVERSION} ${HH}.tgz
}

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
    cp -rvf ${HHSRC}/* ${HHSRC}/*.*  .
    cd ..

    # prune MindForger project files: tests, *.o/... build files, ...
    # TODO
    # TODO
    # TODO
}

# ############################################################################
# # Build .deb for Ubuntu #
# ############################################################################

function createChangelog() {
  export MYTS=`date "+%a, %d %b %Y %H:%M:%S"`
  echo "Changelog timestamp: ${MYTS}"
  echo "mindforger (${HHFULLVERSION}) ${UBUNTUVERSION}; urgency=low" > $1
  echo "" >> $1
  echo "  * ${HHBZRMSG}" >> $1
  echo "" >> $1
  echo " -- Martin Dvorak (Dvorka) <martin.dvorak@mindforger.com>  ${MYTS} +0100" >> $1
  echo "" >> $1
}

# ############################################################################
# # Create tarball #
# ############################################################################

function createTarball() {
  cd ..
  mkdir work
  cd work
  cp -vrf ../${HH} .
  rm -rvf ${HH}/.bzr
  tar zcf ../${HH}.tgz ${HH}
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
    cd ..
    mv mindforger ${HH}
    cd ${HH}
    qmake mindforger.pro
    
    # 5) add new version to LOCAL Bazaar
    bzr add .
    # TODO
    # TODO
    # TODO
    # IMPORTANT: commit UPLOADs branch to server
    #bzr commit -m "Update for ${HH} at ${NOW}."

    # 5) create tarball ~ .tgz archive w/ source and required Debian cfg files
    createTarball
    
    # 6) build deb
    bzr builddeb -- -us -uc
    # build source package
    bzr builddeb --source
    exit 0
    cd ../build-area



    # TODO
    # TODO
    # TODO


    
    echo -e "\n_ mindforger pbuilder-dist build  _______________________________________________\n"
    # IMPORTANT pbuilder's caches in /var and /home MUST be on same physical drive
    export PBUILDFOLDER=/tmp/mindforger-tmp
    rm -rvf ${PBUILDFOLDER}
    mkdir -p ${PBUILDFOLDER}
    cp -rvf ~/pbuilder/*.tgz ${PBUILDFOLDER}
    # END
    pbuilder-dist ${UBUNTUVERSION} build ${HHRELEASE}.dsc

    # push .deb to Launchpad ########################################################

    # from buildarea/ to ./dist
    cd ../${HH}

    echo "Before bzr push: " `pwd`
    bzr push lp:~ultradvorka/+junk/mindforger-package
    #bzr push lp:~ultradvorka/mindforger/mindforger
    cd ..
    echo "Before dput push: " `pwd`
    # recently added /ppa to fix the path and package rejections
    dput ppa:ultradvorka/ppa ${HHRELEASE}_source.changes
    
}

# ############################################################################
# # Main #
# ############################################################################

export ARG_BAZAAR_MSG="Experimental packaging."
export ARG_MAJOR_VERSION=0.7.
export ARG_MINOR_VERSION=1 # minor version is icremented for every Ubuntu version

# Ubuntu version: precise quantal saucy precise utopic / trusty vivid wily xenial yakkety artful
# https://wiki.ubuntu.com/Releases
for UBUNTU_VERSION in xenial
do
    echo "Releasing MF for Ubuntu version: ${UBUNTU_VERSION}"
    releaseForParticularUbuntuVersion ${UBUNTU_VERSION} ${ARG_MAJOR_VERSION}${ARG_MINOR_VERSION} "${ARG_BAZAAR_MSG}"
    MINOR=`expr $MINOR + 1`
done

# eof
