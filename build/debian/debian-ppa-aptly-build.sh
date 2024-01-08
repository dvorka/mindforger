#!/bin/bash
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



#
# This script builds MindForger/HSTR Debian PPA for a particular Debian release.
#   - it adds all .deb pacakges in given directory to aptly repository.
#
# Usage:
#
#   MF_DEBIAN_DISTRO_DIR=... MF_DEBIAN_RELEASE=... ./debian-aptly-add-deb.sh 
#
# Example:
#
#   MF_DEBIAN_DEB_DIR=~/p/mindforger/debian/aptly/mindforger/12-bookworm \ 
#   MF_DEBIAN_RELEASE=bookworm \
#   ./debian-release-aptly-build.sh
#
# aptly abstractions hierarchy:
#
# - repository
#   - packages
# ^ snapshots
#   ^ publish
#
# See 'MindForger Release Guide#Debian and my PPA' notebook for detailed steps description.

export MF_IN_DEB_PACKAGES_DIR="/home/dvorka/p/mindforger/debian/aptly"
# ^ must have the following structure:
#.
#├── hstr
#│   └── 09-stretch
#│       ├── hstr_1.27.0-1_amd64.deb
#│       ├── hstr_2.0.0-1_amd64.deb
#│       └── hstr_2.4.0-1_amd64.deb
#└── mindforger
#    ├── 09-stretch
#    │   ├── mindforger_1.42.0-1_amd64.deb
#    │   ├── ...
#    │   └── mindforger_1.53.0-1_amd64.deb
#    ├── ...
#    └── 13-trixie
#        └── mindforger_1.54.0-1_amd64.deb
#
export MF_OUT_PPA_ALL_DEBIAN_RELEASES_DIR="/home/dvorka/p/mindforger/debian/debian-ppa.mindforger.com"
# ^ PPAs for all releases side-by-side



