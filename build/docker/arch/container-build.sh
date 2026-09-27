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

# runs INSIDE the mindforger-arch-builder container - see build-pkg.sh.
# expects build/arch read-only mounted @ /arch and an output dir @ /out.
#
# usage:
#   container-build.sh srcinfo ... write /out/.SRCINFO generated from PKGBUILD
#   container-build.sh pkg     ... build, check (namcap) and install the package,
#                                  copy it to /out

set -euo pipefail

ARCH_DIR=/arch
OUT=/out
WORK="${HOME}/mindforger-pkg"
MODE="${1:-pkg}"

rm -rf "${WORK}"
mkdir -p "${WORK}"
cp "${ARCH_DIR}/PKGBUILD" "${WORK}/"
cd "${WORK}"

case "${MODE}" in
    srcinfo)
        makepkg --printsrcinfo > "${OUT}/.SRCINFO"
        echo "DONE: .SRCINFO generated"
        ;;

    pkg)
        echo "# Build MindForger package #####################################"
        START=$(date +%s)
        makepkg --syncdeps --noconfirm
        echo "DONE: package built in $(( ($(date +%s) - START) / 60 )) min"

        PKG_FILE="$(ls mindforger-[0-9]*.pkg.tar.zst)"

        echo -e "\n# namcap: PKGBUILD ############################################"
        namcap PKGBUILD || true
        echo -e "\n# namcap: ${PKG_FILE} ############################################"
        NAMCAP_OUT="$(namcap "${PKG_FILE}")"
        echo "${NAMCAP_OUT}"
        if echo "${NAMCAP_OUT}" | grep -q " E: "; then
            echo "ERROR: namcap reported errors for ${PKG_FILE}"
            exit 1
        fi

        echo -e "\n# Install and check ###########################################"
        sudo pacman -U --noconfirm "${PKG_FILE}"
        for F in \
            /usr/bin/mindforger \
            /usr/share/applications/mindforger.desktop \
            /usr/share/icons/hicolor/scalable/apps/mindforger.svg \
            /usr/share/icons/hicolor/128x128/apps/mindforger128x128.png \
            /usr/share/metainfo/com.mindforger.mindforger.metainfo.xml \
            /usr/share/man/man1/mindforger.1.gz
        do
            if [ ! -e "${F}" ]; then
                echo "ERROR: ${F} is not installed"
                exit 1
            fi
            echo "  ${F}"
        done
        if ldd /usr/bin/mindforger | grep -q "not found"; then
            ldd /usr/bin/mindforger | grep "not found"
            echo "ERROR: /usr/bin/mindforger has unresolved shared libraries"
            exit 1
        fi
        if ! ldd /usr/bin/mindforger | grep -q "libQt5WebKit"; then
            echo "ERROR: /usr/bin/mindforger is not linked against Qt WebKit"
            exit 1
        fi

        cp -v "${PKG_FILE}" "${OUT}/"
        echo "DONE: ${PKG_FILE} built, checked and installed"
        ;;

    *)
        echo "ERROR: unknown mode '${MODE}' - use srcinfo or pkg"
        exit 1
        ;;
esac

# eof
