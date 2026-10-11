/*
 recent_notes_model.h     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef M8RUI_RECENT_NOTES_MODEL_H
#define M8RUI_RECENT_NOTES_MODEL_H

#include <QtWidgets>

#include "model_meta_definitions.h"
#include "../../lib/src/model/recent_notes.h"
#include "../../lib/src/representations/html/html_outline_representation.h"

namespace m8r {

/**
 * @brief Recent Notes tree model: groups of consecutive Notebook Notes from the history.
 */
class RecentNotesModel : public QStandardItemModel
{
    Q_OBJECT

    HtmlOutlineRepresentation* htmlRepresentation;

public:
    static const int ROLE_OUTLINE = Qt::UserRole + 1;
    static const int ROLE_NOTE = Qt::UserRole + 2;

    static const int COLUMN_NAME = 0;
    static const int COLUMN_READS = 1;
    static const int COLUMN_WRITES = 2;
    static const int COLUMN_READ = 3;
    static const int COLUMN_MODIFIED = 4;
    static const int COLUMN_COUNT = 5;

public:
    explicit RecentNotesModel(QObject* parent, HtmlOutlineRepresentation* htmlRepresentation);
    RecentNotesModel(const RecentNotesModel&) = delete;
    RecentNotesModel(const RecentNotesModel&&) = delete;
    RecentNotesModel &operator=(const RecentNotesModel&) = delete;
    RecentNotesModel &operator=(const RecentNotesModel&&) = delete;
    ~RecentNotesModel();

    void removeAllRows();
    void addGroup(const RecentNotes::Group& group);

    static Outline* getOutline(const QModelIndex& index);
    static Note* getNote(const QModelIndex& index);

private:
    QStandardItem* createNameItem(
        const std::string& name,
        const std::string& fallbackName,
        const std::vector<const Tag*>* tags
    );
    QStandardItem* createNumberItem(u_int32_t number);
};

}
#endif // M8RUI_RECENT_NOTES_MODEL_H
