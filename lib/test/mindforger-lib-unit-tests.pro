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

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXX = ccache g++
QMAKE_CXXFLAGS += -std=c++0x -pedantic -g -pg

SOURCES += \
    ../../lib/src/repository_indexer.cpp \
    ../../lib/src/gear/datetime_utils.cpp \
    ../../lib/src/gear/file_utils.cpp \
    ../../lib/src/gear/string_utils.cpp \
    ../../lib/src/mind/ontology/ontology.cpp \
    ../../lib/src/model/attachment.cpp \
    ../../lib/src/model/note_type.cpp \
    ../../lib/src/model/note.cpp \
    ../../lib/src/model/outline_type.cpp \
    ../../lib/src/model/outline.cpp \
    ../../lib/src/model/stencil.cpp \
    ../../lib/src/model/tag.cpp \
    ../../lib/src/persistence/filesystem_persistence.cpp \
    ../../lib/src/representations/html/html_outline_representation.cpp \
    ../../lib/src/representations/markdown/markdown_ast_node.cpp \
    ../../lib/src/representations/markdown/markdown_lexem.cpp \
    ../../lib/src/representations/markdown/markdown_lexer_sections.cpp \
    ../../lib/src/representations/markdown/markdown_note_metadata.cpp \
    ../../lib/src/representations/markdown/markdown_outline_metadata.cpp \
    ../../lib/src/representations/markdown/markdown_outline_representation.cpp \
    ../../lib/src/representations/markdown/markdown_parser_sections.cpp \
    ../../lib/src/representations/markdown/markdown_section_metadata.cpp \
    ../../lib/src/representations/outline_representation.cpp \
    ../../lib/test/src/gear/datetime_test.cpp \
    ../../lib/test/src/gear/string_utils_test.cpp \
    ../../lib/test/src/indexer/repository_indexer_test.cpp \
    ../../lib/test/src/markdown/markdown_test.cpp \
    ../../lib/test/src/mind/fts_test.cpp \
    ../../lib/test/src/mind/memory_test.cpp \
    ../../lib/test/src/mind/mind_test.cpp \
    ../../lib/test/src/mind/note_test.cpp \
    ../../lib/test/src/mindforger_lib_unit_tests.cpp \
    ../../lib/src/mind/galaxy.cpp \
    ../../lib/src/mind/memory_dwell.cpp \
    ../../lib/src/mind/memory.cpp \
    ../../lib/src/mind/mind.cpp \
    ../../lib/src/mind/planner.cpp \
    ../../lib/src/mind/working_memory.cpp \
    ../../lib/src/config/configuration.cpp \
    ../../lib/src/representations/markdown/markdown.cpp \
    ../../lib/src/install/installer.cpp \
    ../../lib/test/src/mind/outline_test.cpp \
    ../../lib/test/src/test_gear.cpp \
    ../../lib/src/config/repository.cpp \
    ../../lib/test/src/config/configuration_test.cpp \
    ../../lib/src/mind/ontology/thing_class_rel_triple.cpp

HEADERS += \
    ../../lib/src/debug.h \
    ../../lib/src/exceptions.h \
    ../../lib/src/repository_indexer.h \
    ../../lib/src/3rdparty/hoedown/autolink.h \
    ../../lib/src/3rdparty/hoedown/buffer.h \
    ../../lib/src/3rdparty/hoedown/document.h \
    ../../lib/src/3rdparty/hoedown/escape.h \
    ../../lib/src/3rdparty/hoedown/html.h \
    ../../lib/src/3rdparty/hoedown/stack.h \
    ../../lib/src/3rdparty/hoedown/version.h \
    ../../lib/src/gear/datetime_utils.h \
    ../../lib/src/gear/file_utils.h \
    ../../lib/src/gear/hash_map.h \
    ../../lib/src/gear/lang_utils.h \
    ../../lib/src/gear/string_utils.h \
    ../../lib/src/mind/ontology/ontology_vocabulary.h \
    ../../lib/src/mind/ontology/ontology.h \
    ../../lib/src/model/attachment.h \
    ../../lib/src/model/note_type.h \
    ../../lib/src/model/note.h \
    ../../lib/src/model/outline_type.h \
    ../../lib/src/model/outline.h \
    ../../lib/src/model/resource_types.h \
    ../../lib/src/model/stencil.h \
    ../../lib/src/model/tag.h \
    ../../lib/src/persistence/filesystem_persistence.h \
    ../../lib/src/persistence/persistence.h \
    ../../lib/src/representations/html/html_outline_representation.h \
    ../../lib/src/representations/markdown/markdown_ast_node.h \
    ../../lib/src/representations/markdown/markdown_lexem.h \
    ../../lib/src/representations/markdown/markdown_lexer_sections.h \
    ../../lib/src/representations/markdown/markdown_note_metadata.h \
    ../../lib/src/representations/markdown/markdown_outline_metadata.h \
    ../../lib/src/representations/markdown/markdown_outline_representation.h \
    ../../lib/src/representations/markdown/markdown_parser_sections.h \
    ../../lib/src/representations/markdown/markdown_section_metadata.h \
    ../../lib/src/representations/markdown/markdown.h \
    ../../lib/src/representations/outline_representation.h \
    ../../lib/src/mind/galaxy.h \
    ../../lib/src/mind/memory_dwell.h \
    ../../lib/src/mind/memory.h \
    ../../lib/src/mind/mind.h \
    ../../lib/src/mind/planner.h \
    ../../lib/src/mind/working_memory.h \
    ../../lib/src/mind/mind_listener.h \
    ../../lib/src/config/color.h \
    ../../lib/src/config/configuration.h \
    ../../lib/src/install/installer.h \
    ../../lib/src/config/repository.h \
    ../../lib/test/src/test_gear.h \
    ../../lib/src/mind/ontology/thing_class_rel_triple.h \
    ../../lib/src/mind/ontology/taxonomy.h

# GTest unit test framework library dependencies
unix|win32: LIBS += -lgtest
unix|win32: LIBS += -lpthread
