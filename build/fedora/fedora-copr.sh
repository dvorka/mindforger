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

# builds MindForger for Fedora - locally in Docker and in Fedora COPR
# https://copr.fedorainfracloud.org/coprs/dvorka/mindforger/
#
# - COPR hosts the dnf repository and signs packages - there is no local
#   repository to manage (unlike Debian PPA)
# - Fedora releases are listed in fedora-releases.conf, configuration is in
#   fedora-config.sh
# - Linux host w/ GNU tools (tar, date) and Docker is needed: rpmbuild and
#   copr-cli run in the build/docker/fedora-rpm/ toolbox container
#
# usage:
#   ./fedora-copr.sh srpm
#   ./fedora-copr.sh rpm         [number|supported]
#   ./fedora-copr.sh release
#   ./fedora-copr.sh verify      [number|supported]
#   ./fedora-copr.sh token-check
#   ./fedora-copr.sh copr-cli    <copr-cli arguments>
#
# examples:
#   ./fedora-copr.sh rpm 44
#   ./fedora-copr.sh copr-cli list-chroots
#   MF_RPM_RELEASE=2 ./fedora-copr.sh release

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
DOCKER_DIR="${REPO_ROOT}/build/docker/fedora-rpm"

# ############################################################################
# # Configuration - see fedora-config.sh #
# ############################################################################

# shellcheck source=fedora-config.sh
source "${SCRIPT_DIR}/fedora-config.sh"

# RPM release - bump only when re-spinning packaging of the same MF version
export MF_RPM_RELEASE="${MF_RPM_RELEASE:-1}"

# warn when the COPR API token expires in less than given number of days
MF_TOKEN_WARN_DAYS=30

# ############################################################################
# # Helpers #
# ############################################################################

function info {
    echo "${*}"
}

function warn {
    echo "WARNING: ${*}" >&2
}

function die {
    echo "ERROR: ${*}" >&2
    exit 1
}

function require_cmd {
    for C in "${@}"; do
        command -v "${C}" > /dev/null || die "'${C}' command not found"
    done
}

# print MindForger version from lib/src/app_info.h (the authoritative source)
function mf_version {
    grep MINDFORGER_VERSION_STRING "${REPO_ROOT}/lib/src/app_info.h" | sed -E 's/.*"([^"]+)".*/\1/'
}

# print Fedora release numbers for "number" or "supported" argument
function target_releases {
    local target="${1:-supported}"
    if [ "${target}" = "supported" ]; then
        mf_fedora_releases supported
    else
        mf_fedora_is_release "${target}" || die "unknown Fedora release '${target}' - see ${MF_FEDORA_RELEASES_CONF}"
        echo "${target}"
    fi
}

# the newest supported Fedora release - its toolbox image runs copr-cli
function default_release {
    mf_fedora_releases supported | head -1
}

# build toolbox image for the Fedora release and print its name
function toolbox_image {
    local number="${1}"
    local image="mindforger-rpm-builder:${number}"
    docker build -q --build-arg "FEDORA_RELEASE=${number}" -t "${image}" "${DOCKER_DIR}" > /dev/null \
        || die "failed to build ${image} Docker image"
    echo "${image}"
}

# run given command (entrypoint + arguments) in the toolbox container w/ the
# COPR API token - as the host user so that downloaded files are not owned
# by root
function copr_run {
    local entrypoint="${1}"
    local image
    shift
    [ -f "${MF_FEDORA_COPR_CONFIG}" ] \
        || die "COPR API token ${MF_FEDORA_COPR_CONFIG} not found - get it from https://copr.fedorainfracloud.org/api/ (see build/fedora/README.md)"
    image="$(toolbox_image "$(default_release)")"
    mkdir -p "${MF_FEDORA_RPM_DIR}"
    docker run --rm \
        --user "$(id -u):$(id -g)" \
        -e HOME=/tmp \
        -v "${MF_FEDORA_COPR_CONFIG}:/copr:ro" \
        -v "${MF_FEDORA_RPM_DIR}:/out" \
        --entrypoint "${entrypoint}" \
        "${image}" "${@}"
}

# run copr-cli w/ given arguments in the toolbox container
function copr_cli {
    copr_run copr-cli --config /copr "${@}"
}

# ############################################################################
# # Commands #
# ############################################################################

