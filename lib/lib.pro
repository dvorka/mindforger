# mindforger-lib.pro     MindForger thinking notebook
#
# Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>
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

# Dependencies:
# - INCLUDEPATH is used during compilation to find included header files.
# - DEPENDPATH is used to resolve dependencies between header and source files,
#   e.g. which source files need to be recompiled when certain header file changes.

mfnomd2html {
  DEFINES += MF_NO_MD_2_HTML
} else {
  DEFINES += MF_MD_2_HTML_CMARK CMARK_GFM_STATIC_DEFINE CMARK_GFM_EXTENSIONS_STATIC_DEFINE
  INCLUDEPATH += $$PWD/../deps/cmark-gfm/src
  INCLUDEPATH += $$PWD/../deps/cmark-gfm/extensions
  INCLUDEPATH += $$PWD/../deps/cmark-gfm/build/src
  INCLUDEPATH += $$PWD/../deps/cmark-gfm/build/extensions
}

# Zlib on Windows
win32 {
 INCLUDEPATH += $$PWD/../deps/zlib-win/include
 DEPENDPATH += $$PWD/../deps/zlib-win/include
}

# DEPRECATED: Mitie (NER library)
mfner {
    DEFINES += MF_NER
    INCLUDEPATH += $$PWD/../deps/mitie/mitielib/include
    DEPENDPATH += $$PWD/../deps/mitie/mitielib/include
}

# debug
mfdebug|mfunits {
  DEFINES += DO_MF_DEBUG
}

# compiler options (qmake CONFIG+=mfnoccache ...)
win32{
    QMAKE_CXXFLAGS += /MP
} else {
    # linux and macos
    mfnoccache {
      QMAKE_CXX = g++
    } else:!mfnocxx {
      QMAKE_CXX = ccache g++
    }

    mfdebug|mfunits {
      # debug info
      QMAKE_CXXFLAGS += -g
    }

    # -O3 ... can be specified by this variable (overrides -O1 -O2)
    QMAKE_CXXFLAGS += -pedantic -std=c++11
}

SOURCES += \
    ./src/repository_indexer.cpp \
    ./src/gear/datetime_utils.cpp \
    ./src/gear/file_utils.cpp \
    ./src/gear/string_utils.cpp \
    ./src/mind/ontology/ontology.cpp \
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
    src/config/repository_configuration.cpp \
    src/gear/async_utils.cpp \
    src/gear/math_utils.cpp \
    src/mind/ai/llm/mock_wingman.cpp \
    src/mind/ai/llm/wingman.cpp \
    src/mind/dikw/dikw_pyramid.cpp \
    src/mind/dikw/filesystem_information.cpp \
    src/mind/dikw/information.cpp \
    src/model/eisenhower_matrix.cpp \
    src/model/kanban.cpp \
    src/model/organizer.cpp \
    src/persistence/configuration_persistence.cpp \
    src/persistence/persistence.cpp \
    src/representations/markdown/markdown_document.cpp \
    src/representations/html/html_document.cpp \
    src/mind/ai/ai.cpp \
    src/mind/ai/nlp/markdown_tokenizer.cpp \
    src/mind/ai/nlp/bag_of_words.cpp \
    src/mind/ai/aa_model.cpp \
    src/mind/ai/nlp/lexicon.cpp \
    src/mind/ai/nlp/note_char_provider.cpp \
    src/mind/ai/nlp/outline_char_provider.cpp \
    src/mind/ai/nlp/string_char_provider.cpp \
    src/mind/ai/nn/genann.c \
    src/mind/ai/nlp/word_frequency_list.cpp \
    src/gear/trie.cpp \
    src/mind/ai/nlp/stemmer/stemmer.cpp \
    src/mind/ai/ai_aa_bow.cpp \
    src/mind/ai/ai_aa_weighted_fts.cpp \
    src/mind/ai/aa_notes_feature.cpp \
    src/mind/ai/nlp/common_words_blacklist.cpp \
    src/mind/aspect/tag_scope_aspect.cpp \
    src/mind/aspect/mind_scope_aspect.cpp \
    src/mind/knowledge_graph.cpp \
    src/representations/markdown/markdown_document_representation.cpp \
    src/representations/markdown/markdown_repository_configuration_representation.cpp \
    src/representations/twiki/twiki_outline_representation.cpp \
    src/mind/associated_notes.cpp \
    src/mind/ai/autolinking_preprocessor.cpp \
    src/representations/csv/csv_outline_representation.cpp \
    src/mind/ai/autolinking/naive_autolinking_preprocessor.cpp \
    src/representations/markdown/cmark_gfm_markdown_transcoder.cpp \
    src/mind/ai/autolinking/autolinking_mind.cpp \
    src/mind/limbo.cpp \
    src/representations/unicode.cpp

!mfnomd2html {
    SOURCES += \
    src/mind/ai/autolinking/cmark_aho_corasick_block_autolinking_preprocessor.cpp
}

