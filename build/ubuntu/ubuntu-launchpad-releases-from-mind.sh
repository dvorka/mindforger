#!/usr/bin/env bash
#
# MindForger knowledge management tool
#
# Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>
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
# Tips:
# - run the script from Emacs shell to easily analyze script output
# - set OPT_* for PUSH and RELEASE to false to get .deb for any Ubuntu
#   version locally
#

# ########################################################################
# # Configuration
# ########################################################################

# Ubuntu version:
# - https://wiki.ubuntu.com/Releases
# - obsolete:
#     precise quantal saucy precise utopic vivid wily yakkety artful cosmic
# - current :
#     (trusty) xenial bionic (cosmic disco eoan) focal (groovy) (hirsute) (impish) jammy kinetic
# - command (Bash tuple of distro names):
#     trusty xenial bionic focal jammy kinetic
UBUNTU_VERSIONS=(focal)

# environment variables
export MAJOR_VERSION=1
export MINOR_VERSION=55
export PATCH_VERSION=1 # patch version is incremented for every Ubuntu build @ Launchpad
export MF_VERSION="${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}" # semantic version
export RM_CMD="rm -vrf "
export CP_CMD="cp -vrf "

# shell variables
OPT_VERBOSE="v"
OPT_DO_PUSH="false" # "true" to upload src to bazaar
OPT_DO_RELEASE="false" # "true" to dpush binary .deb to Launchpad and TRIGGER release

# ########################################################################
# # Helpers
# ########################################################################

function echoStep {
    echo -e "\n# ${1} ###################################################"
}

function echoStepDone {
    echo -e "\n# DONE: ${1} #############################################"
    pwd
}

function debugExit {
    echo "EXITING SCRIPT: intentional development script exit"
    exit 1    
}

