# MindForger thinking notebook
#
# Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>
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

# Use this Dockerfile to create a Docker container using:
#   $ docker build -t mindforger:latest mindforger
#
# Run MindForger image:
#   $ xhost +local:root && docker run -it --env="DISPLAY" --env="QT_X11_NO_MITSHM=1" --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" mindforger:latest mindforger

FROM ubuntu:16.04

MAINTAINER "Martin Dvorak" <martin.dvorak@mindforger.com>

# update APT, add MindForger PPA and install MindForger
RUN apt-get update && \
    apt-get install -y locales && \
    locale-gen en_US en_US.UTF-8 && \
    apt-get -y install software-properties-common && \
    add-apt-repository -y ppa:ultradvorka/productivity && \
    apt-get -y update && \
    apt-get -y install mindforger

# ensure locale despite locale-gen
ENV LANG=en_US.UTF-8 LANGUAGE=en_US

RUN useradd -ms /bin/bash mindforger-user
USER mindforger-user
WORKDIR /home/mindforger-user

# eof
