#!/usr/bin/env bash
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

# Method:
#   create tarball (tarball + tests - makefiles)
#

if [ -e "../.git" ]
then
  echo "This script must NOT be run from Git repository - run it e.g. from ~/p/mindforger/release instead"
  exit 1
fi

# ############################################################################
# # Create upstream tarball #
# ############################################################################

function createTarball {
  cd ..
  mkdir work
  cd work
  cp -vrf ../${MF} .
  tar zcf ../${MF}_tarball.tgz ${MF}
  cd ../${MF}
}

# ############################################################################
# # Build source and binary deb packages #
# ############################################################################

function buildGitHubTarball {
    export SCRIPTHOME=`pwd`
    export MFVERSION=$1
    export MFBZRMSG=$2
    export MFCIBUILD=$3
    #export MFFULLVERSION=${MFVERSION}-1.0 # NMU upload
    export MFFULLVERSION=${MFVERSION}-1    # mantainer upload
    if [[ "${MFCIBUILD}" = "ci" ]]
    then
        export MF=mindforger_${MFVERSION}
    else
        export MF=ci_mindforger_${MFVERSION}
    fi
    export MFRELEASE=mindforger-${MFFULLVERSION}
    if [[ -d "/home/dvorka" ]]
    then
        echo "  Linux tarball build"
        export MFSRC="/home/dvorka/p/mindforger/git/mindforger"
    else
	if [[ "${MFCIBUILD}" = "ci" ]]
        then
            echo "  CI tarball build"
	    export MFSRC="/home/runner/work/mindforger/mindforger"
	else
            echo "  macOS tarball build"
	    export MFSRC="/Users/dvorka/p/mindforger/git/mindforger"
	fi
    fi
    export NOW=`date +%Y-%m-%d--%H-%M-%S`
    export MFBUILD=mindforger-${NOW}
    export UBUNTUVERSION=unstable

    # DO_MF_DEBUG is disabled by default now
    #if ! grep -q "//#define DO_MF_DEBUG" "${MFSRC}/lib/src/debug.h"
    #then
    #echo "This script must NOT be run if debug code is enabled - disable DO_MF_DEBUG first"
    #exit 1
    #fi
    
    #
    # 1) create tarball
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
    rm -rvf ./lib.pro.user ./lib/src/mindforger-lib-unit-tests
    rm -rvf ./app/mindforger.app
    rm -rvf ./deps/cmark-gfm/.github ./deps/cmark-gfm/build
    rm -rvf ./deps/mitie
    rm -vf ./mindforger.pro.user ./mindforger.pro.user.macos ./mindforger.pro.user.ubuntu
    find . -type f \( -name "*moc_*.cpp" -or -name "*.a" -or -name "*.o" -or -name "*.*~" -or -name ".gitignore" -or -name ".git" \) | while read F; do rm -vf ${F}; done
    find . -type f \( -name "*.dmg" -or -name "*.stash" \) | while read F; do rm -vf ${F}; done
    # 1.4) create tar archive
    createTarball
}

# ############################################################################
# # Main #
# ############################################################################

export ARG_VERSION="1.54.0"
export ARG_BAZAAR_MSG="MindForger ${ARG_VERSION} release."

buildGitHubTarball "${ARG_VERSION}" "${ARG_BAZAAR_MSG}" ${1}

# eof
