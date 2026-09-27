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
#   on the FIRST run - this takes hours, subsequent runs reuse Docker cache
# - the package is built from the GitHub release tag (pkgver in PKGBUILD),
#   NOT from the local working copy - it verifies what AUR users will get
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

MODE="${1:-}"

case "${MODE}" in
    srcinfo)
        docker build --target srcinfo -t "${IMAGE}:srcinfo" "${SCRIPT_DIR}"
        # container runs as builder (uid 1000) - write to a temp dir it can write to
        TMP_OUT="$(mktemp -d)"
        chmod 777 "${TMP_OUT}"
        docker run --rm \
            -v "${ARCH_DIR}:/arch:ro" \
            -v "${TMP_OUT}:/out" \
            "${IMAGE}:srcinfo" srcinfo
        cp -f "${TMP_OUT}/.SRCINFO" "${ARCH_DIR}/.SRCINFO"
        rm -rf "${TMP_OUT}"
        echo "DONE: ${ARCH_DIR}/.SRCINFO"
        ;;

    pkg)
        OUT_DIR="${2:-${REPO_ROOT}/../mindforger-arch}"
        mkdir -p "${OUT_DIR}"
        chmod 777 "${OUT_DIR}"
        OUT_DIR="$(cd "${OUT_DIR}" && pwd)"
        # drop packages of previous builds so that a stale package can never be
        # mistaken for the result of this build
        rm -vf "${OUT_DIR}"/mindforger-*.pkg.tar.zst

        echo "Repo   : ${REPO_ROOT}"
        echo "Output : ${OUT_DIR}"

        docker build --target pkg -t "${IMAGE}:pkg" "${SCRIPT_DIR}"
        docker run --rm \
            -v "${ARCH_DIR}:/arch:ro" \
            -v "${OUT_DIR}:/out" \
            "${IMAGE}:pkg" pkg

        echo -e "\nBuilt package:"
        ls -la "${OUT_DIR}"/mindforger-*.pkg.tar.zst
        ;;

    *)
        echo "ERROR: usage: build-pkg.sh srcinfo | pkg [output-dir]"
        exit 1
        ;;
esac

# eof
