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
    echo -e "\n# Checkout MindForger from Bazaar ############################"
    mkdir ${HHBUILD}
    cd ${HHBUILD}
    # copy  project files to current directory
    cp -rvf ${HHSRC}/* ${HHSRC}/*.*  .
    
    # 1.2) prune MindForger project source: tests, *.o/... build files, ...
    echo -e "\n# Git repo cleanup ########################################"
    rm -vrf ./.git ./app/mindforger ./build
    find . -type f \( -name "*.a" -or -name "*.o" -or -name "*.*~" -or -name ".gitignore" \) | while read F; do rm -vf $F; done

    # 1.?) generate makefiles (will be used later to build binary)
    
    # 1.3) create tar archive
    cd ..
    createTarball

    #
    # 2) create source deb
    #
    
    # 2.1) add Debian control files
    # 2.2) build deb

    #
    # 3) create binary deb
    #
    
    # 3.1) build deb


    
    # 1) clean up
    echo -e "\n# Cleanup ####################################################"
    rm -rvf *.*~ ./debian ./bin
    # TODO clean all extra directories - find copy

    


    
    cd ./hstr/dist && ./1-dist.sh && rm -vrf ../tests && cd ../..
    cd `pwd`
    mv mindforger ${HH}
    tar zcf ${HH}.tgz ${HH}
    rm -rvf ${HH}

    # bzr dh-make -v packagename version tarball
    bzr dh_make -v hstr ${HHVERSION} ${HH}.tgz

    # rewrite to native dh_make:
    #dh_make --single --createorig 

    rm -vf *.orig.tar.gz

    cd hstr/dist





    
    rm -rvf ../debian
    cp -rvf ${HHSRC}/debian ..

    createChangelog ../debian/changelog
    cp -vf debian/rules ../debian/rules
    cp -vf debian/hstr.dirs ../debian/hstr.dirs
    cp -vf debian/watch ../debian/watch

    # cleanup
    rm -vrf ../dist ../bin ../doc ../pad.xml

    cd ../..
    mv hstr ${HH}
    cd ${HH}

    createTarball

    # 6) build debs
    echo -e "\n# source & binary debs  ######################################"
    # build BINARY? deb package (us uc tells that no GPG signing is needed)
    debuild -us -uc
    # build SIGNED source deb package
    debuild --source
}

# ############################################################################
# # Main #
# ############################################################################

export ARG_BAZAAR_MSG="Experimental Debian package."
export ARG_VERSION="0.7.1"

buildDebPackage ${ARG_VERSION} ${ARG_BAZAAR_MSG}

# eof
