# mindforger-lib-unit-tests.pro     MindForger thinking notebook
#
# Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>
#
# This program is free software ; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation ; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY ; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

TARGET = mindforger-lib-unit-tests
TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

# dependencies
INCLUDEPATH += $$PWD/../../../deps/discount
DEPENDPATH += $$PWD/../../../deps/discount
INCLUDEPATH += $$PWD/../../../lib/src
DEPENDPATH += $$PWD/../../../lib/src

# -L where to look for library, -l link the library
LIBS += -L$$OUT_PWD/../../../lib -lmindforger
LIBS += -L$$OUT_PWD/../../../deps/discount -ldiscount

# GTest unit test framework library dependencies
LIBS += -lgtest
LIBS += -lpthread

# compiler options
QMAKE_CXX = ccache g++
QMAKE_CXXFLAGS += -std=c++0x -pedantic -g -pg

SOURCES += \
    ./gear/datetime_test.cpp \
    ./gear/string_utils_test.cpp \
    ./indexer/repository_indexer_test.cpp \
    ./markdown/markdown_test.cpp \
    ./mind/fts_test.cpp \
    ./mind/memory_test.cpp \
    ./mind/mind_test.cpp \
    ./mind/note_test.cpp \
    ./mindforger_lib_unit_tests.cpp \
    ./mind/outline_test.cpp \
    ./test_gear.cpp \
    ./config/configuration_test.cpp \
    ../benchmark/markdown_benchmark.cpp \
    ../benchmark/html_benchmark.cpp \
    html/html_test.cpp

HEADERS += \
    ./test_gear.h

# eof