# Add ONE .deb package to aptly repository:
#   
# Parameters:
#   $1 - MF version     e.g. 1.55.0
#   $2 - Debian release e.g. stretch 
#
function add_deb {
    NEW_MF_VERSION=${1}
    NEW_DEBIAN_RELEASE=${2}

    export NEW_MF_RELEASE_NAME="mindforger_${NEW_MF_VERSION}"
    export NEW_DEB_NAME="${NEW_MF_RELEASE_NAME}-1_amd64.deb"

    export MY_APTLY_PATH="/home/dvorka/.aptly"
    export MY_APTLY_UPLOAD_PATH="${MY_APTLY_PATH}/public"
    export MY_APTLY_VERSION="$(aptly version)"
    # PPA REPOSITORY name (aptly can manage multiple - this identifier)
    export MY_APTLY_REPO_NAME="${NEW_DEBIAN_RELEASE}-main"
    # PPA repository SNAPSHOT is entity which is published
    #export MY_APTLY_SNAPSHOT_NAME="$(aptly snapshot list --raw)"
    export MY_APTLY_SNAPSHOT_NAME="${NEW_DEBIAN_RELEASE}-snapshot"
    # PUBLISH of repository snapshot is stored to ~/.aptly/public so that it can be uploaded
    #export MY_APTLY_PUBLISH="$(aptly publish list)"
    #export MY_APTLY_PUBLISH_NAME="$(aptly publish list --raw | while read A B; do echo ${B}; done)"
    export MY_APTLY_PUBLISH_NAME="${NEW_DEBIAN_RELEASE}-publish"

    echo
    echo "= Local aptly repository overview: ===================================="
    # echo "  ${MY_APTLY_VERSION}"
    echo "  aptly repository path: ${MY_APTLY_PATH}"
    echo "  aptly config path    : ${MY_APTLY_PATH}.config"
    echo "New snapshot:"
    echo "  Debian release       : ${NEW_DEBIAN_RELEASE}"
    echo "  MF release           : ${NEW_MF_VERSION}"
    echo "  .deb file            : ${NEW_DEB_NAME}"
    echo "  publish name         : ${MY_APTLY_PUBLISH_NAME}"
    echo "  snapshot name        : ${MY_APTLY_SNAPSHOT_NAME}"
    echo "  repository name      : ${MY_APTLY_REPO_NAME}"
    echo "# BEFORE repos ========================================================"
    aptly repo list
    echo "# BEFORE repo ========================================================="
    aptly repo create "${MY_APTLY_REPO_NAME}"
    aptly repo show -with-packages "${MY_APTLY_REPO_NAME}"
    echo "# BEFORE snapshots ===================================================="
    aptly snapshot list
    echo "# BEFORE snapshot ====================================================="
    aptly snapshot show -with-packages "${MY_APTLY_SNAPSHOT_NAME}"
    echo "# BEFORE publishes ===================================================="
    aptly publish list
    echo "# BEFORE publish ======================================================"
    aptly publish show "${MY_APTLY_PUBLISH_NAME}"
    echo "> RUN >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
    echo

    # checks
    if [[ ! -f "${NEW_DEB_NAME}" ]]
    then
        echo "Error: File with package to be released not found: ${NEW_DEB_NAME}"
        exit 1
    fi

    #echo "STEP: .aptly/ backup:"
    #export timestamp=`date +%Y-%m-%d--%H-%M-%S`
    #tar zcf ~/p/mindforger/debian/aptly/backup-aptly-${timestamp}.tgz ~/.aptly
    #echo "DONE: backup"

    echo "STEP: drop publish > snapshot > repo"
    aptly publish drop ${NEW_DEBIAN_RELEASE}
    aptly snapshot drop ${MY_APTLY_SNAPSHOT_NAME}

    echo "STEP: add .deb to repo > snapshot > publish"
    # aptly repo edit -distribution="${NEW_DEBIAN_RELEASE}" "${MY_APTLY_REPO_NAME}"
    aptly repo add ${MY_APTLY_REPO_NAME} ${NEW_DEB_NAME}
    aptly snapshot create ${MY_APTLY_SNAPSHOT_NAME} from repo ${MY_APTLY_REPO_NAME}
    aptly publish snapshot -distribution=${NEW_DEBIAN_RELEASE} ${MY_APTLY_SNAPSHOT_NAME}
    echo "DONE: add .deb"

    # upload
    #echo -e "\n${MY_APTLY_UPLOAD_PATH} can be UPLOADED to www.mindforger.com/debian:"
    #tree ${MY_APTLY_UPLOAD_PATH}

    # copy it to ALL Debian releases dir
    export MY_PPA_DISTR_DIR="${MF_OUT_PPA_ALL_DEBIAN_RELEASES_DIR}/${NEW_DEBIAN_RELEASE}"
    rm -rvf ${MY_PPA_DISTR_DIR}
    mkdir -pv ${MY_PPA_DISTR_DIR}
    cp -vrf ${MY_APTLY_UPLOAD_PATH}/* ${MY_PPA_DISTR_DIR}
    # ensure index.html for the Debian release
    echo "STEP: create index.html for the Debian release"
    cat ./index-ppa.html | sed -e "s/DISTRO/${NEW_DEBIAN_RELEASE}/g" > "${MY_PPA_DISTR_DIR}/index.html"

    tree ${MF_OUT_PPA_ALL_DEBIAN_RELEASES_DIR}
}

function add_debian_release {
    export MF_DEBIAN_RELEASE_DIR=${1}
    export MF_DEBIAN_RELEASE=${2}

    find ${MF_DEBIAN_RELEASE_DIR} -name "*.deb" | while read P
    do
        export MF_DEB_FILE_NAME=$(basename ${P})

        echo ${MF_DEB_FILE_NAME} | tr "_" " " | while read A B C
        do 
        echo $B | tr "-" " " | while read X Y
        do
            echo $X 
        done
        done | while read MF_RELEASE_VERSION
        do
            echo "Debinan release: ${MF_DEBIAN_RELEASE}"
            echo "Input file     : ${P}"
            echo ".deb file name : ${MF_DEB_FILE_NAME}"
            echo "MF version     : ${MF_RELEASE_VERSION}"

            # prepare .deb file to the current directory
            rm -f ${MF_DEB_FILE_NAME}
            cp -f ${P} .

            # add .deb to repo
            add_deb ${MF_RELEASE_VERSION} ${MF_DEBIAN_RELEASE}

            # cleanup
            rm -f ${MF_DEB_FILE_NAME}
        done
    done
}

# 
# MAIN
#

# purge debian-ppa.mindforger.com
rm -rvf "${MF_OUT_PPA_ALL_DEBIAN_RELEASES_DIR}"

# FOR every Debian release in the input directory
find "${MF_IN_DEB_PACKAGES_DIR}/mindforger" -maxdepth 1 -mindepth 1 -type d | while read MF_DEBIAN_RELEASE_DIR
do
    export MF_DEBIAN_RELEASE=$(basename ${MF_DEBIAN_RELEASE_DIR} | tr "-" " " | while read A B; do echo ${B}; done)
    echo "Debinan release: ${MF_DEBIAN_RELEASE}"
    echo "Input dir      : ${MF_DEBIAN_RELEASE_DIR}"

    export MY_APTLY_REPO_NAME="${MF_DEBIAN_RELEASE}-main"
    export MY_APTLY_SNAPSHOT_NAME="${MF_DEBIAN_RELEASE}-snapshot"

    echo "STEP: DROP it all: snapshots, publishes, repos"
    aptly publish drop ${MF_DEBIAN_RELEASE}
    aptly snapshot drop ${MY_APTLY_SNAPSHOT_NAME}
    aptly repo drop ${MY_APTLY_REPO_NAME}
    aptly repo drop "bookworm-main"
    aptly repo drop "jessie-main"

    add_debian_release ${MF_DEBIAN_RELEASE_DIR} ${MF_DEBIAN_RELEASE}

    # purge aptly and start over
    aptly publish drop ${MF_DEBIAN_RELEASE}
    aptly snapshot drop ${MY_APTLY_SNAPSHOT_NAME}
    aptly repo drop ${MY_APTLY_REPO_NAME}
done

# HTML w/ links to for all Debian releases
cp -vf ./index-all-ppas.html "${MF_OUT_PPA_ALL_DEBIAN_RELEASES_DIR}/index.html"

# eof