# source tarball of Git tracked files incl. submodules + SRPM
function cmd_srpm {
    local version tarball image
    require_cmd git docker tar
    mf_fedora_print_config
    version="$(mf_version)"
    tarball="${MF_FEDORA_RPM_DIR}/mindforger-${version}.tar.gz"

    # uninitialized submodules are listed by `git ls-files` as empty dirs -
    # the tarball would be silently incomplete (no cmark-gfm, no doc)
    ! git -C "${REPO_ROOT}" submodule status --recursive | grep -q '^-' \
        || die "Git submodules are not initialized - run: git submodule update --init --recursive"

    mkdir -p "${MF_FEDORA_RPM_DIR}"
    # drop artifacts of previous builds so that a stale one can never be
    # mistaken for the result of this build
    rm -vf "${MF_FEDORA_RPM_DIR}"/mindforger-*.tar.gz "${MF_FEDORA_RPM_DIR}"/mindforger-*.src.rpm

    info "= Source tarball ${tarball} ======="
    # - NUL separated - Git would quote non-ASCII file names otherwise
    # - files deleted in a dirty working tree (development builds) are skipped
    #   w/ a warning - release requires a clean working tree
    git -C "${REPO_ROOT}" ls-files -z --recurse-submodules \
        | tar -C "${REPO_ROOT}" -czf "${tarball}" \
            --transform "s,^,mindforger-${version}/," --ignore-failed-read --null -T -

    image="$(toolbox_image "$(default_release)")"
    docker run --rm \
        -v "${REPO_ROOT}:/src:ro" \
        -v "${MF_FEDORA_RPM_DIR}:/out" \
        -e MF_RPM_RELEASE \
        "${image}" srpm
}

# SRPM rebuilt to .rpm in clean fedora:<number> container(s)
function cmd_rpm {
    local number image releases
    # plain assignment - set -e stops here on unknown release
    releases="$(target_releases "${1:-supported}")"

    cmd_srpm
    for number in ${releases}; do
        info "= Fedora ${number} ======="
        # pre-create the output dir so that it is owned by the user (not root)
        mkdir -p "${MF_FEDORA_RPM_DIR}/fc${number}"
        rm -vf "${MF_FEDORA_RPM_DIR}/fc${number}"/mindforger-*.rpm
        image="$(toolbox_image "${number}")"
        docker run --rm \
            -v "${REPO_ROOT}:/src:ro" \
            -v "${MF_FEDORA_RPM_DIR}:/out" \
            -e MF_RPM_RELEASE \
            "${image}" rpm
    done

    info "DONE: built package(s):"
    for number in ${releases}; do
        ls -la "${MF_FEDORA_RPM_DIR}/fc${number}"/mindforger-*.rpm
    done
}

# build SRPM in COPR for all supported chroots and download the .rpm packages
function cmd_release {
    local version nvr build_id chroot
    local chroot_args=()
    require_cmd git docker
    version="$(mf_version)"
    nvr="mindforger-${version}-${MF_RPM_RELEASE}"

    # release SRPM must be exactly the committed source
    [ -z "$(git -C "${REPO_ROOT}" status --porcelain --untracked-files=no --ignore-submodules=none)" ] \
        || die "working tree is not clean - commit or stash changes (incl. submodules) first"

    cmd_token_check
    cmd_srpm

    for chroot in $(mf_fedora_chroots); do
        chroot_args+=(--chroot "${chroot}")
    done
    [ "${#chroot_args[@]}" -gt 0 ] || die "no supported Fedora release in ${MF_FEDORA_RELEASES_CONF}"

    info "= COPR build ${nvr} in ${MF_FEDORA_COPR_PROJECT}: $(mf_fedora_chroots | tr '\n' ' ') ======="
    build_id="$(copr_cli build --nowait "${chroot_args[@]}" "${MF_FEDORA_COPR_PROJECT}" "/out/${nvr}.src.rpm" \
        | tee /dev/stderr | sed -n 's/^Created builds: \([0-9]*\).*/\1/p')"
    [ -n "${build_id}" ] || die "COPR build was not created"
    info "COPR build: https://copr.fedorainfracloud.org/coprs/${MF_FEDORA_COPR_PROJECT}/build/${build_id}/"
    copr_cli watch-build "${build_id}" || die "COPR build ${build_id} failed - see its logs at the URL above"

    # .rpm packages for the GitHub release - the very same COPR build (logs,
    # SRPMs and debuginfo packages are not needed)
    info "= Downloading COPR build ${build_id} packages ======="
    mkdir -p "${MF_FEDORA_RPM_DIR}/copr"
    rm -vf "${MF_FEDORA_RPM_DIR}/copr"/mindforger-*.rpm
    rm -rf "${MF_FEDORA_RPM_DIR}/copr-build"
    copr_cli download-build --rpms --dest /out/copr-build "${build_id}"
    find "${MF_FEDORA_RPM_DIR}/copr-build" -name "${nvr}.fc*.rpm" ! -name '*.src.rpm' \
        -exec cp -v {} "${MF_FEDORA_RPM_DIR}/copr/" \;
    rm -rf "${MF_FEDORA_RPM_DIR}/copr-build"
    ls "${MF_FEDORA_RPM_DIR}/copr/${nvr}".fc*.rpm > /dev/null 2>&1 || die "no .rpm package downloaded from COPR"

    info "DONE: MindForger ${version} built in COPR, packages for the GitHub release:"
    ls -la "${MF_FEDORA_RPM_DIR}/copr"
}

