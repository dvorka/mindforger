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

# verifies the Arch Linux reference package build/arch/PKGBUILD using Docker,
# so no Arch Linux machine is needed.
#
# - pkg mode builds the Qt WebKit chain from AUR (like an Arch user has to)
#   on the FIRST run - ~40 min on 16 CPU cores, subsequent runs reuse Docker cache
# - the package is built from the GitHub release tag (pkgver in PKGBUILD),
#   NOT from the local working copy - it verifies what AUR users will get
# - results are copied out of the container using docker cp, therefore they
#   are owned by the invoking user and no output directory is bind-mounted
#
# usage:
#   ./build-pkg.sh srcinfo              ... regenerate build/arch/.SRCINFO
#   ./build-pkg.sh pkg [output-dir]     ... build, namcap and install package
#                                           default output-dir: ../mindforger-arch
#
# examples:
#   ./build-pkg.sh srcinfo
#   ./build-pkg.sh pkg ~/mindforger-arch

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../../.." && pwd)"
ARCH_DIR="${REPO_ROOT}/build/arch"
IMAGE=mindforger-arch-builder
# container-build.sh writes its results here
CONTAINER_OUT=/home/builder/out

CID=""
cleanup() {
    if [ -n "${CID}" ]; then
        docker rm -f "${CID}" >/dev/null
    fi
}
trap cleanup EXIT

# builds the image stage MODE (srcinfo or pkg), runs container-build.sh MODE
# in it and copies the results to the host directory DST_DIR
run_container() {
    local MODE="$1"
    local DST_DIR="$2"

    docker build --target "${MODE}" -t "${IMAGE}:${MODE}" "${SCRIPT_DIR}"
    CID="$(docker create -v "${ARCH_DIR}:/arch:ro" "${IMAGE}:${MODE}" "${MODE}")"
    # docker start -a exits w/ the exit code of the container
    docker start -a "${CID}"
    docker cp "${CID}:${CONTAINER_OUT}/." "${DST_DIR}/"
}

MODE="${1:-}"

case "${MODE}" in
    srcinfo)
        run_container srcinfo "${ARCH_DIR}"
        echo "DONE: ${ARCH_DIR}/.SRCINFO"
        ;;

    pkg)
        OUT_DIR="${2:-${REPO_ROOT}/../mindforger-arch}"
        mkdir -p "${OUT_DIR}"
        OUT_DIR="$(cd "${OUT_DIR}" && pwd)"
        # drop packages of previous builds so that a stale package can never be
        # mistaken for the result of this build
        rm -vf "${OUT_DIR}"/mindforger-*.pkg.tar.zst

        echo "Repo   : ${REPO_ROOT}"
        echo "Output : ${OUT_DIR}"

        run_container pkg "${OUT_DIR}"

        echo -e "\nBuilt package:"
        ls -la "${OUT_DIR}"/mindforger-*.pkg.tar.zst
        ;;

    *)
        echo "ERROR: usage: build-pkg.sh srcinfo | pkg [output-dir]"
        exit 1
        ;;
esac

# eof
