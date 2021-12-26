#!/bin/bash
#
# MindForger knowledge management tool
#
# Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>
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
# # Checkout MindForger from bazaar, copy over new sources, clean result #
# ############################################################################

function checkoutMindforger {
    # use `bzr` to manually check and work with Bazaar repository:
    #   - bzr        ... get help
    #   - bzr status 
    #   - bzr commit ... commints and PUSHES changes to server
    
    export MFSRC=$2
    # create new branch: bzr init && bzr push lp:~ultradvorka/+junk/mindforger
    bzr checkout lp:~ultradvorka/+junk/mindforger

    # delete OLD files from Bazaar directory
    cd mindforger
    mv .bzr ..
    rm -rvf app build deps lib man LICENSE *.md
    mv ../.bzr .

    # copy NEW project files to Bazaar directory
    echo -e "\n# copy over new MindForger files ############################"
    cp -rvf ${MFSRC}/* ${MFSRC}/*.*  .

    # prune MindForger project files: tests, *.o/... build files, ...
    echo -e "\n# CLEANUP development and build artifacts ###################"
    rm -vrf ./.git ./.qmake.stash ./app/mindforger ./build ./app/test ./lib/test
    # TODO purge: rm -vrf ./deps/cmark-gfm/.github
    # TODO purge: rm -vrf ./deps/mitie
    rm -vrf ./deps/cmark-gfm/.github
    rm -vrf ./deps/mitie
    rm -vrf ./lib/.qmake.stash ./lib/lib.pro.user ./lib/src/mindforger-lib-unit-tests
    # IMPROVE: static libraries lib*.a are NOT deleted to keep cmark-gfm dependency libs
    find . -type f \( -name "*moc_*.cpp" -or -name "*.o" -or -name "*.*~" -or -name ".gitignore" -or -name ".git" \) | while read F; do rm -vf $F; done
    
    cd ..
}

# ############################################################################
# # Create updated changelog #
# ############################################################################

function createChangelog {
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
# # Create upstream tarball in work/ #
# ############################################################################

function createTarball {
  cd ..
  mkdir work
  cd work
  cp -vrf ../${MF} .
  rm -rvf ${MF}/.bzr
  tar zcf ../${MF}.tgz ${MF}
  # .orig.tar.gz is required Debian convention
  cp -vf ../${MF}.tgz ../${MF}.orig.tar.gz
  cd ../${MF}

  echo -e "\nMindForger TARBALL archives created using work/:\n  mindforger_1.53.4.orig.tar.gz\n  mindforger_1.53.4.tgz"
}

# ############################################################################
# # Patch qmake location (if needed) #
# ############################################################################

function patchQmakePathInMakefile {
    MAKEFILE_PATH=${1}
    echo "Patching qmake in Makefile: ${MAKEFILE_PATH}"
    cat ${MAKEFILE_PATH} | while IFS= read L
    do
	if [[ "${L}" = "QMAKE         = /usr/lib/qt5/bin/qmake" ]]
	then
	    echo "QMAKE         = /usr/bin/qmake"
	else
	    echo "${L}"
	fi
    done > "${MAKEFILE_PATH}.new"
    mv "${MAKEFILE_PATH}.new" ${MAKEFILE_PATH}
}

# ############################################################################
# # Release for *ONE* particular Ubuntu version #
# ############################################################################

function releaseForParticularUbuntuVersion {
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
    
    # 1) clean up
    echo -e "\n# Cleanup ####################################################"
    rm -rvf *.*~ ./debian
    
    # 2) checkout MindForger from Launchpad's bazaar to work directory (will be needed to build SOURCE .deb package)
    echo -e "\n# Checkout MindForger from Launchpad's Bazaar ################"
    mkdir ${MFBUILD}
    cd ${MFBUILD}
    checkoutMindforger ${MF} ${MFSRC}

    # 3) update Debian control files (changelog, descriptor, ...) in bzr clone
    echo -e "\n# Building deb ###############################################"
    cd mindforger && cp -rvf ${MFSRC}/build/ubuntu/debian .
    createChangelog ./debian/changelog

    # 4) prepare MindForger dependencies/libraries
    echo -e "\n---------------------------------------------------------------"
    echo "4.1) build MF dependencies/libs: cmark-gfm, ..."
    rm -rvf deps/cmark-gfm/build
    cd deps/cmark-gfm && mkdir -v build && cd build && cmake -DCMARK_TESTS=OFF -DCMARK_SHARED=OFF .. && cmake --build . && cd ../../..
        
    echo -e "\n---------------------------------------------------------------"
    echo "4.2) Qt: generate Makefiles using qmake"
    cd ..
    mv mindforger ${MF}
    cd ${MF}
    # qt version MUST be specified as it CANNOT be configured by installing
    # qt5-default package: Debian control file does NOT allow virtual packages
    # like this qt5-default. Instead debian/rules file exports env var w/ Qt choice
    # .pro file is also extended to have 'make install' target
    qmake -r mindforger.pro
    # PATCH optionally patch source files e.g. different Ubuntu distro specific paths
    echo "  PATCH files prior TARBALL in '`pwd`'"
    #if [[ "xenial" = "${UBUNTUVERSION}" ]]
    #then
	patchQmakePathInMakefile "Makefile"
	patchQmakePathInMakefile "app/Makefile"
	patchQmakePathInMakefile "lib/Makefile"
	patchQmakePathInMakefile "lib/src/Makefile"
    #fi
    
    # xxx LATER
    # xxx LATER
    # xxx LATER
    # 5) add new version to LOCAL Bazaar
    #echo -e "\n# bazaar add & commit  #######################################"
    #bzr add .
    # IMPORTANT: commit UPLOADs branch to server
    #bzr commit -m "Update for ${MF} at ${NOW}."

    # 5) create tarball ~ .tgz archive w/ source and required Debian cfg files
    echo -e "\n---------------------------------------------------------------"
    echo -e "5.1) create TARBALL: prepare files to work/ directory"
    createTarball
    
    # start GPG agent if it's NOT running
    echo -e "\n---------------------------------------------------------------"
    echo -e "5.2) GPG agent: start agent process if it is NOT running"
    if [ -e "${HOME}/.gnupg/S.gpg-agent" ]
    then
	echo "OK: GPG agent running."
    else
	gpg-agent --daemon
    fi    
    
    # 6) build SOURCE .deb package
    echo -e "\n# source & binary .deb packages  ######################################"
    echo -e "\n---------------------------------------------------------------"
    echo -e "6.1) build SIGNED SOURCE .deb package on HOST system (not FAKEROOT build) using sources prepared by TARBALL in work/ directory"
    pwd
    # Build of SOURCE .deb package
    #   - build is made using Makefile(s) generated by qmake ABOVE
    #   - build is made using files prepared by TARBALL step in work/ directory
    #   - build is made on HOST system, NOT clean FAKEROOT system
    #   - build is made using `bzr builddeb`
    #   - toolchain: builddeb > debuild > dpkg-buildpackage (find documentation there)
    # bzr builddeb -- -us -uc -j7
    #   --source ... build source package
    #   -us ... do NOT sign source package
    #   -uc ... do NOT sign .changes file
    #   -j7 ...
    #   -b  ... build BINARY package (SOURCE package otherwise w/ -S)
    #   -i  ... ignore build-in deps conflicts
    # doc and man:
    #   http://manpages.ubuntu.com/manpages/xenial/en/man1/dpkg-buildpackage.1.html
    #   http://manpages.ubuntu.com/manpages/xenial/en/man1/debuild.1.html
    #   man debuild
    # build SIGNED source .deb package:
    bzr builddeb --source
    # verify build result
    build_status=$?
    echo -e "SOURCE .deb package build on HOST system (buildarea/mindforger_<major>.<minor>.<patch>.orig.tar.gz):"
    [ $build_status -eq 0 ] && echo "  SUCCESSFULL" || exit 1
    
    # 7) build BINARY .deb from source deb on CLEAN system - no deps installed
    echo -e "\n# CLEAN SYSTEM MindForger build by pbuilder @ FAKEROOT #########"
    echo -e "5.4) build SIGNED BINARY .deb package on FAKEROOT system using sources prepared by SOURCE .deb build"
    # Build is made using Makefile(s) generated by qmake above:
    #   - build-area/ directory created by SOURCE `bzr buildeb` in the previous step
    #     is used to build MindForger using `pbuilder-dist` on fakeroot system
    #   - IMPORTANT: pbuilder's caches in /var and /home MUST be on same physical drive
    #     as build-area/
    #   - WARNING: mindforger/Makefile contains path to qmake which was determined
    #     on HOST system, therefore it does NOT have to be the same on fakeroot system
    #   - DEBUG: $PBUILDERFOLDER              ... build directory on HOST system (defined below)
    #   - DEBUG: /var/cache/pbuilder          ... last build's artifacts
    #   - DEBUG: /var/cache/pbuilder/base.tgz ... last build's distro of Ubuntu
    #   - DEBUG: ~/pbuilder                   ... Ubuntu distro snapshots ^
    # DRY RUN
    #   cd /home/dvorka/p/mindforger/launchpad/mindforger-2021-12-26--09-17-35/build-area && export PBUILDFOLDER=/tmp/mindforger-pbuilder-tmp && rm -rvf ${PBUILDFOLDER} ; mkdir -p ${PBUILDFOLDER} ; cp -rvf ~/pbuilder/*.tgz ${PBUILDFOLDER} ; pbuilder-dist xenial build mindforger_1.53.4-0ubuntu1.dsc
    # SHARP run:
    cd ../build-area
    # CLEAN build directory for pbuilder in tmp/
    export PBUILDFOLDER=/tmp/mindforger-pbuilder-tmp
    rm -rvf ${PBUILDFOLDER}
    mkdir -p ${PBUILDFOLDER}
    # COPY Ubuntu distros from user home to pbuilder build directory in tmp/
    cp -rvf ~/pbuilder/*.tgz ${PBUILDFOLDER}
    # BUILD .deb using source .deb files in tmp/ directory according to .dsc
    echo "  Running 'pbuilder-dist ${UBUNTUVERSION} build ${MFRELEASE}.dsc' in '`pwd`'"
    echo "    - mindfoger_<major>.<minor>.<patch>-0ubuntu1.dsc ... control descriptor according to which is build made"
    echo "    - mindfoger_<major>.<minor>.<patch>.orig.tar.gz  ... TARBALL w/ Debian control files used to build .deb"
    pbuilder-dist ${UBUNTUVERSION} build ${MFRELEASE}.dsc
    # VERIFY pbuilder-dist build result
    build_status=$?
    echo -e "BINARY .deb package build on FAKEROOT system (${PBUILDFOLDER}):"
    [ $build_status -eq 0 ] && echo "  SUCCESSFULL" || exit 1
        
    # 8) upload updated sources to Launchpad: push Bazaar and put changes
    echo -e "\n# bzr push .deb to Launchpad ###################################"    
    # from buildarea/ to ./dist
    cd ../${MF}
    echo "Before bzr push: " `pwd`

    # TODO commit in bzr first
    
    echo -e "SKIPPED FOR NOW"
    echo -e "SKIPPED FOR NOW"
    echo -e "SKIPPED FOR NOW"
    exit 0
    exit 0
    exit 0
    
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

export ARG_MAJOR_VERSION=1.53.
export ARG_MINOR_VERSION=4 # minor version is incremented for every Ubuntu version
export ARG_BAZAAR_MSG="MindForger ${ARG_MAJOR_VERSION}${ARG_MINOR_VERSION} release."

# https://wiki.ubuntu.com/Releases
# obsolete: precise quantal saucy precise utopic vivid wily yakkety artful cosmic
# current : (trusty) xenial bionic (cosmic disco eoan) focal (groovy) hirsute impish
# xenial bionic focal hirsute impish
# WIP: trusty xenial bionic focal hirsute impish
for UBUNTU_VERSION in bionic
do
    echo "Releasing MF for Ubuntu version: ${UBUNTU_VERSION}"
    releaseForParticularUbuntuVersion ${UBUNTU_VERSION} ${ARG_MAJOR_VERSION}${ARG_MINOR_VERSION} "${ARG_BAZAAR_MSG}"
    ARG_MINOR_VERSION=`expr $ARG_MINOR_VERSION + 1`
done

# eof
