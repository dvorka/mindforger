/*
 edit_tags_panel.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "mf_widgets.h"

namespace m8r {

class EditTagsPanel : public QGroupBox
{
    Q_OBJECT

    class MyLineEdit : public QLineEdit
    {
    private:
        EditTagsPanel* tagsPanel;
    public:
        explicit MyLineEdit(EditTagsPanel* tagsPanel, QWidget* parent)
            : QLineEdit(parent), tagsPanel(tagsPanel)
        {}

        virtual void keyPressEvent(QKeyEvent* event) override {
            if(event->modifiers() & Qt::ControlModifier){
                switch(event->key()) {
                case Qt::Key_Return: // Qt::Key_Enter is keypad Enter
                    tagsPanel->slotAddTag();
                    break;
                }
            } else {
                if(!text().size()) {
                    switch(event->key()) {
                    case Qt::Key_Down:
                        tagsPanel->setFocusTagList();
                        break;
                    }
                }
            }
            QLineEdit::keyPressEvent(event);

            // notify about key pressed
            tagsPanel->customLineEditKeyPressEvent(event);
        }
    };

private:
    MfWidgetMode mode;

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
    explicit EditTagsPanel(MfWidgetMode mode, Ontology& ontology, QWidget* parent);
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
    std::vector<std::string>& getTagsAsStrings(std::vector<std::string>& tags) const;
    std::set<std::string>& getTagsAsStringSet(std::set<std::string>& tagSet) const;
    void setTags(const std::vector<const Tag*>& tags);
    void setTagsAsStrings(const std::vector<std::string>& tagsStrings);
    void setTagsAsStrings(
        const std::set<std::string>& tagsStrings
    );
    void setFocusAddingTag() {
        lineEdit->setFocus();
        lineEdit->selectAll();
    }
    void setFocusTagList() {
        listView->setFocus();
    }
    int getTagCount() const {
        return listView->model()->rowCount();
    }

    /**
     *  @brief Callback used to notify tag panel about key press event in tag name edit line.
     */
    void customLineEditKeyPressEvent(QKeyEvent* event);

signals:
    void signalTagSelectionChanged();

public slots:
    void slotAddTag();
private slots:
    void slotRemoveTag();
};

}
#endif // M8RUI_EDIT_TAGS_PANEL_H
