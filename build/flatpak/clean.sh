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

# Removes every Flatpak build relic - the flatpak-builder work directories, the local
# OSTree repository and the exported bundle - so that a stale one is never picked up
# by mistake.

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

echo "Cleaning Flatpak build artifacts..."
rm -rf "${SCRIPT_DIR}/build-dir"
rm -rf "${SCRIPT_DIR}/repo"
rm -rf "${SCRIPT_DIR}/.flatpak-builder"
rm -rf "${PROJECT_ROOT}/distro/flatpak"
echo "DONE"

# eof
