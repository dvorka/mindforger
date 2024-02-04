# mindforger-lib-unit-tests.pro     Qt project file for MindForger
#
# Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

TEMPLATE = subdirs

win32|macx {
    # Qt Network as CURL replacement on Win - add Qt to libmindforger
    CONFIG += qt
    QT += network
} else {
    CONFIG -= qt
}

message("= MindForger library test QMake configuration ==========================")
message("Qt version: $$QT_VERSION")

SUBDIRS = lib src

# where to find the sub projects - give the folders
lib.subdir  = ../../lib
src.subdir  = ./src

# build dependencies
src.depends = lib

# ########################################
# Diagnostics
# ########################################

message(DEFINES of app.pro build: $$DEFINES)
message(QMAKE_EXTRA_TARGETS of app.pro build: $$QMAKE_EXTRA_TARGETS)
message(QT of app.pro build: $$QT)
message(PATH is: $$(PATH))

# eof
