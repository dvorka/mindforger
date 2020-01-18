#!/bin/bash
#
# MindForger thinking notebook
#
# Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>
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

export MINDFORGER_IMAGE_ID="mindforger:latest"

if [ $# -eq 1 ]
then
    MINDFORGER_IMAGE_ID=${1}
fi

##############################
# SECURITY RISK
##############################
xhost +local:root

# run MindForger
docker run -it --env="DISPLAY" --env="QT_X11_NO_MITSHM=1" --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" ${MINDFORGER_IMAGE_ID}  mindforger

# remember container ID
docker ps -l -q > ~/.mindforger.docker

# start stopped MindForger container
echo "Start MindForger container using:"
echo '  $ docker start $(cat ~/.mindforger.docker)'

# eof
