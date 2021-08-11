/*
 organizer_new_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_ORGANIZER_NEW_DIALOG_H
#define M8RUI_ORGANIZER_NEW_DIALOG_H

#include <vector>

#include <QtWidgets>

#include "../../lib/src/mind/ontology/thing_class_rel_triple.h"
#include "../../lib/src/mind/ontology/ontology.h"
#include "../../lib/src/model/outline.h"

#include "../widgets/edit_tags_panel.h"

namespace m8r {

class OrganizerNewDialog : public QDialog
{
    Q_OBJECT

private:
    Ontology& ontology;

    QPushButton* closeButton;

    Thing* choice;
    std::vector<Thing*> things;

protected:
    enum ThingsMode {
        OUTLINES,
        NOTES
    };

    ThingsMode mode;
    EditTagsPanel* upperRighTags;
    EditTagsPanel* lowerRighTags;
    EditTagsPanel* upperLeftTags;
    EditTagsPanel* lowerLeftTags;
    QPushButton* findButton;

public:
    explicit OrganizerNewDialog(Ontology& ontology, QWidget* parent);
    OrganizerNewDialog(const OrganizerNewDialog&) = delete;
    OrganizerNewDialog(const OrganizerNewDialog&&) = delete;
    OrganizerNewDialog &operator=(const OrganizerNewDialog&) = delete;
    OrganizerNewDialog &operator=(const OrganizerNewDialog&&) = delete;
    ~OrganizerNewDialog();

    QPushButton* getFindButton() const { return findButton; }
    void getChosenTags(std::vector<const Tag*>* tags);
    Thing* getChoice() const { return choice; }

    void show(std::vector<const Tag*>* tags=nullptr);

signals:
    void searchFinished();
};

}
#endif // M8RUI_ORGANIZER_NEW_DIALOG_H