# DEPRECATED: Mitie (NER library)
mfner {
    SOURCES += \
    src/mind/ai/nlp/named_entity_recognition.cpp \
    src/mind/ai/nlp/ner_named_entity.cpp
}

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
    ./src/mind/working_memory.h \
    ./src/mind/mind_listener.h \
    ./src/config/color.h \
    ./src/config/configuration.h \
    ./src/install/installer.h \
    ./src/config/repository.h \
    ./src/definitions.h \
    ./src/mind/ontology/thing_class_rel_triple.h \
    ./src/mind/ontology/taxonomy.h \
    ./src/mind/aspect/aspect.h \
    ./src/mind/aspect/time_scope_aspect.h \
    ./src/representations/markdown/markdown_configuration_representation.h \
    ./src/config/time_scope.h \
    ./src/model/link.h \
    ./src/config/palette.h \
    ./src/config/repository_configuration.h \
    ./src/gear/async_utils.h \
    ./src/gear/math_utils.h \
    ./src/mind/dikw/dikw_pyramid.h \
    ./src/mind/dikw/filesystem_information.h \
    src/mind/ai/llm/mock_wingman.h \
    src/mind/ai/llm/wingman.h \
    src/mind/dikw/information.h \
    src/model/eisenhower_matrix.h \
    src/model/kanban.h \
    src/model/organizer.h \
    src/persistence/configuration_persistence.h \
    src/representations/markdown/markdown_document.h \
    src/representations/html/html_document.h \
    src/representations/markdown/markdown_document_representation.h \
    src/representations/markdown/markdown_repository_configuration_representation.h \
    src/representations/unicode.h \
    src/version.h \
    src/mind/ai/ai.h \
    src/mind/ai/nlp/markdown_tokenizer.h \
    src/mind/ai/nlp/bag_of_words.h \
    src/mind/ai/nlp/lexicon.h \
    src/mind/ai/nlp/note_char_provider.h \
    src/mind/ai/nlp/outline_char_provider.h \
    src/mind/ai/nlp/string_char_provider.h \
    src/mind/ai/nn/genann.h \
    src/mind/ai/nlp/word_frequency_list.h \
    src/gear/trie.h \
    src/mind/ai/nlp/char_provider.h \
    src/mind/ai/nlp/stemmer/stemmer.h \
    src/mind/ai/nlp/stemmer/stemming/danish_stem.h \
    src/mind/ai/nlp/stemmer/stemming/dutch_stem.h \
    src/mind/ai/nlp/stemmer/stemming/english_stem.h \
    src/mind/ai/nlp/stemmer/stemming/finnish_stem.h \
    src/mind/ai/nlp/stemmer/stemming/french_stem.h \
    src/mind/ai/nlp/stemmer/stemming/german_stem.h \
    src/mind/ai/nlp/stemmer/stemming/italian_stem.h \
    src/mind/ai/nlp/stemmer/stemming/norwegian_stem.h \
    src/mind/ai/nlp/stemmer/stemming/portuguese_stem.h \
    src/mind/ai/nlp/stemmer/stemming/russian_stem.h \
    src/mind/ai/nlp/stemmer/stemming/spanish_stem.h \
    src/mind/ai/nlp/stemmer/stemming/stemming.h \
    src/mind/ai/nlp/stemmer/stemming/swedish_stem.h \
    src/mind/ai/nlp/stemmer/indexing/common_lang_constants.h \
    src/mind/ai/nlp/stemmer/indexing/string_util.h \
    src/mind/ai/nlp/stemmer/utilities/debug_logic.h \
    src/mind/ai/nlp/stemmer/utilities/safe_math.h \
    src/mind/ai/nlp/stemmer/utilities/utilities.h \
    src/mind/ai/ai_aa_bow.h \
    src/mind/ai/ai_aa_weighted_fts.h \
    src/mind/ai/aa_model.h \
    src/mind/ai/aa_notes_feature.h \
    src/mind/ai/ai_aa.h \
    src/mind/ai/nlp/common_words_blacklist.h \
    src/mind/aspect/tag_scope_aspect.h \
    src/mind/aspect/mind_scope_aspect.h \
    src/compilation.h \
    src/mind/knowledge_graph.h \
    src/representations/twiki/twiki_outline_representation.h \
    src/mind/associated_notes.h \
    src/mind/ai/autolinking_preprocessor.h \
    src/representations/representation_interceptor.h \
    src/representations/csv/csv_outline_representation.h \
    src/mind/ai/autolinking/naive_autolinking_preprocessor.h \
    src/representations/markdown/markdown_transcoder.h \
    src/representations/representation_type.h \
    src/definitions.h \
    src/representations/markdown/cmark_gfm_markdown_transcoder.h \
    src/mind/ai/autolinking/autolinking_mind.h \
    src/mind/limbo.h

!mfnomd2html {
    SOURCES += \
    src/mind/ai/autolinking/cmark_aho_corasick_block_autolinking_preprocessor.h
}

# DEPRECATED: Mitie (NER library)
mfner {
    HEADERS += \
    src/mind/ai/nlp/named_entity_recognition.h \
    src/mind/ai/nlp/ner_named_entity.h
}

win32 {
    HEADERS += \
    ../deps/dirent/dirent.h \
    ../deps/strptime/strptime.h

    SOURCES += \
    ../deps/strptime/strptime.c
}
