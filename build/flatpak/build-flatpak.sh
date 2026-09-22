#!/usr/bin/env bash
#
# MindForger thinking notebook
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# MindForger Flatpak package build script
#
# Builds the Flatpak application from build/flatpak/com.mindforger.MindForger.yaml and
# exports a single-file .flatpak bundle to distro/flatpak/ - that bundle is the artifact
# uploaded to a GitHub Release (this is NOT the Flathub-hosted build).
#
# Prerequisites:
#   sudo apt install flatpak flatpak-builder   # or dnf/pacman/zypper equivalent
#   flatpak remote-add --if-not-exists --user flathub \
#       https://flathub.org/repo/flathub.flatpakrepo
#   flatpak install --user flathub \
#       org.kde.Platform//5.15-24.08 org.kde.Sdk//5.15-24.08 \
#       io.qt.qtwebengine.BaseApp//5.15-24.08
#
# Usage:
#   ./build/flatpak/build-flatpak.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

APP_ID="com.mindforger.MindForger"
MANIFEST="${SCRIPT_DIR}/${APP_ID}.yaml"
BUILD_DIR="${SCRIPT_DIR}/build-dir"
REPO_DIR="${SCRIPT_DIR}/repo"
OUT_DIR="${PROJECT_ROOT}/distro/flatpak"

# lib/src/app_info.h (MINDFORGER_VERSION_STRING) is the authoritative version source
MINDFORGER_VERSION="$(sed -n 's/.*MINDFORGER_VERSION_STRING "\(.*\)"/\1/p' "${PROJECT_ROOT}/lib/src/app_info.h")"
if [ -z "${MINDFORGER_VERSION}" ]
then
    echo "ERROR: MindForger version not found in lib/src/app_info.h"
    exit 1
fi
echo "MindForger version: ${MINDFORGER_VERSION}"

# the manifest builds from the working tree ('type: dir'), so the git submodules with
# the cmark-gfm dependency (deps/cmark-gfm) and the default repository content (doc/)
# must be checked out - an un-initialized submodule would silently produce a bundle
# w/o the default memory and stencils
if [ ! -f "${PROJECT_ROOT}/deps/cmark-gfm/CMakeLists.txt" ] || [ ! -d "${PROJECT_ROOT}/doc/memory" ]
then
    echo "Updating Git submodules (deps/cmark-gfm, doc)..."
    git -C "${PROJECT_ROOT}" submodule update --init --recursive
fi

mkdir -p "${OUT_DIR}"

echo "Building Flatpak application..."
flatpak-builder --user --force-clean \
    --state-dir="${SCRIPT_DIR}/.flatpak-builder" \
    --repo="${REPO_DIR}" "${BUILD_DIR}" "${MANIFEST}"

# sanity check of the built application tree: a missing default repository content or
# a desktop/metainfo/icon file which is not app-ID-prefixed (and thus NOT exported by
# Flatpak) would otherwise only show up when a user installs the bundle
echo "Verifying built application tree..."
FILES_DIR="${BUILD_DIR}/files"
VERIFY_FAILED="false"

function verifyExists {
    if [ -e "${FILES_DIR}/${1}" ]
    then
        echo "  DONE  ${1}"
    else
        echo "  ERROR missing: ${1}"
        VERIFY_FAILED="true"
    fi
}

function verifyAbsent {
    if [ -e "${FILES_DIR}/${1}" ]
    then
        echo "  ERROR should have been cleaned up: ${1}"
        VERIFY_FAILED="true"
    else
        echo "  DONE  cleaned up: ${1}"
    fi
}

# the application, its launcher and the Qt WebEngine render process
verifyExists "bin/mindforger"
verifyExists "bin/mindforger-flatpak-launcher"
verifyExists "bin/QtWebEngineProcess"
# the exported desktop integration - these names MUST be app-ID-prefixed
verifyExists "share/applications/${APP_ID}.desktop"
verifyExists "share/metainfo/${APP_ID}.metainfo.xml"
verifyExists "share/icons/hicolor/128x128/apps/${APP_ID}.png"
verifyExists "share/icons/hicolor/scalable/apps/${APP_ID}.svg"
# the default repository content seeded by bin/mindforger-flatpak-launcher
verifyExists "share/doc/mindforger/memory"
verifyExists "share/doc/mindforger/stencils"
verifyExists "share/man/man1/mindforger.1"
# build-time only relics which must NOT be shipped
verifyAbsent "lib/libhunspell.so"
verifyAbsent "share/applications/mindforger.desktop"
verifyAbsent "share/metainfo/com.mindforger.mindforger.metainfo.xml"

if [ "${VERIFY_FAILED}" = "true" ]
then
    echo "ERROR Flatpak application tree verification failed - bundle NOT exported"
    exit 1
fi

echo "Exporting single-file bundle..."
BUNDLE="${OUT_DIR}/mindforger-${MINDFORGER_VERSION}.flatpak"
# --runtime-repo embeds where to fetch the Qt 5.15 runtime from: when a user installs
# this standalone .flatpak and does not have org.kde.Platform//5.15-24.08, flatpak
# offers to pull it from Flathub instead of failing - the user does NOT need to have
# Flathub configured up front
flatpak build-bundle \
    --runtime-repo=https://dl.flathub.org/repo/flathub.flatpakrepo \
    "${REPO_DIR}" "${BUNDLE}" "${APP_ID}"

echo "DONE Flatpak bundle created: ${BUNDLE}"

# eof
