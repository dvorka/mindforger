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

# Debian release infrastructure configuration (to be sourced).
#
# This is the ONE AND ONLY place where the paths and the key are configured
# (make targets and README.md just use/describe these defaults).
#
# Every value can be overridden by the environment variable of the same name
# - use it when the archive is moved (e.g. on a new machine) instead of
# editing this file:
#
#   export MF_DEBIAN_ARCHIVE_DIR=/new/location/aptly/mindforger  # ~/.bashrc
#
# The configuration in effect (incl. its source: default or env) is printed
# by every command which writes anything - see mf_debian_print_config.
#
# Sourced by: build/debian/debian-ppa.sh, build/docker/debian-deb/build-deb.sh

MF_REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

# remember which values come from the environment (printed by mf_debian_print_config)
MF_ENV_OVERRIDES=" "
for V in MF_DEBIAN_ARCHIVE_DIR MF_DEBIAN_PPA_DIR MF_DEBIAN_DEB_DIR MF_DEBIAN_GPG_KEY MF_DEBIAN_GPG_KEY_URL; do
    if [ -n "${!V:-}" ]; then
        MF_ENV_OVERRIDES="${MF_ENV_OVERRIDES}${V} "
    fi
done

# MASTER .deb archive (source of truth) w/ <number>-<codename>/mindforger_*.deb
# subdirs - it is NOT in Git, back it up!
MF_DEBIAN_ARCHIVE_DIR="${MF_DEBIAN_ARCHIVE_DIR:-${HOME}/p/mindforger/debian/aptly/mindforger}"
# PPAs for all Debian releases side-by-side - to be uploaded to /debian-ppa/
# (generated, Git ignored - it can be rebuilt from the archive anytime)
MF_DEBIAN_PPA_DIR="${MF_DEBIAN_PPA_DIR:-${MF_REPO_ROOT}/distro/debian}"
# .deb packages built by build/docker/debian-deb/build-deb.sh
MF_DEBIAN_DEB_DIR="${MF_DEBIAN_DEB_DIR:-${MF_REPO_ROOT}/../mindforger-deb}"
# GPG key used to sign PPAs (fingerprint)
MF_DEBIAN_GPG_KEY="${MF_DEBIAN_GPG_KEY:-E3E8520DE3820D8C1A724BB7B72E4F7F24AF591D}"
# public GPG key as published on the web (users import it)
MF_DEBIAN_GPG_KEY_URL="${MF_DEBIAN_GPG_KEY_URL:-https://www.mindforger.com/gpgpubkey.txt}"

# print the configuration in effect (once per run)
MF_CONFIG_PRINTED=false
function mf_debian_print_config {
    local V
    if [ "${MF_CONFIG_PRINTED}" = "true" ]; then
        return
    fi
    MF_CONFIG_PRINTED=true
    echo "= Configuration (build/debian/debian-config.sh, env overrides) ======="
    for V in MF_DEBIAN_ARCHIVE_DIR MF_DEBIAN_PPA_DIR MF_DEBIAN_DEB_DIR MF_DEBIAN_GPG_KEY; do
        if [[ "${MF_ENV_OVERRIDES}" == *" ${V} "* ]]; then
            printf "  %-22s: %s (env)\n" "${V}" "${!V}"
        else
            printf "  %-22s: %s (default)\n" "${V}" "${!V}"
        fi
    done
}

# eof