# ########################################################################
# # Checkout MindForger from bazaar, copy over new sources, clean result
# ########################################################################

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
    rm -rf$OPT_VERBOSE} app build deps lib man LICENSE *.md
    mv ../.bzr .

    # copy NEW project files to Bazaar directory
    echo -e "\n# copy over new MindForger files ############################"
    cp -rf$OPT_VERBOSE} ${MFSRC}/* ${MFSRC}/*.*  .

    # prune MindForger project files: tests, *.o/... build files, ...
    echo -e "\n# CLEANUP development and build artifacts ###################"
    rm -rf$OPT_VERBOSE} ./.git ./.qmake.stash ./app/mindforger ./build ./app/test ./lib/test
    rm -rf$OPT_VERBOSE} ./lib/.qmake.stash ./lib/lib.pro.user ./lib/src/mindforger-lib-unit-tests
    rm -rf$OPT_VERBOSE} ./deps/cmark-gfm/.github
    rm -rf$OPT_VERBOSE} ./deps/mitie
    # IMPROVE: static libraries lib*.a are NOT deleted to keep cmark-gfm dependency libs
    find . -type f \( -name "*moc_*.cpp" -or -name "*.o" -or -name "*.*~" -or -name ".gitignore" -or -name ".git" \) | while read F; do rm -vf $F; done
    
    cd ..
}

# ############################################################################
# # Create updated changelog
# ############################################################################

function createChangelog {
  export MYTS=`date "+%a, %d %b %Y %H:%M:%S"`
  echo "Changelog timestamp: ${MYTS}"
  echo "mindforger (${MFFULLVERSION}) ${UBUNTU_VERSION}; urgency=low" > $1
  echo " " >> $1
  echo "  * ${MFBZRMSG}" >> $1
  echo " " >> $1
  echo " -- Martin Dvorak (Dvorka) <martin.dvorak@mindforger.com>  ${MYTS} +0100" >> $1
  # additional new line removed as it's not in HSTR
  #echo "" >> $1
}

# ############################################################################
# # Create upstream tarball in work/
# ############################################################################

function createTarball {
    echoStep "Create TARBALL ${MF}"
    # .orig.tar.gz is required Debian convention
    TARBALL_FILE="${MF}.orig.tar.gz"
    
    cd ..
    mkdir work
    cd work
    cp -rf${OPT_VERBOSE} ../${MF} .
    # remove bazaar control files
    rm -rf${OPT_VERBOSE} ${MF}/.bzr
    # create archives
    tar zcf ../${MF}.tgz ${MF}
    cp -fv ../${MF}.tgz ../${TARBALL_FILE}
    # change back to the directory which was zipped as tarball
    cd ../${MF}

    echoStepDone "TARBALL archive created in $(pwd)/..:"
    ls -l ../*.gz
}

# ############################################################################
# # Patch qmake location (if needed)
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
# # Release for *ONE* particular Ubuntu version
# ############################################################################
# - this is the main() function of this script
# - step by step it releases MindForger for one particular Ubuntu version

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
    echoStep "Cleanup"
    rm -rvf *.*~ ./debian
    
    # 2) checkout MindForger from Launchpad's bazaar to work directory
    #    (will be needed to build SOURCE .deb package)
    echoStep "Checkout MF from LaunchPad bazaar"
    mkdir ${MFBUILD}
    cd ${MFBUILD}
    checkoutMindforger ${MF} ${MFSRC}

    # 3) update Debian control files (changelog, descriptor, ...) in bzr clone
    echoStep "Create Debian control files"
    cd mindforger && cp -rvf ${MFSRC}/build/ubuntu/debian .
    createChangelog ./debian/changelog
    echo "Changelog:"
    cat ./debian/changelog

    # 4) build MF dependencies
    echoStep "Build MindForger library dependencies: cmark-gfm"
    rm -rf${OPT_VERBOSE} deps/cmark-gfm/build
    cd deps/cmark-gfm && mkdir -v build && cd build && cmake -DCMARK_TESTS=OFF -DCMARK_SHARED=OFF .. && cmake --build . && cd ../../..
    echoStepDone "cmark-gfm build"
    # cmark-gfm static library:
    ls -l deps/cmark-gfm/build/src/libcmark-gfm.a
    if [[ $? -eq 0 ]] ; then echo "  SUCCESSFULL"; else echo "  FAILED"; exit 1; fi
    
    # 5) generate MF Makefiles using qmake
    echoStep "Generate Makefiles using qmake"
    cd ..
    mv mindforger ${MF}
    cd ${MF}
    # Qt version MUST be specified as it CANNOT be configured by installing
    # qt5-default package: Debian control file does NOT allow virtual packages
    # like this qt5-default.
    # Instead debian/rules file exports env var w/ Qt choice
    # .pro file is also extended to have 'make install' target
    qmake -r mindforger.pro
    
    # 6) optionally PATCH source files e.g. different Ubuntu distro specific paths
    echoStep "Patch Makefiles - fix Qt paths for Ubuntu versions"
    #if [[ "xenial" = "${UBUNTUVERSION}" ]]
    #then
	patchQmakePathInMakefile "Makefile"
	patchQmakePathInMakefile "app/Makefile"
	patchQmakePathInMakefile "lib/Makefile"
	patchQmakePathInMakefile "lib/src/Makefile"
    #fi

    # 7) create tarball ~ .tgz archive w/ source and required Debian cfg files
    echo -e "\n---------------------------------------------------------------"
    echo -e "5.1) create TARBALL: prepare files to work/ directory"
    createTarball
    
    # 8) start GPG agent if it's NOT running
    echoStep "Start GPG agent process (if it is NOT running)"
    if [ -e "${HOME}/.gnupg/S.gpg-agent" ]
    then
	echo "OK: GPG agent running."
    else
	gpg-agent --daemon
    fi    

    # 9) add new version to LOCAL Bazaar branch (which will be used for .deb build)
    echoStep "add & commit ${MF} prepared files to the current bazaar branch"
    bzr add .
    # IMPORTANT: commit UPLOADs branch to server
    # https://code.launchpad.net/~ultradvorka/+junk/mindforger
    #   ^ browse changes/code and pushes @ Launchpad
    bzr commit -m "Update for ${MF} at ${NOW}."

    # 10) build SOURCE .deb package (copy sources, add control files and .tgz it to get .deb package)
    echoStep "Build SIGNED SOURCE .deb package from bzr branch"
    echo "  Building .deb package on HOST system (not FAKEROOT build) using sources prepared by TARBALL in bazaar work/ directory:"
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
    #   http://doc.bazaar.canonical.com/plugins/en/builddeb-plugin.html
    #   http://manpages.ubuntu.com/manpages/xenial/en/man1/dpkg-buildpackage.1.html
    #   http://manpages.ubuntu.com/manpages/xenial/en/man1/debuild.1.html
    #   man debuild
    # build SIGNED source .deb package:
    bzr builddeb --verbose --source
    # verify build result and EXIT on failure
    build_status=$?
    echo -e "DONE: SOURCE .deb package build on HOST system (buildarea/mindforger_<major>.<minor>.<patch>.orig.tar.gz):"
    if [[ $build_status -eq 0 ]] ; then echo "  SUCCESSFULL"; else echo "  FAILED"; exit 1; fi
    
    # 11) build BINARY .deb from source .deb on CLEAN system to dry run build @ Launchpad
    echoStep "Build SIGNED BINARY .deb package from source .deb (created in previous step) on CLEAN system (FAKEROOT mounted) - this is actual deps installation, compilation and link of the executable to create .deb file with the binary, HOWEVER, the binar .deb is NOT uploaded, this steps is made just to verify that the build will NOT fail on Launchpad (for the build is used just signed .dsc file)"
    # Build is made using Makefile(s) generated by qmake above:
    #   - build is made using GPG signed .dsc file which pbuild-dist takes as a parameter
    #     - .dsc files was created in the previous steps for .deb files
    #   - build-area/ directory created by SOURCE `bzr buildeb` in the previous step
    #     is used to build MindForger using `pbuilder-dist` on fakeroot system
    #   - OUTPUT of the pbuild-dist is stored in /tmp/mindforger-pbuilder-tmp/<ubuntu version name>_result
    #   - IMPORTANT: pbuilder's caches in /var and /home MUST be on same physical drive
    #     as build-area/
    #   - WARNING: mindforger/Makefile contains path to qmake which was determined
    #     on HOST system, therefore it does NOT have to be the same on fakeroot system
    #   - DEBUG: $PBUILDERFOLDER              ... build directory on HOST system (defined below)
    #   - DEBUG: /var/cache/pbuilder          ... last build's artifacts
    #   - DEBUG: /var/cache/pbuilder/base.tgz ... last build's distro of Ubuntu
    #   - DEBUG: ~/pbuilder                   ... Ubuntu distro snapshots ^
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
    # pbuild-dist help: https://wiki.ubuntu.com/PbuilderHowto
    pbuilder-dist ${UBUNTUVERSION} build ${MFRELEASE}.dsc
    # VERIFY pbuilder-dist build result
    build_status=$?
    echo -e "DONE: BINARY .deb package build on FAKEROOT system, result stored to ${PBUILDFOLDER}/${UBUNTUVERSION}_result:"
    if [[ $build_status -eq 0 ]] ; then echo "  SUCCESSFULL"; else echo "  FAILED"; exit 1; fi

    if [[ ${OPT_DO_PUSH} == "true" ]]
    then
       # 12) push updated sources to Launchpad bazaar server
       echoStep "Push updated sources to Launchpad bazaar"
       cd ../${MF}
       echo "Invoking bzr push in: $(pwd)"
       bzr push lp:~ultradvorka/+junk/mindforger

       if [[ ${OPT_DO_RELEASE} == "true" ]]
       then       
	   cd ..
	   echo "Invoking dput in: $(pwd)"
	   ls -l ${MFRELEASE}_source.changes
	   # recently added /ppa to fix the path and package rejections
	   # MF PPA w/ 64b build only
	   dput ppa:ultradvorka/productivity ${MFRELEASE}_source.changes
	   # SKIP: MF PPA w/ 64b 32b and ARM builds
	   #dput ppa:ultradvorka/ppa ${MFRELEASE}_source.changes
       fi
    fi
}

#############################################################################
# Main
#############################################################################

if [ -e "../../.git" ]
then
    echo "This script must NOT be run from Git repository - run it e.g. from ~/p/mindforger/launchpad instead"
    exit 1
fi

export BAZAAR_MSG="MindForger ${MF_VERSION} release."

for UBUNTU_VERSION in ${UBUNTU_VERSIONS}
do
    echo -e "\n###################################################"
    echo "# Releasing MF for Ubuntu version: ${UBUNTU_VERSION}"
    echo "###################################################"
    releaseForParticularUbuntuVersion ${UBUNTU_VERSION} "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}" "${BAZAAR_MSG}"
    MINOR_VERSION=`expr $MINOR_VERSION + 1`
done

# eof
