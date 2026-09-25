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

# builds MindForger .deb packages for supported Debian releases using
# Docker, each in its own isolated container/image, so no Debian trixie (or
# bullseye/bookworm) machine is needed to produce a package that can be
# attached to a GitHub release.
#
# Debian releases (and their HTML backend) are read from
# build/debian/debian-releases.conf
#
# usage:
#   ./build-deb.sh [release ...] [output-dir]
#
#   release     one or more codenames from debian-releases.conf, or
#               supported (default) ... all releases w/ supported status
#   output-dir  default: MF_DEBIAN_DEB_DIR (build/debian/debian-config.sh),
#               one subdir per release
#
# examples:
#   ./build-deb.sh trixie
#   ./build-deb.sh bookworm trixie ~/mindforger-debs

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../../.." && pwd)"

# shellcheck source=../../debian/debian-config.sh
source "${REPO_ROOT}/build/debian/debian-config.sh"
# shellcheck source=../../debian/debian-releases.sh
source "${REPO_ROOT}/build/debian/debian-releases.sh"

read -r -a SUPPORTED_RELEASES <<< "$(mf_debian_releases supported | tr '\n' ' ')"

ARGS=("$@")
OUT_DIR="${MF_DEBIAN_DEB_DIR}"
RELEASES=()

for A in "${ARGS[@]+"${ARGS[@]}"}"; do
    if [ "${A}" = "supported" ] || [ "${A}" = "all" ]; then
        RELEASES+=("${SUPPORTED_RELEASES[@]}")
    elif mf_debian_is_release "${A}"; then
        RELEASES+=("${A}")
    else
        OUT_DIR="${A}"
    fi
done

if [ "${#RELEASES[@]}" -eq 0 ]; then
    RELEASES=("${SUPPORTED_RELEASES[@]}")
fi

for REL in "${RELEASES[@]}"; do
    HTML_BACKEND="$(mf_debian_release_html_backend "${REL}")"
    if [ "${HTML_BACKEND}" != "webkit" ] && [ "${HTML_BACKEND}" != "webengine" ]; then
        echo "ERROR: Debian ${REL} cannot be built (html-backend '${HTML_BACKEND}' in ${MF_DEBIAN_RELEASES_CONF})"
        exit 1
    fi
done

mkdir -p "${OUT_DIR}"
OUT_DIR="$(cd "${OUT_DIR}" && pwd)"

echo "Repo     : ${REPO_ROOT}"
echo "Output   : ${OUT_DIR}"
echo "Releases : ${RELEASES[*]}"

for REL in "${RELEASES[@]}"; do
    echo -e "\n==================== ${REL} ===================="
    REL_OUT_DIR="${OUT_DIR}/${REL}"
    mkdir -p "${REL_OUT_DIR}"

    docker build \
        --build-arg "DEBIAN_RELEASE=${REL}" \
        --build-arg "MF_HTML_BACKEND=$(mf_debian_release_html_backend "${REL}")" \
        -t "mindforger-deb-builder:${REL}" \
        "${SCRIPT_DIR}"
    docker run --rm \
        -v "${REPO_ROOT}:/src:ro" \
        -v "${REL_OUT_DIR}:/out" \
        "mindforger-deb-builder:${REL}"
done

echo -e "\nBuilt package(s):"
for REL in "${RELEASES[@]}"; do
    ls -la "${OUT_DIR}/${REL}"/*.deb
done

# eof