# install MindForger from COPR in a clean fedora:<number> container - checks
# repository, signature, dependencies, version and dynamic linking
function verify_release {
    local number="${1}"
    info "= Verifying COPR ${MF_FEDORA_COPR_PROJECT} in fedora:${number} container ======="
    if docker run --rm \
        -e "MF_COPR_PROJECT=${MF_FEDORA_COPR_PROJECT}" \
        -e "MF_EXPECTED=$(mf_version)" \
        "fedora:${number}" \
        bash -c '
            set -euo pipefail
            dnf install -q -y "dnf-command(copr)" > /dev/null
            dnf copr enable -y "${MF_COPR_PROJECT}"
            dnf install -q -y mindforger > /dev/null
            INSTALLED="$(rpm -q --qf "%{VERSION}" mindforger)"
            echo "installed mindforger ${INSTALLED} (expected ${MF_EXPECTED})"
            [ "${INSTALLED}" = "${MF_EXPECTED}" ]
            if ldd /usr/bin/mindforger | grep "not found"; then
                echo "ERROR: unresolved shared libraries"
                exit 1
            fi
        '
    then
        info "DONE: Fedora ${number} COPR verified"
    else
        die "Fedora ${number} COPR verification failed"
    fi
}

function cmd_verify {
    local number releases
    require_cmd docker
    # plain assignment - set -e stops here on unknown release
    releases="$(target_releases "${1:-supported}")"
    for number in ${releases}; do
        verify_release "${number}"
    done
}

# COPR API tokens expire - check the token before it is needed
function cmd_token_check {
    local expiration days
    require_cmd docker
    mf_fedora_print_config
    # `copr-cli whoami` only prints the user name from the token file - call
    # an endpoint protected by login instead
    copr_run python3 -c 'from copr.v3 import Client; Client.create_from_config_file("/copr").base_proxy.auth_check()' 2> /dev/null \
        || die "COPR API token is invalid or expired - renew it at https://copr.fedorainfracloud.org/api/ and save it to ${MF_FEDORA_COPR_CONFIG}"

    # the token file downloaded from COPR has "# expiration date: YYYY-MM-DD" comment
    expiration="$(sed -n 's/^#[[:space:]]*expiration date:[[:space:]]*\([0-9-]*\).*/\1/p' "${MF_FEDORA_COPR_CONFIG}")"
    if [ -n "${expiration}" ]; then
        days=$(( ( $(date -d "${expiration}" +%s) - $(date +%s) ) / 86400 ))
        if [ "${days}" -lt "${MF_TOKEN_WARN_DAYS}" ]; then
            warn "COPR API token expires in ${days} days (${expiration}) - renew it at https://copr.fedorainfracloud.org/api/"
        fi
        info "DONE: COPR API token is valid, expires ${expiration} (in ${days} days)"
    else
        info "DONE: COPR API token is valid (expiration date unknown)"
    fi
}

function usage {
    sed -n '/^# usage:/,/^$/p' "${BASH_SOURCE[0]}" | sed -e 's/^# \{0,1\}//'
    exit 1
}

# ############################################################################
# # Main #
# ############################################################################

COMMAND="${1:-}"
shift || true

case "${COMMAND}" in
    srpm)        cmd_srpm ;;
    rpm)         cmd_rpm "${@}" ;;
    release)     cmd_release ;;
    verify)      cmd_verify "${@}" ;;
    token-check) cmd_token_check ;;
    copr-cli)    require_cmd docker; copr_cli "${@}" ;;
    *)           usage ;;
esac

# eof
