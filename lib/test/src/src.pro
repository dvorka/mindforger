# mindforger-lib-unit-tests.pro     MindForger thinking notebook
#
# Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>
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

DEFINES = DO_MF_DEBUG

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD/../../../lib/src
DEPENDPATH += $$PWD/../../../lib/src

# -L where to look for library, -l link the library
win32 {
    CONFIG(release, debug|release): LIBS += -L$$PWD/../../release -lmindforger
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../debug -lmindforger
} else {
    LIBS += -L$$OUT_PWD/../../../lib -lmindforger
}

#discount if mfmd2htmldiscount and not windows otherwise cmark
!mfnomd2html {
  win32 {
    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/../../../deps/cmark-gfm/build/src/Release -lcmark-gfm_static
        LIBS += -L$$PWD/../../../deps/cmark-gfm/build/extensions/Release -lcmark-gfm-extensions_static
    } else:CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../../deps/cmark-gfm/build/src/Debug -lcmark-gfm_static
        LIBS += -L$$PWD/../../../deps/cmark-gfm/build/extensions/Debug -lcmark-gfm-extensions_static
    }
  } else {
    mfmd2htmldiscount {
      # MF must link against ldiscount.a (built in ../deps/discount) - NOT lmarkdown
      LIBS += -L$$OUT_PWD/../../../deps/discount -ldiscount
    } else {
      #cmark
      LIBS += -L$$PWD/../../../deps/cmark-gfm/build/extensions -lcmark-gfm-extensions
      LIBS += -L$$PWD/../../../deps/cmark-gfm/build/src -lcmark-gfm
    }
  }
}


# zlib
win32 {
    INCLUDEPATH += $$PWD/../../../deps/zlib-win/include
    DEPENDPATH += $$PWD/../../../deps/zlib-win/include

    CONFIG(release, debug|release): LIBS += -L$$PWD/../../../deps/zlib-win/lib/ -lzlibwapi
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../deps/zlib-win/lib/ -lzlibwapi
} else {
    LIBS += -lz
}

#
win32 {
    LIBS += -lRpcrt4 -lOle32 -lShell32
}

# GTest unit test framework library dependencies
win32 {
    INCLUDEPATH += "c:/Program Files/googletest-distribution/include"
    CONFIG(release, debug|release): LIBS += -L"c:/Program Files/googletest-distribution/lib" -lgtest
    else:CONFIG(debug, debug|release): LIBS += -L"c:/Program Files/googletest-distribution/lib" -lgtestd
} else {
    LIBS += -lgtest
    LIBS += -lpthread
}

# compiler options
win32{
    QMAKE_CXXFLAGS += /MP
} else {
    # linux and macos
    mfnoccache {
      QMAKE_CXX = g++
    } else:!mfnocxx {
      QMAKE_CXX = ccache g++
    }
    QMAKE_CXXFLAGS += -pedantic -std=c++11
}

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
    ./html/html_test.cpp \
    ./ai/nlp_test.cpp \
    ../benchmark/trie_benchmark.cpp \
    ../benchmark/ai_benchmark.cpp \
    gear/file_utils_test.cpp \
    gear/trie_test.cpp \
    ai/autolinking_test.cpp

HEADERS += \
    ./test_gear.h

# eof
