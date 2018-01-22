/*
 edit_tags_panel.h     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_EDIT_TAGS_PANEL_H
#define M8RUI_EDIT_TAGS_PANEL_H

#include <QtWidgets>

#include "lib/src/mind/ontology/ontology.h"

namespace m8r {

class EditTagsPanel : public QGroupBox
{
    Q_OBJECT

private:
    Ontology& ontology;
    std::vector<const Tag*> tags;

    QVBoxLayout* layout;

    QLineEdit* lineEdit;
    QCompleter* completer;
    QListView* listView;
    QPushButton* addButton;
    QPushButton* removeButton;

    QStringList completerStrings;
    QStringListModel listViewModel;
    QStringList listViewStrings;

public:
    explicit EditTagsPanel(Ontology& ontology, QWidget* parent);
    EditTagsPanel(const EditTagsPanel&) = delete;
    EditTagsPanel(const EditTagsPanel&&) = delete;
    EditTagsPanel &operator=(const EditTagsPanel&) = delete;
    EditTagsPanel &operator=(const EditTagsPanel&&) = delete;
    ~EditTagsPanel();

    void refresh(const std::vector<const Tag*>& noteTags);
    std::vector<const Tag*>& getTags(void);

private slots:
    void slotAddTag(void);
    void slotRemoveTag(void);
};

}
#endif // M8RUI_EDIT_TAGS_PANEL_H
