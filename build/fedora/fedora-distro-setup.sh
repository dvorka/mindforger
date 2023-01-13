#!/usr/bin/env bash
#
# MindForger knowledge management tool
#
# Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>
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

# Setup clean Fedora installation for MindForger .rpm release
#
# Hints:
# - change resolution using Displays (set maximum video memory @ VirtualBox)
# - scripts to download .deb / tarball using wget/http(s), not FTP
#

# system dependencies
sudo dnf install -y mc emacs git tig

# source code
mkdir -vp ~/p/mindforger/git
mkdir -vp ~/p/mindforger/alien
# cd git/mindforger && git clone https://github.com/dvorka/mindforger.git && git submodule init --with-update

# save .deb in alien/ directory
# .deb download (GitHub): Releases/
# .deb download (HTTPS): wget
# .deb download (WebFTP): webftp.hosting.tld
# .deb download (FTP): ftp user@host > ls cd put get

# run .deb to .rpm conversion script:
#   ~/p/mindforger/git/mindforger/build/fedora/fedora-rpm-from-deb.sh

# eof
