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

# builds a MindForger .deb for a chosen Debian release, in an isolated
# per-release container - see build-deb.sh for usage.
#
# - bullseye (11) and bookworm (12) build exactly like build/debian/debian/
#   already declares (Qt WebKit): nothing about that packaging is touched.
# - trixie (13) dropped libqt5webkit5-dev from the archive entirely, so its
#   build patches its OWN COPY of debian/control + debian/rules (done in
#   container-build.sh, never the checked-in files) to use Qt WebEngine
#   instead - see mindforger.pro's documented CONFIG+=mfwebengine option.

ARG DEBIAN_RELEASE=trixie
FROM debian:${DEBIAN_RELEASE}

ARG DEBIAN_RELEASE
ENV DEBIAN_FRONTEND=noninteractive
ENV MF_DEBIAN_RELEASE=${DEBIAN_RELEASE}

# bullseye (11) is past standard support: deb.debian.org's Release file for
# it has expired, and worse, its package pool has already partly moved off
# deb.debian.org (some .debs 404 despite a seemingly fine index). The
# debian:bullseye image ships commented-out snapshot.debian.org pins for
# exactly this situation - switch to those instead of deb.debian.org.
RUN if [ "${DEBIAN_RELEASE}" = "bullseye" ]; then \
        sed -i \
            -e 's|^# deb http://snapshot|deb http://snapshot|' \
            -e 's|^deb http://deb.debian.org|# deb http://deb.debian.org|' \
            /etc/apt/sources.list; \
    fi

RUN if [ "${DEBIAN_RELEASE}" = "trixie" ]; then MF_WEB_PKG=qtwebengine5-dev; else MF_WEB_PKG=libqt5webkit5-dev; fi \
    && apt-get -o Acquire::Check-Valid-Until=false update && apt-get install -y --no-install-recommends \
        build-essential \
        debhelper \
        devscripts \
        fakeroot \
        dpkg-dev \
        ccache \
        cmake \
        ca-certificates \
        zlib1g-dev \
        libcurl4-gnutls-dev \
        libhunspell-dev \
        qt5-qmake \
        qtbase5-dev \
        "${MF_WEB_PKG}" \
    && rm -rf /var/lib/apt/lists/*

COPY container-build.sh /usr/local/bin/container-build.sh
RUN chmod +x /usr/local/bin/container-build.sh

ENTRYPOINT ["/usr/local/bin/container-build.sh"]

# eof
