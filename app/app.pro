# mindforger-app.pro     Qt project file for MindForger
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

# webengine
#QT += webenginewidgets

# webkit is supposed to be obsoleted by webengine, but webengine is disabled
# on Linux since Qt 5.9 due to its tragic performance > conditional compilation
# seems to be the only way (webkit on Linux, webengine on Windows)
QT += webkit
QT += webkitwidgets

INCLUDEPATH += $$PWD/../deps/discount
DEPENDPATH += $$PWD/../deps/discount
INCLUDEPATH += $$PWD/../lib/src
DEPENDPATH += $$PWD/../lib/src

# -L where to look for library, -l link the library
LIBS += -L$$OUT_PWD/../lib -lmindforger
LIBS += -L$$OUT_PWD/../deps/discount -ldiscount

HEADERS += \
    ./qt/3rdparty/elasticnodes/edge.h \
    ./qt/3rdparty/elasticnodes/graphwidget.h \
    ./qt/3rdparty/elasticnodes/node.h \
    ./qt/main_window_view.h \
    ./qt/outlines_table_view.h \
    ./qt/outlines_table_presenter.h \
    ./qt/outline_tree_view.h \
    ./qt/outline_tree_presenter.h \
    ./qt/status_bar_view.h \
    ./qt/status_bar_presenter.h \
    ./qt/main_window_presenter.h \
    ./qt/main_menu_view.h \
    ./qt/main_menu_presenter.h \
    ./qt/cli_n_breadcrumbs_presenter.h \
    ./qt/cli_n_breadcrumbs_view.h \
    ./qt/orloj_view.h \
    ./qt/orloj_presenter.h \
    ./qt/notes_table_view.h \
    ./qt/notes_table_presenter.h \
    ./qt/notes_table_model.h \
    ./qt/outline_tree_model.h \
    ./qt/outlines_table_model.h \
    ./qt/model_meta_definitions.h \
    ./qt/note_view_model.h \
    ./qt/note_view_presenter.h \
    ./qt/note_view.h \
    ./qt/note_edit_model.h \
    ./qt/note_edit_presenter.h \
    ./qt/look_n_feel.h \
    ./qt/html_delegate.h \
    ./qt/note_edit_highlight.h \
    ./qt/gear/qutils.h \
    ./qt/i18nl10n.h \
    ./qt/outline_view_presenter.h \
    ./qt/outline_view.h \
    ./qt/outline_header_edit_presenter.h \
    ./qt/outline_header_view_presenter.h \
    ./qt/outline_header_edit_view.h \
    ./qt/outline_header_view.h \
    ./qt/note_editor_view.h \
    ./qt/note_edit_view.h \
    ./qt/dialogs/note_new_dialog.h \
    ./qt/dialogs/outline_new_dialog.h \
    ./qt/dialogs/fts_dialog.h \
    ./qt/dialogs/find_outline_by_name_dialog.h \
    ./qt/dialogs/find_note_by_name_dialog.h \
    ./qt/widgets/edit_name_and_buttons_panel.h \
    ./qt/dialogs/note_edit_dialog.h \
    ./qt/dialogs/configuration_dialog.h \
    ./qt/widgets/edit_tags_panel.h \
    ./qt/widgets/labeled_edit_line_panel.h \
    ./qt/widgets/line_number_panel.h \
    ./qt/dialogs/outline_header_edit_dialog.h \
    ./qt/widgets/urgency_combo_box.h \
    ./qt/widgets/importance_combo_box.h \
    ./qt/dialogs/refactor_note_to_outline_dialog.h \
    ./qt/recent_files_menu.h \
    ./qt/dialogs/forget_dialog.h \
    ./qt/dialogs/time_scope_dialog.h \
    ./qt/dialogs/find_outline_by_tag_dialog.h \
    ./qt/dialogs/find_note_by_tag_dialog.h

