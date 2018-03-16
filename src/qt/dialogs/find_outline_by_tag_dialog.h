/*
 find_outline_by_tag.h     MindForger thinking notebook

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
#ifndef M8RUI_FIND_OUTLINE_BY_TAG_DIALOG_H
#define M8RUI_FIND_OUTLINE_BY_TAG_DIALOG_H

#include <vector>

#include <QtWidgets>

#include "../../../lib/src/mind/ontology/thing_class_rel_triple.h"
#include "../../../lib/src/mind/ontology/ontology.h"

#include "../widgets/edit_tags_panel.h"

namespace m8r {

class FindOutlineByTagDialog : public QDialog
{
    Q_OBJECT

private:
    Ontology& ontology;

    EditTagsPanel* editTagsGroup;
    QListView* listView;
    QStringList listViewStrings;
    QStringListModel listViewModel;
    QPushButton* closeButton;

    Thing* choice;
    std::vector<Thing*> things;

protected:
    QLabel* label;
    QPushButton* findButton;

public:
    explicit FindOutlineByTagDialog(Ontology& ontology, QWidget* parent);
    FindOutlineByTagDialog(const FindOutlineByTagDialog&) = delete;
    FindOutlineByTagDialog(const FindOutlineByTagDialog&&) = delete;
    FindOutlineByTagDialog &operator=(const FindOutlineByTagDialog&) = delete;
    FindOutlineByTagDialog &operator=(const FindOutlineByTagDialog&&) = delete;
    ~FindOutlineByTagDialog();

    QPushButton* getFindButton() const { return findButton; }
    Thing* getChoice() const { return choice; }

    void show(std::vector<Thing*>& outlines, std::vector<std::string>* customizedNames=nullptr);

signals:
    void searchFinished();

private slots:
    void enableFindButton(const QString &text);
    void handleChoice();
    void handleReturn();
};

}
#endif // M8RUI_FIND_OUTLINE_BY_TAG_DIALOG_H
