/*
 edit_tags_panel.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "../../lib/src/mind/ontology/ontology.h"

namespace m8r {

class EditTagsPanel : public QGroupBox
{
    Q_OBJECT

    class MyLineEdit : public QLineEdit
    {
    private:
        EditTagsPanel* tagsPanel;
    public:
        explicit MyLineEdit(EditTagsPanel* tagsPanel, QWidget* parent) : QLineEdit(parent), tagsPanel(tagsPanel) {}
        virtual void keyPressEvent(QKeyEvent* event) override {
            if(event->modifiers() & Qt::ControlModifier){
                switch(event->key()) {
                case Qt::Key_Return: // Qt::Key_Enter is keypad Enter
                    tagsPanel->slotAddTag();
                    break;
                }
            }
            QLineEdit::keyPressEvent(event);
        }
    };

private:
    Ontology& ontology;
    std::vector<const Tag*> tags;

    QVBoxLayout* layout;

    MyLineEdit* lineEdit;
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

    QLineEdit* getLineEdit() const { return lineEdit; }
    void clearTagList();
    void refreshOntologyTags();
    void refresh(const std::vector<const Tag*>* noteTags);
    const std::vector<const Tag*>& getTags();
    void setTags(const std::vector<const Tag*>& tags);
    void setFocusAddingTag() {
        lineEdit->setFocus();
        lineEdit->selectAll();
    }
    int getTagCount() const {
        return listView->model()->rowCount();
    }

signals:
    void signalTagSelectionChanged();

public slots:
    void slotAddTag();
private slots:
    void slotRemoveTag();
};

}
#endif // M8RUI_EDIT_TAGS_PANEL_H
