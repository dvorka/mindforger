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

# dependencies
INCLUDEPATH += $$PWD/../deps/discount
DEPENDPATH += $$PWD/../deps/discount
INCLUDEPATH += $$PWD/../lib/src
DEPENDPATH += $$PWD/../lib/src

# -L where to look for library, -l link the library
LIBS += -L$$OUT_PWD/../lib -lmindforger
# MF must link against ldiscount.a (built in ../deps/discount) - NOT lmarkdown
LIBS += -L$$OUT_PWD/../deps/discount -ldiscount

# development environment remarks:
# - Beast 64b:   GCC 5.4.0, Qt 5.5.1
# - S7    64b:   GCC 4.8.5, Qt 5.2.1
# - Win10 64b: MinGW 4.9.2, Qt 5.10.0
#
# - GCC: -std=c++0x ~ -std=c++11

# compiler options (if not qmake CONFIG+=travisci)
!travisci {
  QMAKE_CXX = ccache g++
}
QMAKE_CXXFLAGS += -pedantic -std=c++11

HEADERS += \
    ./src/qt/3rdparty/elasticnodes/edge.h \
    ./src/qt/3rdparty/elasticnodes/graphwidget.h \
    ./src/qt/3rdparty/elasticnodes/node.h \
    ./src/qt/main_window_view.h \
    ./src/qt/outlines_table_view.h \
    ./src/qt/outlines_table_presenter.h \
    ./src/qt/outline_tree_view.h \
    ./src/qt/outline_tree_presenter.h \
    ./src/qt/status_bar_view.h \
    ./src/qt/status_bar_presenter.h \
    ./src/qt/main_window_presenter.h \
    ./src/qt/main_menu_view.h \
    ./src/qt/main_menu_presenter.h \
    ./src/qt/cli_n_breadcrumbs_presenter.h \
    ./src/qt/cli_n_breadcrumbs_view.h \
    ./src/qt/orloj_view.h \
    ./src/qt/orloj_presenter.h \
    ./src/qt/notes_table_view.h \
    ./src/qt/notes_table_presenter.h \
    ./src/qt/notes_table_model.h \
    ./src/qt/outline_tree_model.h \
    ./src/qt/outlines_table_model.h \
    ./src/qt/model_meta_definitions.h \
    ./src/qt/note_view_model.h \
    ./src/qt/note_view_presenter.h \
    ./src/qt/note_view.h \
    ./src/qt/note_edit_model.h \
    ./src/qt/note_edit_presenter.h \
    ./src/qt/look_n_feel.h \
    ./src/qt/html_delegate.h \
    ./src/qt/note_edit_highlight.h \
    ./src/qt/gear/qutils.h \
    ./src/qt/i18nl10n.h \
    ./src/qt/outline_view_presenter.h \
    ./src/qt/outline_view.h \
    ./src/qt/outline_header_edit_presenter.h \
    ./src/qt/outline_header_view_presenter.h \
    ./src/qt/outline_header_edit_view.h \
    ./src/qt/outline_header_view.h \
    ./src/qt/note_editor_view.h \
    ./src/qt/note_edit_view.h \
    ./src/qt/dialogs/note_new_dialog.h \
    ./src/qt/dialogs/outline_new_dialog.h \
    ./src/qt/dialogs/fts_dialog.h \
    ./src/qt/dialogs/find_outline_by_name_dialog.h \
    ./src/qt/dialogs/find_note_by_name_dialog.h \
    ./src/qt/widgets/edit_name_and_buttons_panel.h \
    ./src/qt/dialogs/note_edit_dialog.h \
    ./src/qt/dialogs/configuration_dialog.h \
    ./src/qt/widgets/edit_tags_panel.h \
    ./src/qt/widgets/labeled_edit_line_panel.h \
    ./src/qt/widgets/line_number_panel.h \
    ./src/qt/dialogs/outline_header_edit_dialog.h \
    ./src/qt/widgets/urgency_combo_box.h \
    ./src/qt/widgets/importance_combo_box.h \
    ./src/qt/dialogs/refactor_note_to_outline_dialog.h \
    ./src/qt/recent_files_menu.h \
    ./src/qt/dialogs/scope_dialog.h \
    ./src/qt/dialogs/find_outline_by_tag_dialog.h \
    ./src/qt/dialogs/find_note_by_tag_dialog.h \
    src/qt/outline_view_splitter.h \
    src/qt/assoc_leaderboard_model.h \
    src/qt/assoc_leaderboard_presenter.h \
    src/qt/assoc_leaderboard_view.h \
    src/qt/gear/async_task_notifications_distributor.h \
    src/qt/dialogs/insert_image_dialog.h \
    src/qt/dialogs/insert_link_dialog.h \
    src/qt/dialogs/rows_and_depth_dialog.h \
    src/qt/dialogs/new_file_dialog.h \
    src/qt/dialogs/new_repository_dialog.h

