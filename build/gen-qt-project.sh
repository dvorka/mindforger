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

export H=`pwd`
export RMPREFIX="./"



cat ./gen-qt-project.prefix



echo -n "HEADERS +="
cd ../../lib/src && find . -name "*.h" | while read F
do
    # ./lib/src/mind/memory_dwell.h \
    echo "        ./lib/src/${F#$RMPREFIX} \\"
done
cd $H
cd ../../src && find . -name "*.h" | while read F
do
    # ./lib/src/mind/memory_dwell.cpp \
    echo "        ./src/${F#$RMPREFIX} \\"
done
echo
echo


cd $H
echo -n "SOURCES +="
cd ../../lib/src && find . -name "*.cpp" | while read F
do
    # ./lib/src/mind/memory_dwell.h \
    echo "        ./lib/src/${F#$RMPREFIX} \\"
done
cd $H
cd ../../src && find . -name "*.cpp" | while read F
do
    # ./lib/src/mind/memory_dwell.cpp \
    echo "        ./src/${F#$RMPREFIX} \\"
done
echo
echo



cd $H
cat ./gen-qt-project.suffix

# eof
