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

# Debian releases: https://www.debian.org/releases/
#   6/7/8/9/10: squeeze wheezy jessie stretch buster

# create new repository: Debian stretch, mindforger-release
#aptly repo create -distribution=stretch -component=main mindforger-release

# add DEB to repo
#aptly repo add mindforger-release mindforger_0.7.1-1_amd64.deb

# show repo content
#aptly repo show -with-packages mindforger-release

# create snapshot in the time of release
#aptly snapshot create mindforger-0.7.1 from repo mindforger-release

# publish snapshot to local filesystem so that it can be uploaded
#aptly publish snapshot mindforger-0.7.1

# eof