SOURCES += \
    ./src/qt/mindforger.cpp \
    ./src/qt/3rdparty/elasticnodes/edge.cpp \
    ./src/qt/3rdparty/elasticnodes/graphwidget.cpp \
    ./src/qt/3rdparty/elasticnodes/node.cpp \
    ./src/qt/main_window_view.cpp \
    ./src/qt/outlines_table_view.cpp \
    ./src/qt/outlines_table_presenter.cpp \
    ./src/qt/outline_tree_view.cpp \
    ./src/qt/outline_tree_presenter.cpp \
    ./src/qt/status_bar_view.cpp \
    ./src/qt/status_bar_presenter.cpp \
    ./src/qt/main_window_presenter.cpp \
    ./src/qt/main_menu_view.cpp \
    ./src/qt/main_menu_presenter.cpp \
    ./src/qt/cli_n_breadcrumbs_presenter.cpp \
    ./src/qt/cli_n_breadcrumbs_view.cpp \
    ./src/qt/orloj_view.cpp \
    ./src/qt/orloj_presenter.cpp \
    ./src/qt/notes_table_view.cpp \
    ./src/qt/notes_table_presenter.cpp \
    ./src/qt/notes_table_model.cpp \
    ./src/qt/outline_tree_model.cpp \
    ./src/qt/outlines_table_model.cpp \
    ./src/qt/note_view_model.cpp \
    ./src/qt/note_view_presenter.cpp \
    ./src/qt/note_view.cpp \
    ./src/qt/note_edit_model.cpp \
    ./src/qt/note_edit_presenter.cpp \
    ./src/qt/look_n_feel.cpp \
    ./src/qt/html_delegate.cpp \
    ./src/qt/note_edit_highlight.cpp \
    ./src/qt/gear/qutils.cpp \
    ./src/qt/i18nl10n.cpp \
    ./src/qt/outline_view_presenter.cpp \
    ./src/qt/outline_view.cpp \
    ./src/qt/outline_header_edit_presenter.cpp \
    ./src/qt/outline_header_view_presenter.cpp \
    ./src/qt/outline_header_edit_view.cpp \
    ./src/qt/outline_header_view.cpp \
    ./src/qt/note_editor_view.cpp \
    ./src/qt/note_edit_view.cpp \
    ./src/qt/dialogs/note_new_dialog.cpp \
    ./src/qt/dialogs/outline_new_dialog.cpp \
    ./src/qt/dialogs/fts_dialog.cpp \
    ./src/qt/dialogs/find_outline_by_name_dialog.cpp \
    ./src/qt/dialogs/find_note_by_name_dialog.cpp \
    ./src/qt/widgets/edit_name_and_buttons_panel.cpp \
    ./src/qt/dialogs/note_edit_dialog.cpp \
    ./src/qt/dialogs/configuration_dialog.cpp \
    ./src/qt/widgets/edit_tags_panel.cpp \
    ./src/qt/widgets/labeled_edit_line_panel.cpp \
    ./src/qt/widgets/line_number_panel.cpp \
    ./src/qt/dialogs/outline_header_edit_dialog.cpp \
    ./src/qt/widgets/urgency_combo_box.cpp \
    ./src/qt/widgets/importance_combo_box.cpp \
    ./src/qt/dialogs/refactor_note_to_outline_dialog.cpp \
    ./src/qt/recent_files_menu.cpp \
    ./src/qt/dialogs/scope_dialog.cpp \
    ./src/qt/dialogs/find_outline_by_tag_dialog.cpp \
    ./src/qt/dialogs/find_note_by_tag_dialog.cpp \
    src/qt/outline_view_splitter.cpp \
    src/qt/assoc_leaderboard_model.cpp \
    src/qt/assoc_leaderboard_presenter.cpp \
    src/qt/assoc_leaderboard_view.cpp \
    src/qt/gear/async_task_notifications_distributor.cpp \
    src/qt/dialogs/insert_image_dialog.cpp \
    src/qt/dialogs/insert_link_dialog.cpp \
    src/qt/dialogs/rows_and_depth_dialog.cpp \
    src/qt/dialogs/new_file_dialog.cpp \
    src/qt/dialogs/new_repository_dialog.cpp

# L10n
#./resources/qt/translations/mindforger_de.ts
#./resources/qt/translations/mindforger_fr.ts
#./resources/qt/translations/mindforger_es.ts
TRANSLATIONS = \
    ./resources/qt/translations/mindforger_en.ts \
    ./resources/qt/translations/mindforger_cs.ts

RESOURCES += \
    ./mf-resources.qrc

# ########################################
# Linux installation: make install
# ########################################
# See http://doc.qt.io/qt-5/qmake-advanced-usage.html

binfile.files += mindforger
binfile.path = /usr/bin/
INSTALLS += binfile

# eof
