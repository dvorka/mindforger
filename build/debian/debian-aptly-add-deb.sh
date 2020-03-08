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

# Debian releases: https://www.debian.org/releases/
#   6/7/8/9/10: squeeze wheezy jessie stretch buster

echo "See 'MindForger Release Guide#Debian and my PPA' notebook for detailed steps description..."

# list aptly repositories
#aptly repo list

# show repository basic info
#aptly repo show mindforger-com-ppa

# show repo content w/ packages
#aptly repo show -with-packages mindforger-com-ppa

# show snapshots
#aptly snapshot list 

# show published snapshots
#aptly publish list 

echo "Add new .deb to PPA"

# delete published snapshot(s) ONLY if needed
#aptly publish drop stretch

# delete snapshot(s) ONLY if needed - publish must be deleted first!
#aptly snapshot drop mindforger-1.10.0-snapshot

# add .deb to repository
#aptly repo add mindforger-com-ppa mindforger_1.20.1-1_amd64.deb

# create snapshot in the time of release - note release NAME 
#aptly snapshot create mindforger-1.20.0-snapshot from repo mindforger-com-ppa

# publish snapshot to local filesystem so that it can be uploaded - note release NAME
#aptly publish snapshot mindforger-1.20.0-snapshot

######################################################################

# .deb to add
export NEW_DEB="mindforger_1.52.0-1_amd64.deb"
export OLD_VERSION="1.51.0"
export NEW_VERSION="1.52.0"

# check
if [ -f "${NEW_DEB}" ]
then
    echo "Adding ${NEW_DEB} ..."
else
    echo "Package ${NEW_DEB} not found"
    exit 1
fi

# backup
export timestamp=`date +%Y-%m-%d--%H-%M-%S`
tar zvcf ~/.aptly-${timestamp}.tgz ~/.aptly

# gather current entities
export MY_REPO=`aptly repo list --raw`
export OLD_PUBLISH=`aptly publish list --raw | while read A B; do echo $B; done`
export OLD_SNAPSHOT=`aptly snapshot list --raw`
aptly repo show -with-packages ${MY_REPO}

# prepare new entities
export NEW_PUBLISH="${OLD_PUBLISH/${OLD_VERSION}/${NEW_VERSION}}"
export NEW_SNAPSHOT="${OLD_SNAPSHOT/${OLD_VERSION}/${NEW_VERSION}}"

echo -e "\nSummary:"
echo "${NEW_DEB}"
echo "${OLD_VERSION} > ${NEW_VERSION}"
echo "${MY_REPO}"
echo "${OLD_PUBLISH} > ${NEW_PUBLISH}"
echo "${OLD_SNAPSHOT} > ${NEW_SNAPSHOT}"

# drop: publish > snapshot
echo "aptly publish drop ${OLD_PUBLISH}"
echo "aptly snapshot drop ${OLD_SNAPSHOT}"
# add .deb > snapshot > publish
echo "aptly repo add ${MY_REPO} ${NEW_DEB}"
echo "aptly snapshot create ${NEW_SNAPSHOT} from repo ${MY_REPO}"
echo "aptly publish snapshot ${NEW_SNAPSHOT}"

# exit for dry run
#exit 0

# drop: publish > snapshot
aptly publish drop ${OLD_PUBLISH}
aptly snapshot drop ${OLD_SNAPSHOT}
# add .deb > snapshot > publish
aptly repo add ${MY_REPO} ${NEW_DEB}
aptly snapshot create ${NEW_SNAPSHOT} from repo ${MY_REPO}
aptly publish snapshot ${NEW_SNAPSHOT}

aptly repo show -with-packages ${MY_REPO}

# eof
