#!/bin/bash
#
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

if [ $# -eq 0 ]
then
    echo "ERROR: Docker image ID must be specified as the first script parameter!"
    docker images
    exit 1
fi

##############################
# SECURITY RISK
##############################
xhost +local:root

# run MindForger
docker run -it --env="DISPLAY" --env="QT_X11_NO_MITSHM=1" --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" ${1} mindforger
export MIND_FORGER_CONTAINER_ID=$(docker ps -l -q)

# start stopped MindForger container
echo "Start MindForger container using: $ docker start ${MIND_FORGER_CONTAINER_ID} "

# eof
