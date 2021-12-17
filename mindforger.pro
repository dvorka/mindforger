# mindforger.pro     Qt project file for MindForger
#
# Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>
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

#########################################################################
#
# QMake build options:
#   qmake CONFIG+=mfwebengine       ... build project w/ Qt WebEngine instead of Qt WebKit
#   qmake CONFIG+=mfner             ... build project w/ NER and link dlib/MITIE
#   qmake CONFIG+=mfnomd2html       ... build project w/o Markdown 2 HTML library (cmark-gfm)
#   qmake CONFIG+=mfnocxx           ... do NOT define CXX i.e. g++
#   qmake CONFIG+=mfnoccache        ... do NOT use ccache to build the project
#   qmake CONFIG+=mfdebug           ... show debug messages + include WIP code
#   qmake CONFIG+=mfunits           ... options to run unit tests
#
#########################################################################

TEMPLATE = subdirs

SUBDIRS = lib app

# build dependencies
app.depends = lib

# ########################################
# Linux installation: make install
# ########################################
# See http://doc.qt.io/qt-5/qmake-advanced-usage.html

#IMPORTANT: binfile MUST be specified in app/app.pro (project next to/that builds binary)

docfiles.files += doc/*
docfiles.path = $$PREFIX/share/doc/mindforger/
INSTALLS += docfiles

manfiles.files += man/*
manfiles.path = $$PREFIX/share/man/man1/
INSTALLS += manfiles

iconfiles.files += app/resources/icons/*
iconfiles.path = $$PREFIX/share/icons/mindforger/
INSTALLS += iconfiles

# experiment w/ file
shortcutfiles.files += app/resources/gnome-shell/mindforger.desktop
shortcutfiles.path = $$PREFIX/share/applications/
INSTALLS += shortcutfiles

# eof
