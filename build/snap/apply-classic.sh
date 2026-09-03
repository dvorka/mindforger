#!/bin/bash
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

# Transform a BUILD-COPY snapcraft.yaml from the strict (default, committed) manifest
# into the classic variant for the downloadable, sideloaded GitHub Release snap.
#
# This is the single authoritative place for the strict -> classic edits (DRY). It must
# only ever be pointed at a build copy, never the committed build/snap/snapcraft.yaml.
#
# Known risk (unverified here - no older-host test rig available): classic confinement
# runs the binary against the HOST's own ld.so/glibc, not core24's. If a user's host
# glibc is older than core24's build glibc, loading the bundled Qt5/WebKit .so files
# via LD_LIBRARY_PATH below can hit a glibc/ld.so ABI mismatch (symbol lookup errors
# such as "undefined symbol: __tunable_is_initialized, GLIBC_PRIVATE"). If that turns
# up in the wild, the fix is the same one MyTraL uses for its classic variant: a launcher
# that execs core24's own ld.so directly with --library-path pointing at core24's libs
# (see /home/dvorka/p/mytral/git/mytral/build/snap/mytral-wrapper for the pattern).
#
# Usage: apply-classic.sh <path-to-build-copy-snapcraft.yaml>

set -e

YAML="$1"
if [ -z "$YAML" ] || [ ! -f "$YAML" ]; then
    echo "ERROR: apply-classic.sh needs a path to a snapcraft.yaml build copy" >&2
    exit 1
fi

# 1. confinement: strict -> classic (sideloadable, full host access)
sed -i 's|^confinement: strict$|confinement: classic|' "$YAML"

# 2. remove the strict-only HOME redirect; classic keeps the real ~/.mindforger.md
#    and ~/mindforger-repository, matching the existing .deb/tarball behavior users
#    already expect - classic has no 'home' interface hidden-file restriction to work
#    around in the first place
sed -i '/# strict-only wiring/,/HOME: \${SNAP_USER_COMMON}/d' "$YAML"

# 3. classic does not get snapd's automatic per-snap library search path injection -
#    point the dynamic linker at the bundled Qt5/WebKit libs explicitly, otherwise the
#    app falls back to whatever (if anything) the host distro happens to have installed.
#    Hardcode the multiarch dir - ${SNAPCRAFT_ARCH_TRIPLET} is a snapcraft BUILD-time
#    variable, not one snapd exports at runtime (confirmed against a real Snap Store
#    install of the strict build, which hit this same mistake and shipped a broken,
#    empty-substituted path). Safe since this snap is amd64-only.
sed -i '/QT_QPA_PLATFORM_PLUGIN_PATH:/a\      LD_LIBRARY_PATH: ${SNAP}/usr/lib/x86_64-linux-gnu:${SNAP}/usr/lib/x86_64-linux-gnu/qt5/plugins/platforms' "$YAML"

# 4. remove the apps.mindforger plugs block (classic ignores interfaces); delete the
#    'plugs:' line and the contiguous 6-space-indented list items that follow it
awk '
    /^    plugs:[[:space:]]*$/ { in_plugs = 1; next }
    in_plugs && /^      - / { next }
    in_plugs { in_plugs = 0 }
    { print }
' "$YAML" > "$YAML.tmp" && mv -f "$YAML.tmp" "$YAML"

echo "Applied classic-confinement transform to: $YAML"
