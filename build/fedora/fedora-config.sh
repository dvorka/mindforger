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

# Fedora release infrastructure configuration (to be sourced).
#
# This is the ONE AND ONLY place where the paths and the COPR project are
# configured (make targets and README.md just use/describe these defaults).
#
# Every value can be overridden by the environment variable of the same name:
#
#   export MF_FEDORA_RPM_DIR=/new/location/fedora  # ~/.bashrc
#
# The configuration in effect (incl. its source: default or env) is printed
# by every command - see mf_fedora_print_config.
#
# Sourced by: build/fedora/fedora-copr.sh

MF_REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

MF_FEDORA_CONFIG_VARS="MF_FEDORA_COPR_PROJECT MF_FEDORA_ARCHES MF_FEDORA_RPM_DIR MF_FEDORA_COPR_CONFIG MF_FEDORA_RELEASES_CONF"

# remember which values come from the environment (printed by mf_fedora_print_config)
MF_ENV_OVERRIDES=" "
for V in ${MF_FEDORA_CONFIG_VARS}; do
    if [ -n "${!V:-}" ]; then
        MF_ENV_OVERRIDES="${MF_ENV_OVERRIDES}${V} "
    fi
done

# COPR project (owner/name) - https://copr.fedorainfracloud.org/coprs/<owner>/<name>/
MF_FEDORA_COPR_PROJECT="${MF_FEDORA_COPR_PROJECT:-dvorka/mindforger}"
# architectures built in COPR (chroot fedora-<number>-<arch>)
MF_FEDORA_ARCHES="${MF_FEDORA_ARCHES:-x86_64 aarch64}"
# source tarball, SRPM and .rpm packages (local builds and COPR downloads)
# - generated, Git ignored (like MF_DEBIAN_PPA_DIR)
MF_FEDORA_RPM_DIR="${MF_FEDORA_RPM_DIR:-${MF_REPO_ROOT}/distro/fedora}"
# COPR API token file - https://copr.fedorainfracloud.org/api/
MF_FEDORA_COPR_CONFIG="${MF_FEDORA_COPR_CONFIG:-${HOME}/.config/copr}"
# the list of Fedora releases
MF_FEDORA_RELEASES_CONF="${MF_FEDORA_RELEASES_CONF:-${MF_REPO_ROOT}/build/fedora/fedora-releases.conf}"

# print the configuration in effect (once per run)
MF_CONFIG_PRINTED=false
function mf_fedora_print_config {
    local V
    if [ "${MF_CONFIG_PRINTED}" = "true" ]; then
        return
    fi
    MF_CONFIG_PRINTED=true
    echo "= Configuration (build/fedora/fedora-config.sh, env overrides) ======="
    for V in ${MF_FEDORA_CONFIG_VARS}; do
        if [[ "${MF_ENV_OVERRIDES}" == *" ${V} "* ]]; then
            printf "  %-24s: %s (env)\n" "${V}" "${!V}"
        else
            printf "  %-24s: %s (default)\n" "${V}" "${!V}"
        fi
    done
}

# print Fedora release numbers (newest first) w/ optional status filter
#
# parameters:
#   $1 - status (optional): supported | frozen
function mf_fedora_releases {
    awk -v status="${1:-}" \
        '!/^[[:space:]]*(#|$)/ && (status == "" || $2 == status) { print $1 }' \
        "${MF_FEDORA_RELEASES_CONF}"
}

# succeeds if the release number is listed in fedora-releases.conf
function mf_fedora_is_release {
    [ -n "${1:-}" ] && mf_fedora_releases | grep -qx "${1}"
}

# print COPR chroots of supported Fedora releases e.g. fedora-44-x86_64
function mf_fedora_chroots {
    local number arch
    for number in $(mf_fedora_releases supported); do
        for arch in ${MF_FEDORA_ARCHES}; do
            echo "fedora-${number}-${arch}"
        done
    done
}

# eof
