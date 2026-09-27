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

# Fedora toolbox used by build/fedora/fedora-copr.sh: builds MindForger SRPM,
# rebuilds it to .rpm for a chosen Fedora release (the local equivalent of
# what COPR does in mock) and runs copr-cli.
#
# - build dependencies are NOT listed here - container-build.sh installs them
#   by `dnf builddep` from the spec (BuildRequires is the only list)
# - redhat-rpm-config + qt5-srpm-macros provide %{?dist}, Fedora compiler
#   flags and %{qt5_qtwebengine_arches} which are needed already by
#   `rpmbuild -bs` (COPR mock buildroot has them by default)

ARG FEDORA_RELEASE=44
FROM fedora:${FEDORA_RELEASE}

RUN dnf install -y \
        rpm-build \
        redhat-rpm-config \
        qt5-srpm-macros \
        dnf-plugins-core \
        copr-cli \
    && dnf clean all

COPY container-build.sh /usr/local/bin/container-build.sh
RUN chmod +x /usr/local/bin/container-build.sh

ENTRYPOINT ["/usr/local/bin/container-build.sh"]

# eof
