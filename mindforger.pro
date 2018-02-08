# mindforger.pro     Qt project file for MindForger
#
# Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>
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

TARGET = mindforger

TEMPLATE = app

QT += widgets
QT += webkit
QT += webkitwidgets

HEADERS += ./lib/src/mind/memory_dwell.h \
    lib/src/mind/planner.h \
    lib/src/mind/ontology/thing.h \
    lib/src/mind/ontology/ontology.h \
    lib/src/mind/working_memory.h \
    lib/src/mind/mind.h \
    lib/src/mind/memory.h \
    lib/src/model/outline_type.h \
    lib/src/model/attachment.h \
    lib/src/model/outline.h \
    lib/src/model/note.h \
    lib/src/model/note_type.h \
    lib/src/model/tag.h \
    lib/src/repository_indexer.h \
    lib/src/gear/string_utils.h \
    lib/src/gear/datetime_utils.h \
    lib/src/gear/file_utils.h \
    lib/src/representations/markdown/markdown_section_metadata.h \
    lib/src/representations/markdown/markdown_lexer_sections.h \
    lib/src/representations/markdown/markdown_lexem.h \
    lib/src/representations/markdown/markdown_outline_metadata.h \
    lib/src/representations/markdown/markdown_ast_node.h \
    lib/src/representations/markdown/markdown_outline_representation.h \
    lib/src/representations/markdown/markdown.h \
    lib/src/representations/markdown/markdown_parser_sections.h \
    lib/src/representations/markdown/markdown_note_metadata.h \
    lib/src/representations/outline_representation.h \
    lib/src/representations/html/html_outline_representation.h \
    src/qt/3rdparty/elasticnodes/edge.h \
    src/qt/3rdparty/elasticnodes/graphwidget.h \
    src/qt/3rdparty/elasticnodes/node.h \
    src/qt/main_window_view.h \
    src/qt/outlines_table_view.h \
    src/qt/outlines_table_presenter.h \
    src/qt/outline_tree_view.h \
    src/qt/outline_tree_presenter.h \
    src/qt/status_bar_view.h \
    src/qt/status_bar_presenter.h \
    src/qt/main_window_presenter.h \
    src/qt/main_menu_view.h \
    src/qt/main_menu_presenter.h \
    src/qt/cli_n_breadcrumbs_presenter.h \
    src/qt/cli_n_breadcrumbs_view.h \
    src/qt/orloj_view.h \
    src/qt/orloj_presenter.h \
    src/qt/notes_table_view.h \
    src/qt/notes_table_presenter.h \
    src/qt/notes_table_model.h \
    src/qt/outline_tree_model.h \
    src/qt/outlines_table_model.h \
    src/qt/model_meta_definitions.h \
    src/qt/note_view_model.h \
    src/qt/note_view_presenter.h \
    src/qt/note_view.h \
    src/qt/note_edit_model.h \
    src/qt/note_edit_presenter.h \
    src/qt/look_n_feel.h \
    src/qt/html_delegate.h \
    lib/src/gear/lang_utils.h \
    src/qt/note_edit_highlight.h \
    lib/src/persistence/filesystem_persistence.h \
    lib/src/persistence/persistence.h \
    lib/src/model/stencil.h \
    src/qt/gear/qutils.h \
    src/qt/i18nl10n.h \
    src/qt/outline_view_presenter.h \
    src/qt/outline_view.h \
    src/qt/outline_header_edit_presenter.h \
    src/qt/outline_header_view_presenter.h \
    src/qt/outline_header_edit_view.h \
    src/qt/outline_header_view.h \
    lib/src/gear/color.h \
    lib/src/version.h \
    lib/src/config/color.h \
    lib/src/mind/mind_listener.h \
    lib/src/install/installer.h \
    src/qt/note_editor_view.h \
    src/qt/note_edit_view.h \
    src/qt/dialogs/note_new_dialog.h \
    src/qt/dialogs/outline_new_dialog.h \
    src/qt/dialogs/fts_dialog.h \
    src/qt/dialogs/find_outline_by_name_dialog.h \
    src/qt/dialogs/find_note_by_name_dialog.h \
    lib/src/model/mind_entity.h \
    src/qt/widgets/edit_name_and_buttons_panel.h \
    src/qt/dialogs/note_edit_dialog.h \
    src/qt/dialogs/configuration_dialog.h \
    src/qt/widgets/edit_tags_panel.h \
    src/qt/widgets/labeled_edit_line_panel.h \
    src/qt/widgets/line_number_panel.h \
    src/qt/dialogs/outline_header_edit_dialog.h \
    src/qt/widgets/urgency_combo_box.h \
    src/qt/widgets/importance_combo_box.h \
    src/qt/dialogs/refactor_note_to_outline_dialog.h

