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

# Set window size for creation of screenshots for web mf.com: 1360x768 (then use Screenshot app)

echo "IMPORTANT: before running this script make sure window is NOT maximized or half sized - use rectangle button that allows window resizing (using right-down corner)"

MF_VERSION="1.30.0"

wmctrl -r "MindForger - Thinking Notebook - ${MF_VERSION} - /home/dvorka/mf" -e 0,0,0,1360,768
#wmctrl -r "MindForger - Thinking Notebook - ${MF_VERSION} - /home/dvorka/tmp/mf-copy" -e 0,0,0,1360,768
#wmctrl -r "MindForger - Thinking Notebook - ${MF_VERSION} - /home/dvorka/mf" -e 0,0,0,1360,768
#wmctrl -r "MindForger - Thinking Notebook - ${MF_VERSION} - /home/dvorka/demo" -e 0,0,0,1360,768
#wmctrl -r "MindForger - Thinking Notebook - ${MF_VERSION} - /home/dvorka/p/mindforger/git/mindforger-repository" -e 0,0,0,1360,768

# eof
