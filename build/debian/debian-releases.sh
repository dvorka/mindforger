#!/usr/bin/env bash
#
# MindForger knowledge management tool
#
# Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>
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

# library (to be sourced) which reads the list of Debian releases from
# debian-releases.conf - see that file for the description of the columns.
#
# usage:
#   source build/debian/debian-releases.sh
#   mf_debian_releases supported        # codenames w/ given status
#   mf_debian_release_html_backend trixie

MF_DEBIAN_RELEASES_CONF="${MF_DEBIAN_RELEASES_CONF:-$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/debian-releases.conf}"

# conf columns
MF_DEBIAN_COL_NUMBER=1
MF_DEBIAN_COL_CODENAME=2
MF_DEBIAN_COL_STATUS=3
MF_DEBIAN_COL_HTML_BACKEND=4

# print codenames of Debian releases (newest first) w/ optional status filter
#
# parameters:
#   $1 - status (optional): supported | frozen
function mf_debian_releases {
    awk -v status="${1:-}" -v s="${MF_DEBIAN_COL_STATUS}" -v c="${MF_DEBIAN_COL_CODENAME}" \
        '!/^[[:space:]]*(#|$)/ && (status == "" || $s == status) { print $c }' \
        "${MF_DEBIAN_RELEASES_CONF}"
}

# print given column of the Debian release (empty if release is unknown)
#
# parameters:
#   $1 - codename  e.g. trixie
#   $2 - column    e.g. ${MF_DEBIAN_COL_STATUS}
function mf_debian_release_field {
    awk -v codename="${1}" -v f="${2}" -v c="${MF_DEBIAN_COL_CODENAME}" \
        '!/^[[:space:]]*(#|$)/ && $c == codename { print $f }' \
        "${MF_DEBIAN_RELEASES_CONF}"
}

function mf_debian_release_number {
    mf_debian_release_field "${1}" "${MF_DEBIAN_COL_NUMBER}"
}

function mf_debian_release_status {
    mf_debian_release_field "${1}" "${MF_DEBIAN_COL_STATUS}"
}

function mf_debian_release_html_backend {
    mf_debian_release_field "${1}" "${MF_DEBIAN_COL_HTML_BACKEND}"
}

# succeeds if the codename is listed in debian-releases.conf
function mf_debian_is_release {
    [ -n "$(mf_debian_release_number "${1}")" ]
}

# eof