SOURCES += ./lib/src/mind/planner.cpp \
    lib/src/mind/memory_dwell.cpp \
    lib/src/mind/memory.cpp \
    lib/src/mind/mind.cpp \
    lib/src/mind/ontology/ontology.cpp \
    lib/src/mind/ontology/thing.cpp \
    lib/src/mind/working_memory.cpp \
    lib/src/repository_indexer.cpp \
    lib/src/model/attachment.cpp \
    lib/src/model/outline.cpp \
    lib/src/model/note_type.cpp \
    lib/src/model/outline_type.cpp \
    lib/src/model/note.cpp \
    lib/src/model/tag.cpp \
    lib/src/gear/file_utils.cpp \
    lib/src/gear/string_utils.cpp \
    lib/src/gear/datetime_utils.cpp \
    lib/src/representations/markdown/markdown_lexer_sections.cpp \
    lib/src/representations/markdown/markdown_section_metadata.cpp \
    lib/src/representations/markdown/markdown_outline_metadata.cpp \
    lib/src/representations/markdown/markdown_outline_representation.cpp \
    lib/src/representations/markdown/markdown_ast_node.cpp \
    lib/src/representations/markdown/markdown_lexem.cpp \
    lib/src/representations/markdown/markdown_note_metadata.cpp \
    lib/src/representations/markdown/markdown_parser_sections.cpp \
    lib/src/representations/html/html_outline_representation.cpp \
    lib/src/representations/outline_representation.cpp \
    src/qt/mindforger.cpp \
    src/qt/3rdparty/elasticnodes/edge.cpp \
    src/qt/3rdparty/elasticnodes/graphwidget.cpp \
    src/qt/3rdparty/elasticnodes/node.cpp \
    src/qt/main_window_view.cpp \
    src/qt/outlines_table_view.cpp \
    src/qt/outlines_table_presenter.cpp \
    src/qt/outline_tree_view.cpp \
    src/qt/outline_tree_presenter.cpp \
    src/qt/status_bar_view.cpp \
    src/qt/status_bar_presenter.cpp \
    src/qt/main_window_presenter.cpp \
    src/qt/main_menu_view.cpp \
    src/qt/main_menu_presenter.cpp \
    src/qt/cli_n_breadcrumbs_presenter.cpp \
    src/qt/cli_n_breadcrumbs_view.cpp \
    src/qt/orloj_view.cpp \
    src/qt/orloj_presenter.cpp \
    src/qt/notes_table_view.cpp \
    src/qt/notes_table_presenter.cpp \
    src/qt/notes_table_model.cpp \
    src/qt/outline_tree_model.cpp \
    src/qt/outlines_table_model.cpp \
    src/qt/note_view_model.cpp \
    src/qt/note_view_presenter.cpp \
    src/qt/note_view.cpp \
    src/qt/note_edit_model.cpp \
    src/qt/note_edit_presenter.cpp \
    src/qt/look_n_feel.cpp \
    src/qt/html_delegate.cpp \
    src/qt/note_edit_highlight.cpp \
    lib/src/persistence/filesystem_persistence.cpp \
    lib/src/model/stencil.cpp \
    src/qt/gear/qutils.cpp \
    src/qt/i18nl10n.cpp \
    src/qt/outline_view_presenter.cpp \
    src/qt/outline_view.cpp \
    src/qt/outline_header_edit_presenter.cpp \
    src/qt/outline_header_view_presenter.cpp \
    src/qt/outline_header_edit_view.cpp \
    src/qt/outline_header_view.cpp \
    lib/src/config/configuration.cpp \
    lib/src/representations/markdown/markdown.cpp \
    lib/src/install/installer.cpp \
    src/qt/note_editor_view.cpp \
    src/qt/note_edit_view.cpp \
    src/qt/dialogs/note_new_dialog.cpp \
    src/qt/dialogs/outline_new_dialog.cpp \
    src/qt/dialogs/fts_dialog.cpp \
    src/qt/dialogs/find_outline_by_name_dialog.cpp \
    src/qt/dialogs/find_note_by_name_dialog.cpp \
    src/qt/widgets/edit_name_and_buttons_panel.cpp \
    src/qt/dialogs/note_edit_dialog.cpp \
    src/qt/dialogs/configuration_dialog.cpp \
    src/qt/widgets/edit_tags_panel.cpp \
    src/qt/widgets/labeled_edit_line_panel.cpp \
    src/qt/widgets/line_number_panel.cpp \
    src/qt/dialogs/outline_header_edit_dialog.cpp \
    src/qt/widgets/urgency_combo_box.cpp \
    src/qt/widgets/importance_combo_box.cpp \
    src/qt/dialogs/refactor_note_to_outline_dialog.cpp

# Development environment remarks:
# - Beast: GCC 5.4.0, Qt 5.5.1
# - S7   : GCC 4.8.5, Qt 5.2.1.
# - GCC: -std=c++0x ~ -std=c++11

QMAKE_CXX = ccache g++
QMAKE_CXXFLAGS += -pedantic -std=c++11

TRANSLATIONS = src/qt/translations/mindforger_en.ts \
    src/qt/translations/mindforger_de.ts \
    src/qt/translations/mindforger_fr.ts \
    src/qt/translations/mindforger_es.ts \
    src/qt/translations/mindforger_cs.ts

RESOURCES += \
    mf-resources.qrc

# eof