SOURCES += \
    ./qt/mindforger.cpp \
    ./qt/3rdparty/elasticnodes/edge.cpp \
    ./qt/3rdparty/elasticnodes/graphwidget.cpp \
    ./qt/3rdparty/elasticnodes/node.cpp \
    ./qt/main_window_view.cpp \
    ./qt/outlines_table_view.cpp \
    ./qt/outlines_table_presenter.cpp \
    ./qt/outline_tree_view.cpp \
    ./qt/outline_tree_presenter.cpp \
    ./qt/status_bar_view.cpp \
    ./qt/status_bar_presenter.cpp \
    ./qt/main_window_presenter.cpp \
    ./qt/main_menu_view.cpp \
    ./qt/main_menu_presenter.cpp \
    ./qt/cli_n_breadcrumbs_presenter.cpp \
    ./qt/cli_n_breadcrumbs_view.cpp \
    ./qt/orloj_view.cpp \
    ./qt/orloj_presenter.cpp \
    ./qt/notes_table_view.cpp \
    ./qt/notes_table_presenter.cpp \
    ./qt/notes_table_model.cpp \
    ./qt/outline_tree_model.cpp \
    ./qt/outlines_table_model.cpp \
    ./qt/note_view_model.cpp \
    ./qt/note_view_presenter.cpp \
    ./qt/note_view.cpp \
    ./qt/note_edit_model.cpp \
    ./qt/note_edit_presenter.cpp \
    ./qt/look_n_feel.cpp \
    ./qt/html_delegate.cpp \
    ./qt/note_edit_highlight.cpp \
    ./qt/gear/qutils.cpp \
    ./qt/i18nl10n.cpp \
    ./qt/outline_view_presenter.cpp \
    ./qt/outline_view.cpp \
    ./qt/outline_header_edit_presenter.cpp \
    ./qt/outline_header_view_presenter.cpp \
    ./qt/outline_header_edit_view.cpp \
    ./qt/outline_header_view.cpp \
    ./qt/note_editor_view.cpp \
    ./qt/note_edit_view.cpp \
    ./qt/dialogs/note_new_dialog.cpp \
    ./qt/dialogs/outline_new_dialog.cpp \
    ./qt/dialogs/fts_dialog.cpp \
    ./qt/dialogs/find_outline_by_name_dialog.cpp \
    ./qt/dialogs/find_note_by_name_dialog.cpp \
    ./qt/widgets/edit_name_and_buttons_panel.cpp \
    ./qt/dialogs/note_edit_dialog.cpp \
    ./qt/dialogs/configuration_dialog.cpp \
    ./qt/widgets/edit_tags_panel.cpp \
    ./qt/widgets/labeled_edit_line_panel.cpp \
    ./qt/widgets/line_number_panel.cpp \
    ./qt/dialogs/outline_header_edit_dialog.cpp \
    ./qt/widgets/urgency_combo_box.cpp \
    ./qt/widgets/importance_combo_box.cpp \
    ./qt/dialogs/refactor_note_to_outline_dialog.cpp \
    ./qt/recent_files_menu.cpp \
    ./qt/dialogs/forget_dialog.cpp \
    ./qt/dialogs/time_scope_dialog.cpp \
    ./qt/dialogs/find_outline_by_tag_dialog.cpp \
    ./qt/dialogs/find_note_by_tag_dialog.cpp

# Development environment remarks:
# - Beast 64b:   GCC 5.4.0, Qt 5.5.1
# - S7    64b:   GCC 4.8.5, Qt 5.2.1
# - Win10 64b: MinGW 4.9.2, Qt 5.10.0
#
# - GCC: -std=c++0x ~ -std=c++11

QMAKE_CXX = ccache g++
QMAKE_CXXFLAGS += -pedantic -std=c++11

TRANSLATIONS = \
    ./qt/translations/mindforger_en.ts \
    ./qt/translations/mindforger_de.ts \
    ./qt/translations/mindforger_fr.ts \
    ./qt/translations/mindforger_es.ts \
    ./qt/translations/mindforger_cs.ts

RESOURCES += \
    ../mf-resources.qrc

# eof
