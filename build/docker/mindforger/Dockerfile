# MindForger thinking notebook
#
# Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>
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
# $ docker build -t mindforger:latest mindforger

FROM ubuntu:16.04

# IMPROVE: run MF under normal user

# APT prereqs
RUN apt-get update
RUN apt-get -y install software-properties-common

# add MindForger PPA to trusted repositories
RUN add-apt-repository -y ppa:ultradvorka/productivity

# update sources
RUN apt-get -y update

# install MindForger
RUN apt-get -y install mindforger

# run MindForger
CMD echo -e "\nRun MindForger:\n  $ xhost +local:root && docker run -it --env=\"DISPLAY\" --env=\"QT_X11_NO_MITSHM=1\" --volume=\"/tmp/.X11-unix:/tmp/.X11-unix:rw\" mindforger:latest mindforger"

# eof
