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

# This scripts list content of the local aptly repository.
#
# Debian releases: https://www.debian.org/releases/
# - 6/7/8/9/10: squeeze wheezy jessie stretch buster bullseye
#
# aptly abstractions hierarchy:
#
# - repository
#   - packages
# ^ snapshots
#   ^ publish
#
# See 'MindForger Release Guide#Debian and my PPA' notebook for detailed steps description...

# .deb package to be added
export NEW_VERSION_NAME="1.55.1"
export NEW_RELEASE_NAME="mindforger_${NEW_VERSION_NAME}"
export NEW_DEB_NAME="${NEW_RELEASE_NAME}-1_amd64.deb"
# Debian release ~ aptly publish
export MY_DEBIAN_RELEASE="stretch"
# export MY_DEBIAN_RELEASE="bullseye"

export MY_APTLY_PATH="/home/dvorka/.aptly"
export MY_APTLY_UPLOAD_PATH="${MY_APTLY_PATH}/public"
export MY_APTLY_VERSION="$(aptly version)"
# PPA REPOSITORY name (aptly can manage multiple - this identifier)
export MY_APTLY_REPO_NAME="$(aptly repo list --raw)"
# PPA repository SNAPSHOT is entity which is published
export MY_APTLY_SNAPSHOT_NAME="$(aptly snapshot list --raw)"
# PUBLISH of repository snapshot is stored to ~/.aptly/public so that it can be uploaded
export MY_APTLY_PUBLISH="$(aptly publish list)"
export MY_APTLY_PUBLISH_NAME="$(aptly publish list --raw | while read A B; do echo ${B}; done)"

echo "Local aptly repository overview:"
echo "  ${MY_APTLY_VERSION}"
echo "  aptly repository path: ${MY_APTLY_PATH}"
echo "  aptly config path    : ${MY_APTLY_PATH}.config"
echo "Package:"
echo "  release              : ${NEW_RELEASE_NAME}"
echo "  .deb                 : ${NEW_DEB_NAME}"
echo "${MY_APTLY_PATH} overview:"
echo "  repository           : ${MY_APTLY_REPO_NAME}"
echo "  publish              : ${MY_APTLY_PUBLISH_NAME}"
echo "  snapshot             : ${MY_APTLY_SNAPSHOT_NAME}"
echo -e "  packages             :\n"
aptly repo show -with-packages "${MY_APTLY_REPO_NAME}"
# echo -e "\n${MY_APTLY_UPLOAD_PATH} to be UPLOADED to www.mindforger.com/debian:"
# cd ${MY_APTLY_PATH}/public && tree

# next steps:
# - BACKUP ~/.aptly with timestamp
# - add PACKAGE to repository
# - create SNAPSHOT
# - create PUBLISH

export MY_NEXT_RELEASE_SNAPSHOT_NAME="${NEW_RELEASE_NAME-snapshot}"

# checks

if [[ ! -f "${NEW_DEB_NAME}" ]]
then
    echo "Error: File with package to be released not found: ${NEW_DEB_NAME}"
    exit 1
fi

# backup
export timestamp=`date +%Y-%m-%d--%H-%M-%S`
tar zvcf ~/.aptly-${timestamp}.tgz ~/.aptly
echo "Backup DONE"

# drop
aptly publish drop ${MY_APTLY_PUBLISH_NAME}
aptly snapshot drop ${MY_APTLY_SNAPSHOT_NAME}
# add
export MY_APTLY_SNAPSHOT_NAME="${NEW_RELEASE_NAME}"
aptly repo edit -distribution="${MY_DEBIAN_RELEASE}" "${MY_APTLY_REPO_NAME}"
aptly repo add ${MY_APTLY_REPO_NAME} ${NEW_DEB_NAME}
aptly snapshot create ${MY_APTLY_SNAPSHOT_NAME} from repo ${MY_APTLY_REPO_NAME}
aptly publish snapshot ${MY_APTLY_SNAPSHOT_NAME}

# upload
echo -e "\n${MY_APTLY_UPLOAD_PATH} to be UPLOADED to www.mindforger.com/debian:"

# eof
