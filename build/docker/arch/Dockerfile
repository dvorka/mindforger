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

# verifies build/arch/PKGBUILD on Arch Linux in an isolated container - see
# build-pkg.sh for usage.
#
# stages:
# - base     ... Arch + base-devel + non-root builder (makepkg refuses root)
# - srcinfo  ... base + container-build.sh: regenerates .SRCINFO (seconds)
# - pkg      ... base + Qt WebKit chain built from AUR + container-build.sh:
#                builds, checks (namcap) and installs the MindForger package
#
# Qt WebKit and its Qt 5 dependencies are no longer in the official Arch
# repositories, so they are built from AUR - exactly like an Arch user has to.
# Each AUR package is built in its own layer, therefore the (long) chain is
# built only once and cached by Docker. container-build.sh is copied AFTER
# the chain so that editing it does not trigger the rebuild of the chain.

FROM archlinux:base-devel AS base

RUN pacman -Syu --noconfirm --needed git namcap sudo \
    && pacman -Scc --noconfirm \
    && useradd -m builder \
    && echo "builder ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/builder \
    && sed -i -E "s/^#?MAKEFLAGS=.*/MAKEFLAGS=\"-j$(nproc)\"/" /etc/makepkg.conf \
    && sed -i -E "s/^(OPTIONS=.*[( ])debug/\1!debug/" /etc/makepkg.conf

COPY aur-install.sh /usr/local/bin/aur-install.sh
RUN chmod +x /usr/local/bin/aur-install.sh

USER builder
WORKDIR /home/builder


FROM base AS srcinfo

USER root
COPY container-build.sh /usr/local/bin/container-build.sh
RUN chmod +x /usr/local/bin/container-build.sh
USER builder

ENTRYPOINT ["/usr/local/bin/container-build.sh"]


FROM base AS pkg

# Qt WebKit chain in dependency order (qt5-doc is a make dependency of qt5-webkit)
RUN aur-install.sh qt5-location
RUN aur-install.sh qt5-sensors
RUN aur-install.sh qt5-webchannel
RUN aur-install.sh qt5-doc
RUN aur-install.sh qt5-webkit

USER root
COPY container-build.sh /usr/local/bin/container-build.sh
RUN chmod +x /usr/local/bin/container-build.sh
USER builder

ENTRYPOINT ["/usr/local/bin/container-build.sh"]

# eof
