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

# manages MindForger Debian PPAs - one PPA per Debian release - which are
# uploaded to https://www.mindforger.com/debian-ppa/<codename>/
#
# - the .deb ARCHIVE is the source of truth: add/update/remove change the
#   archive and then the PPA of the Debian release is REBUILT from scratch
#   using aptly in a throwaway aptly root (no persistent ~/.aptly state)
# - Debian releases are listed in debian-releases.conf
# - see README.md in this directory for the whole release process
#
# usage:
#   ./debian-ppa.sh add       <codename> <version>
#   ./debian-ppa.sh update    <codename> <version>
#   ./debian-ppa.sh remove    <codename> <version>
#   ./debian-ppa.sh release   <version>
#   ./debian-ppa.sh sync      <codename|all>
#   ./debian-ppa.sh list      [codename]
#   ./debian-ppa.sh index
#   ./debian-ppa.sh verify    <codename|supported>
#   ./debian-ppa.sh key-check
#
# examples:
#   ./debian-ppa.sh add trixie 2.5.0
#   MF_DEBIAN_PPA_DIR=/tmp/ppa ./debian-ppa.sh sync all

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

# shellcheck source=debian-releases.sh
source "${SCRIPT_DIR}/debian-releases.sh"

# ############################################################################
# # Configuration - see debian-config.sh #
# ############################################################################

# shellcheck source=debian-config.sh
source "${SCRIPT_DIR}/debian-config.sh"

# RSA keys shorter than 3072 bits are accepted by Debian apt (sqv) until this date
MF_RSA2048_DEADLINE="2030-02-01"

# temporary directories to be deleted on exit
MF_TMP_DIRS=()

function cleanup {
    for D in "${MF_TMP_DIRS[@]+"${MF_TMP_DIRS[@]}"}"; do
        rm -rf "${D}"
    done
}
trap cleanup EXIT

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

function new_tmp_dir {
    local tmp_dir
    tmp_dir="$(mktemp -d)"
    MF_TMP_DIRS+=("${tmp_dir}")
    echo "${tmp_dir}"
}

function require_cmd {
    for C in "${@}"; do
        command -v "${C}" > /dev/null || die "'${C}' command not found - see README.md prerequisites"
    done
}

function require_release {
    [ -n "${1:-}" ] || die "Debian release codename not specified"
    mf_debian_is_release "${1}" || die "unknown Debian release '${1}' - see ${MF_DEBIAN_RELEASES_CONF}"
}

function require_supported_release {
    require_release "${1}"
    [ "$(mf_debian_release_status "${1}")" = "supported" ] \
        || die "Debian release '${1}' is not supported (frozen) - no new MindForger versions can be added"
}

