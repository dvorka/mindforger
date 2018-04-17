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

# ubuntu-make-distro.sh [ubuntu version] [mindforger version] [change description]
#   - ./ubuntu-make-distro.sh raring 1.9.5 "Fixed #25."
#   - this script to be run from a directory that allows cd ../git/mindforger

## https://wiki.ubuntu.com/Releases
export UBUNTUVERSION=$1
#export UBUNTUVERSION=precise
#export UBUNTUVERSION=quantal
#export UBUNTUVERSION=saucy
#export UBUNTUVERSION=trusty
#export UBUNTUVERSION=utopic
#export UBUNTUVERSION=xenial
#export UBUNTUVERSION=yakkety
export HHVERSION=$2
export HHBZRMSG=$3

export HHRDONLYGIT=/home/dvorka/p/mindforger/git/mindforger
export BUILDROOT=/tmp/mindforger-launchpad
export HHSRC=${BUILDROOT}/git/mindforger
export NOW=`date +%Y-%m-%d--%H-%M-%S`
export HH=mindforger_${HHVERSION}
export HHFULLVERSION=${HHVERSION}-0ubuntu1
export HHRELEASE=mindforger_${HHFULLVERSION}
export HHBUILD=${BUILDROOT}/mindforger-${NOW}

# Prepare source code copy and stripe it
function prepareSourceCode() {
  mkdir -p ${BUILDDIR}/git
  cp -vrf ${HHRDONLYGIT} ${BUILDDIR}/git || exit 2
  rm -vrf ${BUILDDIR}/git/mindforger/.git || exit 3
}

# Checkout MindForger from bazaar and make MindForger 
function checkoutMindforger() {
  # Create new branch: bzr init && bzr push lp:~ultradvorka/+junk/mindforger-package
  bzr checkout lp:~ultradvorka/+junk/mindforger-package
  #bzr checkout lp:~ultradvorka/mindforger/mindforger
  mv mindforger-package ${HH}
  cd ${HH}
  mv .bzr ..
  # TODO update
  rm -rvf debian dist man src LICENSE *.am *.md *.ac auto*.*
  mv ../.bzr .
  cp -rvf ${HHSRC}/* ${HHSRC}/*.*  .
  cd ..
  mv -v ${HH} mindforger
  # TODO remove - Makefile is there
  cd ./mindforger/dist && ./1-dist.sh && cd ../..
}

function createChangelog() {
  export MYTS=`date "+%a, %d %b %Y %H:%M:%S"`
  echo "Changelog timestamp: ${MYTS}"
  echo -e "mindforger (${HHFULLVERSION}) ${UBUNTUVERSION}; urgency=low" > $1
  echo -e "\n" >> $1
  echo -e "  * ${HHBZRMSG}" >> $1
  echo -e "\n" >> $1
  echo -e " -- Martin Dvorak (Dvorka) <martin.dvorak@mindforger.com>  ${MYTS} +0100" >> $1
  echo -e "\n" >> $1
}

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

# main() ###########################################################################

if [ -e "../../.git" ]
then
  echo "Error: This script MUST NOT be run from Git repository - run it e.g. from /tmp/mindforger-launchpad instead"
  exit 1
fi
if [ "$#" -ne 3 ]; then
  echo "Error: Illegal number of parameters: ${#} (must be 4)"
  exit 2
fi

prepareSourceCode

# STOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOP
exit 0





# TODO RM: rm -rvf *.*~ ./debian

mkdir ${HHBUILD} && cd ${HHBUILD} || exit 4
checkoutMindforger `pwd`

cd mindforger/dist

# build .deb for Ubuntu #############################################################

export SCRIPTHOME=`pwd`


echo -e "\n_ mindforger deb build  _______________________________________________\n"

rm -rvf ../debian
cp -rvf ${HHSRC}/debian ..

createChangelog ../debian/changelog

cd ../..
mv mindforger ${HH}
cd ${HH}
bzr add .
bzr commit -m "Update for ${HH} at ${NOW}."

createTarball

bzr builddeb -- -us -uc
bzr builddeb -S
cd ../build-area


echo -e "\n_ mindforger pbuilder-dist build  _______________________________________________\n"
# BEGIN: bug workaround - pbuilder's caches in /var and /home must be on same physical drive
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

# eof
