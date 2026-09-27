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

# runs INSIDE the mindforger-rpm-builder:<release> container - see
# build/fedora/fedora-copr.sh. expects the repository read-only mounted
# @ /src and the output dir @ /out (w/ the source tarball for srpm, w/ the
# SRPM for rpm).
#
# usage:
#   container-build.sh srpm ... /out/mindforger-V.tar.gz -> /out/mindforger-V-R.src.rpm
#   container-build.sh rpm  ... /out/mindforger-V-R.src.rpm -> /out/fcNN/mindforger-V-R.fcNN.<arch>.rpm

set -euo pipefail

SRC=/src
OUT=/out
WORK=/build/work

MODE="${1:-}"
MFVERSION=$(grep MINDFORGER_VERSION_STRING "${SRC}/lib/src/app_info.h" | sed -E 's/.*"([^"]+)".*/\1/')
MF_RPM_RELEASE="${MF_RPM_RELEASE:-1}"
MF_NVR="mindforger-${MFVERSION}-${MF_RPM_RELEASE}"
SRPM="${OUT}/${MF_NVR}.src.rpm"

function die {
    echo "ERROR: ${*}" >&2
    exit 1
}

# spec copy w/ version, release and %changelog entry for this exact version
# (the checked-in template is never touched)
function build_srpm {
    local tarball="${OUT}/mindforger-${MFVERSION}.tar.gz"
    local spec="${WORK}/mindforger.spec"
    local ts

    echo "# Building MindForger ${MFVERSION} SRPM ##################"
    [ -f "${tarball}" ] || die "source tarball ${tarball} not found"

    rm -rf "${WORK}"
    mkdir -p "${WORK}"
    sed -e "s/@MF_VERSION@/${MFVERSION}/" \
        -e "s/@MF_RPM_RELEASE@/${MF_RPM_RELEASE}/" \
        "${SRC}/build/fedora/mindforger.spec" > "${spec}"
    ts=$(LC_ALL=C date "+%a %b %d %Y")
    {
        echo "* ${ts} Martin Dvorak (Dvorka) <martin.dvorak@mindforger.com> - ${MFVERSION}-${MF_RPM_RELEASE}"
        echo "- MindForger ${MFVERSION} release."
    } >> "${spec}"

    # SRPM is Fedora release independent - no dist tag in its name
    rpmbuild -bs \
        --define "dist %{nil}" \
        --define "_sourcedir ${OUT}" \
        --define "_srcrpmdir ${OUT}" \
        "${spec}"
    [ -f "${SRPM}" ] || die "no SRPM produced"
    echo -e "\nDONE: ${SRPM}"
}

# rebuild the SRPM (not the source tree) - the exact artifact uploaded to COPR
function build_rpm {
    local dist rpms_dir out_dir
    dist="fc$(. /etc/os-release && echo "${VERSION_ID}")"
    rpms_dir="${WORK}/rpms"
    out_dir="${OUT}/${dist}"

    echo "# Building MindForger ${MFVERSION} .rpm for ${dist} ##################"
    [ -f "${SRPM}" ] || die "SRPM ${SRPM} not found"

    rm -rf "${WORK}"
    mkdir -p "${rpms_dir}" "${out_dir}"
    dnf builddep -y "${SRPM}"
    rpmbuild --rebuild --define "_rpmdir ${rpms_dir}" "${SRPM}"

    # the binary package is mandatory (fail loudly), debuginfo packages are
    # not published
    find "${rpms_dir}" -name "${MF_NVR}.${dist}.*.rpm" -exec cp -v {} "${out_dir}/" \;
    ls "${out_dir}/${MF_NVR}.${dist}".*.rpm > /dev/null 2>&1 || die "no .rpm package produced"
    echo -e "\nDONE: package(s) written to ${out_dir}"
    ls -la "${out_dir}"
}

case "${MODE}" in
    srpm) build_srpm ;;
    rpm)  build_rpm ;;
    *)    die "usage: container-build.sh <srpm|rpm>" ;;
esac

# eof