function require_version {
    [[ "${1:-}" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || die "invalid MindForger version '${1:-}' - expected MAJOR.MINOR.PATCH"
}

# print .deb file name for given MindForger version
function deb_name {
    echo "mindforger_${1}-1_amd64.deb"
}

# print archive dir of the Debian release e.g. .../13-trixie (existing
# dir w/ any number prefix wins, otherwise zero padded number is used)
function archive_dir {
    local codename="${1}"
    local dir
    for dir in "${MF_DEBIAN_ARCHIVE_DIR}"/*-"${codename}"; do
        if [ -d "${dir}" ]; then
            echo "${dir}"
            return
        fi
    done
    printf "%s/%02d-%s\n" "${MF_DEBIAN_ARCHIVE_DIR}" "$(mf_debian_release_number "${codename}")" "${codename}"
}

# print MindForger versions in the archive of the Debian release (sorted)
function archive_versions {
    local dir
    dir="$(archive_dir "${1}")"
    [ -d "${dir}" ] || return 0
    find "${dir}" -maxdepth 1 -name 'mindforger_*-1_amd64.deb' -printf '%f\n' \
        | sed -E 's/^mindforger_([^-]+)-1_amd64\.deb$/\1/' \
        | sort -V
}

# print MindForger versions published in the PPA of the Debian release
function ppa_versions {
    local packages="${MF_DEBIAN_PPA_DIR}/${1}/dists/${1}/main/binary-amd64/Packages"
    [ -f "${packages}" ] || return 0
    sed -n 's/^Version: //p' "${packages}" | sort -V
}

# export signing public key to binary keyring file (readable by apt's _apt)
function export_keyring {
    gpg --export --export-options export-minimal "${MF_DEBIAN_GPG_KEY}" > "${1}"
    [ -s "${1}" ] || die "GPG key ${MF_DEBIAN_GPG_KEY} not found in the keyring"
    chmod 644 "${1}"
}

# the master archive MUST exist - it is never created, because PPAs rebuilt
# from an empty/wrong archive would DROP all previously released versions
function require_archive_root {
    [ -d "${MF_DEBIAN_ARCHIVE_DIR}" ] || die "master .deb archive ${MF_DEBIAN_ARCHIVE_DIR} not found - set MF_DEBIAN_ARCHIVE_DIR to its location (see README.md)"
}

# make pinentry work when run from a terminal
if [ -t 0 ]; then
    GPG_TTY="$(tty)"
    export GPG_TTY
fi

# ############################################################################
# # Index #
# ############################################################################

# generate index.html for the PPA of the Debian release
function index_release {
    local codename="${1}"
    sed -e "s/DISTRO/${codename}/g" "${SCRIPT_DIR}/index-ppa.html" \
        > "${MF_DEBIAN_PPA_DIR}/${codename}/index.html"
}

# generate index.html for all PPAs w/ links to the published releases
function cmd_index {
    local codename items=""
    info "WRITE: index.html pages in ${MF_DEBIAN_PPA_DIR}"
    mkdir -p "${MF_DEBIAN_PPA_DIR}"
    for codename in $(mf_debian_releases); do
        [ -d "${MF_DEBIAN_PPA_DIR}/${codename}/dists" ] || continue
        index_release "${codename}"
        local item="<li><a href=\"${codename}/index.html\">Debian $(mf_debian_release_number "${codename}") ${codename^}</a>"
        if [ "$(mf_debian_release_status "${codename}")" = "frozen" ]; then
            item="${item} (frozen - no new MindForger versions)"
        fi
        items="${items}          ${item}</li>\n"
    done
    items="${items%\\n}"
    # GNU sed turns \n in the replacement to new line
    sed -e "s|RELEASES|${items}|" "${SCRIPT_DIR}/index-all-ppas.html" \
        > "${MF_DEBIAN_PPA_DIR}/index.html"
    info "DONE: index.html pages generated in ${MF_DEBIAN_PPA_DIR}"
}

# ############################################################################
# # PPA rebuild #
# ############################################################################

# rebuild the PPA of the Debian release from the archive using aptly
function sync_release {
    local codename="${1}"
    local dir aptly_root aptly_conf repo
    dir="$(archive_dir "${codename}")"
    repo="mindforger-${codename}"

    if [ -z "$(archive_versions "${codename}")" ]; then
        die "no .deb packages for ${codename} in ${dir}"
    fi

    info "= Rebuilding ${codename} PPA ========================================"
    info "  archive : ${dir}"
    info "  PPA     : ${MF_DEBIAN_PPA_DIR}/${codename}"
    info "  versions: $(archive_versions "${codename}" | tr '\n' ' ')"
    info "  GPG key : ${MF_DEBIAN_GPG_KEY}"

    aptly_root="$(new_tmp_dir)"
    aptly_conf="${aptly_root}/aptly.conf"
    cat > "${aptly_conf}" <<EOF
{
  "rootDir": "${aptly_root}",
  "architectures": ["amd64"],
  "gpgDisableSign": false,
  "gpgDisableVerify": false,
  "gpgProvider": "gpg",
  "skipContentsPublishing": false
}
EOF

    aptly -config="${aptly_conf}" repo create \
        -distribution="${codename}" -component=main "${repo}"
    aptly -config="${aptly_conf}" repo add "${repo}" "${dir}"
    # prefix <codename> > public/<codename>/{dists,pool} i.e. the web layout
    aptly -config="${aptly_conf}" publish repo \
        -architectures=amd64 \
        -origin=MindForger \
        -label=MindForger \
        -gpg-key="${MF_DEBIAN_GPG_KEY}" \
        "${repo}" "${codename}"

    info "WRITE: replacing PPA ${MF_DEBIAN_PPA_DIR}/${codename}"
    rm -rf "${MF_DEBIAN_PPA_DIR:?}/${codename}"
    mkdir -p "${MF_DEBIAN_PPA_DIR}"
    cp -r "${aptly_root}/public/${codename}" "${MF_DEBIAN_PPA_DIR}/${codename}"
    index_release "${codename}"

    info "DONE: ${codename} PPA rebuilt w/ $(ppa_versions "${codename}" | tr '\n' ' ')"
}

function cmd_sync {
    local target="${1:-}"
    local codename
    require_cmd aptly gpg
    require_archive_root
    mf_debian_print_config
    if [ "${target}" = "all" ]; then
        for codename in $(mf_debian_releases); do
            if [ -z "$(archive_versions "${codename}")" ]; then
                warn "skipping ${codename} - no .deb packages in $(archive_dir "${codename}")"
                continue
            fi
            sync_release "${codename}"
        done
    else
        require_release "${target}"
        sync_release "${target}"
    fi
    cmd_index
}

# ############################################################################
# # Add / update / remove #
# ############################################################################

# copy .deb built by Docker to the archive
#
# parameters:
#   $1 - codename
#   $2 - version
#   $3 - mode: add (must not exist) | update (must exist)
function archive_put {
    local codename="${1}" version="${2}" mode="${3}"
    local src dir
    src="${MF_DEBIAN_DEB_DIR}/${codename}/$(deb_name "${version}")"
    dir="$(archive_dir "${codename}")"

    [ -f "${src}" ] || die "${src} not found - build it first: make distro-debian-deb DEBIAN_RELEASE=${codename}"
    dpkg-deb --field "${src}" Version | grep -qx "${version}-1" \
        || die "${src} is not MindForger ${version}-1 package"

    if [ "${mode}" = "add" ] && [ -f "${dir}/$(deb_name "${version}")" ]; then
        die "MindForger ${version} is already in ${codename} PPA - use update instead"
    fi
    if [ "${mode}" = "update" ] && [ ! -f "${dir}/$(deb_name "${version}")" ]; then
        die "MindForger ${version} is not in ${codename} PPA - use add instead"
    fi

    info "WRITE: MASTER archive ${mode}: ${src} > ${dir}/"
    mkdir -p "${dir}"
    cp -f "${src}" "${dir}/"
    info "DONE: ${dir}/$(deb_name "${version}")"
}

function cmd_add_or_update {
    local mode="${1}" codename="${2:-}" version="${3:-}"
    require_supported_release "${codename}"
    require_version "${version}"
    require_cmd aptly gpg dpkg-deb
    require_archive_root
    mf_debian_print_config
    archive_put "${codename}" "${version}" "${mode}"
    cmd_sync "${codename}"
}

function cmd_remove {
    local codename="${1:-}" version="${2:-}"
    local deb
    require_release "${codename}"
    require_version "${version}"
    require_cmd aptly gpg
    require_archive_root
    mf_debian_print_config
    deb="$(archive_dir "${codename}")/$(deb_name "${version}")"

    [ -f "${deb}" ] || die "MindForger ${version} is not in ${codename} PPA"
    [ "$(archive_versions "${codename}" | wc -l)" -gt 1 ] \
        || die "MindForger ${version} is the last package in ${codename} PPA - refusing to remove it"

    info "WRITE: MASTER archive remove: ${deb}"
    rm -f "${deb}"
    info "DONE: ${deb} removed"
    cmd_sync "${codename}"
}

# add (or update) the MindForger version to PPAs of ALL supported releases
function cmd_release {
    local version="${1:-}"
    local codename mode
    require_version "${version}"
    require_cmd aptly gpg dpkg-deb
    require_archive_root
    mf_debian_print_config
    for codename in $(mf_debian_releases supported); do
        mode="add"
        if archive_versions "${codename}" | grep -qx "${version}"; then
            mode="update"
        fi
        info "= ${mode} MindForger ${version} to ${codename} ======================"
        archive_put "${codename}" "${version}" "${mode}"
        sync_release "${codename}"
    done
    cmd_index
}

# ############################################################################
# # List #
# ############################################################################

function cmd_list {
    local codename
    local releases
    if [ -n "${1:-}" ]; then
        require_release "${1}"
        releases="${1}"
    else
        releases="$(mf_debian_releases)"
    fi
    mf_debian_print_config
    if [ ! -d "${MF_DEBIAN_ARCHIVE_DIR}" ]; then
        warn "master .deb archive ${MF_DEBIAN_ARCHIVE_DIR} not found"
    fi
    for codename in ${releases}; do
        info ""
        info "${codename} ($(mf_debian_release_number "${codename}"), $(mf_debian_release_status "${codename}")):"
        info "  archive dir: $(archive_dir "${codename}")"
        info "  PPA dir    : ${MF_DEBIAN_PPA_DIR}/${codename}"
        info "  archive    : $(archive_versions "${codename}" | tr '\n' ' ')"
        info "  published  : $(ppa_versions "${codename}" | tr '\n' ' ')"
    done
}

# ############################################################################
# # Verify #
# ############################################################################

# install MindForger from the local PPA in a clean Debian container using
# the release's own apt - checks signature (apt/sqv policy), dependencies
# and that the newest version is installed
function verify_release {
    local codename="${1}"
    local ppa="${MF_DEBIAN_PPA_DIR}/${codename}"
    local expected keyring_dir

    [ -f "${ppa}/dists/${codename}/InRelease" ] || die "${codename} PPA not found in ${ppa}"
    expected="$(ppa_versions "${codename}" | tail -1)"

    keyring_dir="$(new_tmp_dir)"
    chmod 755 "${keyring_dir}"
    export_keyring "${keyring_dir}/mindforger.gpg"

    info "= Verifying ${codename} PPA in debian:${codename} container =========="
    if docker run --rm \
        -v "${ppa}:/ppa:ro" \
        -v "${keyring_dir}:/keyring:ro" \
        -e "MF_CODENAME=${codename}" \
        -e "MF_EXPECTED=${expected}" \
        "debian:${codename}" \
        bash -c '
            set -euo pipefail
            echo "deb [signed-by=/keyring/mindforger.gpg] file:/ppa ${MF_CODENAME} main" \
                > /etc/apt/sources.list.d/mindforger.list
            apt-get -q update 2>&1 | tee /tmp/apt-update.log
            if grep -qE "^(W|E|Warning|Error):" /tmp/apt-update.log; then
                echo "ERROR: apt-get update reported warnings/errors"
                exit 1
            fi
            DEBIAN_FRONTEND=noninteractive apt-get -q install -y --no-install-recommends mindforger > /dev/null
            INSTALLED="$(dpkg-query -W -f="\${Version}" mindforger)"
            echo "installed mindforger ${INSTALLED} (expected ${MF_EXPECTED})"
            [ "${INSTALLED}" = "${MF_EXPECTED}" ]
        '
    then
        info "DONE: ${codename} PPA verified"
    else
        die "${codename} PPA verification failed"
    fi
}

function cmd_verify {
    local target="${1:-supported}"
    local codename
    require_cmd docker gpg
    if [ "${target}" = "supported" ]; then
        for codename in $(mf_debian_releases supported); do
            verify_release "${codename}"
        done
    else
        require_release "${target}"
        verify_release "${target}"
    fi
}

# ############################################################################
# # Signing key check #
# ############################################################################

# print number of SHA-1 self-signatures (user ID/subkey bindings) in
# the binary or armored key file
function sha1_self_sigs {
    gpg --list-packets < "${1}" 2>/dev/null | awk '
        /sigclass 0x1[0-3]|sigclass 0x18/ { binding = 1; next }
        /sigclass/                        { binding = 0; next }
        /digest algo 2,/ && binding       { count++ }
        END                               { print count + 0 }'
}

function cmd_key_check {
    local errors=0
    local tmp_dir pub_line validity key_length key_algo expires web_fpr web_sha1
    require_cmd gpg curl
    tmp_dir="$(new_tmp_dir)"
    chmod 755 "${tmp_dir}"

    info "= Signing key ${MF_DEBIAN_GPG_KEY} =================================="

    # public key
    if ! gpg --list-keys "${MF_DEBIAN_GPG_KEY}" > /dev/null 2>&1; then
        die "public key ${MF_DEBIAN_GPG_KEY} not found in the local keyring"
    fi
    gpg --list-keys --keyid-format long "${MF_DEBIAN_GPG_KEY}"

    # secret key
    if gpg --list-secret-keys "${MF_DEBIAN_GPG_KEY}" > /dev/null 2>&1; then
        info "DONE: secret key is available"
    else
        echo "ERROR: secret key is NOT available - PPAs cannot be signed"
        errors=$((errors + 1))
    fi

    # validity & expiration
    pub_line="$(gpg --with-colons --list-keys "${MF_DEBIAN_GPG_KEY}" | grep '^pub:')"
    validity="$(echo "${pub_line}" | cut -d: -f2)"
    key_length="$(echo "${pub_line}" | cut -d: -f3)"
    key_algo="$(echo "${pub_line}" | cut -d: -f4)"
    expires="$(echo "${pub_line}" | cut -d: -f7)"
    case "${validity}" in
        r) echo "ERROR: key is REVOKED"; errors=$((errors + 1)) ;;
        e) echo "ERROR: key is EXPIRED"; errors=$((errors + 1)) ;;
        *) info "DONE: key is neither revoked nor expired" ;;
    esac
    if [ -z "${expires}" ]; then
        info "DONE: key does not expire"
    else
        info "DONE: key expires on $(date -d "@${expires}" +%Y-%m-%d)"
    fi

    # algorithm (1 ~ RSA)
    if [ "${key_algo}" = "1" ] && [ "${key_length}" -lt 3072 ]; then
        warn "rsa${key_length} key is accepted by Debian apt only until ${MF_RSA2048_DEADLINE} - plan key rotation"
    fi

    # self-signature digests (SHA-1 bindings are rejected by trixie+ apt)
    export_keyring "${tmp_dir}/local.gpg"
    if [ "$(sha1_self_sigs "${tmp_dir}/local.gpg")" -gt 0 ]; then
        echo "ERROR: local key has SHA-1 self-signatures - Debian trixie+ apt rejects it (see README.md: refresh self-signatures)"
        errors=$((errors + 1))
    else
        info "DONE: local key self-signatures are not SHA-1"
    fi

    # key published on the web
    if curl -fsSL -o "${tmp_dir}/web.asc" "${MF_DEBIAN_GPG_KEY_URL}"; then
        web_fpr="$(gpg --with-colons --show-keys "${tmp_dir}/web.asc" 2>/dev/null | awk -F: '/^fpr:/ { print $10; exit }')"
        if [ "${web_fpr}" = "${MF_DEBIAN_GPG_KEY}" ]; then
            info "DONE: ${MF_DEBIAN_GPG_KEY_URL} fingerprint matches"
        else
            echo "ERROR: ${MF_DEBIAN_GPG_KEY_URL} fingerprint '${web_fpr}' does NOT match"
            errors=$((errors + 1))
        fi
        web_sha1="$(sha1_self_sigs "${tmp_dir}/web.asc")"
        if [ "${web_sha1}" -gt 0 ]; then
            warn "${MF_DEBIAN_GPG_KEY_URL} still has SHA-1 self-signatures - upload the re-exported public key (see README.md)"
        else
            info "DONE: ${MF_DEBIAN_GPG_KEY_URL} self-signatures are not SHA-1"
        fi
    else
        warn "unable to download ${MF_DEBIAN_GPG_KEY_URL}"
    fi

    # sign & verify using Debian trixie apt's verifier (sqv) and policy
    if command -v docker > /dev/null && [ "$(sha1_self_sigs "${tmp_dir}/local.gpg")" -eq 0 ]; then
        echo "MindForger PPA signing test" > "${tmp_dir}/test.txt"
        chmod 644 "${tmp_dir}/test.txt"
        gpg --yes --local-user "${MF_DEBIAN_GPG_KEY}" --detach-sign --output "${tmp_dir}/test.txt.sig" "${tmp_dir}/test.txt"
        chmod 644 "${tmp_dir}/test.txt.sig"
        if docker run --rm --network none -v "${tmp_dir}:/k:ro" debian:trixie \
            sqv --keyring /k/local.gpg /k/test.txt.sig /k/test.txt > /dev/null; then
            info "DONE: signature verified by Debian trixie sqv (apt policy)"
        else
            echo "ERROR: signature rejected by Debian trixie sqv (apt policy)"
            errors=$((errors + 1))
        fi
    else
        warn "skipping Debian trixie sqv signature test (docker n/a or SHA-1 self-signatures)"
    fi

    if [ "${errors}" -gt 0 ]; then
        die "signing key check failed w/ ${errors} error(s)"
    fi
    info "DONE: signing key check passed"
}

# ############################################################################
# # Main #
# ############################################################################

function usage {
    sed -n '/^# usage:/,/^$/p' "${BASH_SOURCE[0]}" | sed -e 's/^# \{0,1\}//'
    exit 1
}

COMMAND="${1:-}"
shift || true

case "${COMMAND}" in
    add)       cmd_add_or_update add "${@}" ;;
    update)    cmd_add_or_update update "${@}" ;;
    remove)    cmd_remove "${@}" ;;
    release)   cmd_release "${@}" ;;
    sync)      cmd_sync "${@}" ;;
    list)      cmd_list "${@}" ;;
    index)     cmd_index ;;
    verify)    cmd_verify "${@}" ;;
    key-check) cmd_key_check ;;
    *)         usage ;;
esac

# eof
