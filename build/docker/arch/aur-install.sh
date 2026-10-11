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

# builds and installs ONE package from AUR - the manual equivalent of an AUR
# helper like yay or paru. runs INSIDE the container as the non-root builder
# (see Dockerfile). dependencies which are in the official repositories are
# installed by makepkg -s, AUR dependencies must be installed BEFORE.
#
# usage:
#   aur-install.sh <aur-package-name>

set -euo pipefail

if [ "$#" -ne 1 ]; then
    echo "ERROR: usage: aur-install.sh <aur-package-name>"
    exit 1
fi

AUR_PKG="$1"
AUR_WORK="${HOME}/aur/${AUR_PKG}"

echo "# AUR: building ${AUR_PKG} ##################################"
START=$(date +%s)

rm -rf "${AUR_WORK}"
git clone --depth 1 "https://aur.archlinux.org/${AUR_PKG}.git" "${AUR_WORK}"
cd "${AUR_WORK}"
makepkg --syncdeps --install --noconfirm --needed

# drop sources and build tree to keep the Docker layer small
cd "${HOME}"
rm -rf "${AUR_WORK}"
sudo pacman -Scc --noconfirm

echo "DONE: AUR ${AUR_PKG} built and installed in $(( ($(date +%s) - START) / 60 )) min"

# eof
