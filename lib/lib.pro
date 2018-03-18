# mindforger-lib.pro     MindForger thinking notebook
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

TARGET = mindforger
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

QMAKE_CXX = ccache g++
QMAKE_CXXFLAGS += -std=c++0x -pedantic -g -pg

SOURCES += \
    ./src/repository_indexer.cpp \
    ./src/gear/datetime_utils.cpp \
    ./src/gear/file_utils.cpp \
    ./src/gear/string_utils.cpp \
    ./src/mind/ontology/ontology.cpp \
    ./src/model/attachment.cpp \
    ./src/model/note_type.cpp \
    ./src/model/note.cpp \
    ./src/model/outline_type.cpp \
    ./src/model/outline.cpp \
    ./src/model/stencil.cpp \
    ./src/model/tag.cpp \
    ./src/persistence/filesystem_persistence.cpp \
    ./src/representations/html/html_outline_representation.cpp \
    ./src/representations/markdown/markdown_ast_node.cpp \
    ./src/representations/markdown/markdown_lexem.cpp \
    ./src/representations/markdown/markdown_lexer_sections.cpp \
    ./src/representations/markdown/markdown_note_metadata.cpp \
    ./src/representations/markdown/markdown_outline_metadata.cpp \
    ./src/representations/markdown/markdown_outline_representation.cpp \
    ./src/representations/markdown/markdown_parser_sections.cpp \
    ./src/representations/markdown/markdown_section_metadata.cpp \
    ./src/representations/outline_representation.cpp \
    ./src/mind/galaxy.cpp \
    ./src/mind/memory_dwell.cpp \
    ./src/mind/memory.cpp \
    ./src/mind/mind.cpp \
    ./src/mind/planner.cpp \
    ./src/mind/working_memory.cpp \
    ./src/config/configuration.cpp \
    ./src/install/installer.cpp \
    ./src/config/repository.cpp \
    ./src/mind/ontology/thing_class_rel_triple.cpp \
    ./src/mind/aspect/time_scope_aspect.cpp \
    ./src/representations/markdown/markdown_configuration_representation.cpp \
    ./src/config/time_scope.cpp \
    ./src/model/link.cpp \
    ./src/config/palette.cpp \
    src/representations/markdown/markdown_document.cpp \
    src/representations/html/html_document.cpp

HEADERS += \
    ./src/debug.h \
    ./src/exceptions.h \
    ./src/repository_indexer.h \
    ./src/3rdparty/hoedown/autolink.h \
    ./src/3rdparty/hoedown/buffer.h \
    ./src/3rdparty/hoedown/document.h \
    ./src/3rdparty/hoedown/escape.h \
    ./src/3rdparty/hoedown/html.h \
    ./src/3rdparty/hoedown/stack.h \
    ./src/3rdparty/hoedown/version.h \
    ./src/gear/datetime_utils.h \
    ./src/gear/file_utils.h \
    ./src/gear/hash_map.h \
    ./src/gear/lang_utils.h \
    ./src/gear/string_utils.h \
    ./src/mind/ontology/ontology_vocabulary.h \
    ./src/mind/ontology/ontology.h \
    ./src/model/attachment.h \
    ./src/model/note_type.h \
    ./src/model/note.h \
    ./src/model/outline_type.h \
    ./src/model/outline.h \
    ./src/model/resource_types.h \
    ./src/model/stencil.h \
    ./src/model/tag.h \
    ./src/persistence/filesystem_persistence.h \
    ./src/persistence/persistence.h \
    ./src/representations/html/html_outline_representation.h \
    ./src/representations/markdown/markdown_ast_node.h \
    ./src/representations/markdown/markdown_lexem.h \
    ./src/representations/markdown/markdown_lexer_sections.h \
    ./src/representations/markdown/markdown_note_metadata.h \
    ./src/representations/markdown/markdown_outline_metadata.h \
    ./src/representations/markdown/markdown_outline_representation.h \
    ./src/representations/markdown/markdown_parser_sections.h \
    ./src/representations/markdown/markdown_section_metadata.h \
    ./src/representations/markdown/markdown.h \
    ./src/representations/outline_representation.h \
    ./src/mind/galaxy.h \
    ./src/mind/memory_dwell.h \
    ./src/mind/memory.h \
    ./src/mind/mind.h \
    ./src/mind/planner.h \
    ./src/mind/working_memory.h \
    ./src/mind/mind_listener.h \
    ./src/config/color.h \
    ./src/config/configuration.h \
    ./src/install/installer.h \
    ./src/config/repository.h \
    ./src/mind/ontology/thing_class_rel_triple.h \
    ./src/mind/ontology/taxonomy.h \
    ./src/mind/aspect/aspect.h \
    ./src/mind/aspect/time_scope_aspect.h \
    ./src/representations/markdown/markdown_configuration_representation.h \
    ./src/config/time_scope.h \
    ./src/model/link.h \
    ./src/config/palette.h \
    src/representations/markdown/markdown_document.h \
    src/representations/html/html_document.h
