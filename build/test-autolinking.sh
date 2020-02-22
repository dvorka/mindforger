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

# This script prepares autolinking data by creating cloning production
# MF repository to /tmp, then it runs lib unit test script which
# compiles/valgrinds/... monster autolinking unit test (autolinks
# each and every production repo section and tracks memory).

export SCRIPT_DIR=`pwd`
export MONSTER_REPO_PATH="/tmp/mindforger-unit-test/monster-repository"

if [[ -d ${MONSTER_REPO_PATH} ]]
then
    echo "Monster directory exists: ${MONSTER_REPO_PATH}"
    #rm -rvf ${MONSTER_REPO_PATH}
else
    mkdir -p ${MONSTER_REPO_PATH}
    cp -rvf ~/mf/* ${MONSTER_REPO_PATH}
    rm -rvf ${MONSTER_REPO_PATH}/.git ${MONSTER_REPO_PATH}/import ${MONSTER_REPO_PATH}/limbo
fi

# Run monster unit tests
# ./test-lib-units.sh TEST_NAME

# eof
