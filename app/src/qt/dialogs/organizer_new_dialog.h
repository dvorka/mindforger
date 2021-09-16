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

#include "find_outline_by_name_dialog.h"

namespace m8r {

/**
 * @brief Create new organizer.
 *
 * Implementation plan:
 *
 * 1. tags only (filter: outlines; no sorting)
 * 2. tags only (filter: outlines, notes, o+n; no sorting)
 * 3. tags + scope (scope disables non-meaningful actions)
 * 4. sorting
 */
class OrganizerNewDialog : public QDialog
{
    Q_OBJECT

private:
    Ontology& ontology;

    QPushButton* closeButton;

    std::vector<Outline*> outlines;

protected:
    enum ThingsMode {
        OUTLINES,
        NOTES
    };

    ThingsMode mode;

    FindOutlineByNameDialog* findOutlineByNameDialog;

    QLabel* nameLabel;
    QLineEdit* nameEdit;
    EditTagsPanel* upperRightTags;
    EditTagsPanel* lowerRightTags;
    EditTagsPanel* upperLeftTags;
    EditTagsPanel* lowerLeftTags;
    QLabel* oScopeLabel;
    QLineEdit* oScopeEdit;
    QPushButton* findOutlineButton;
    QLabel* sortByLabel;
    QComboBox* sortByCombo;
    QLabel* filterByLabel;
    QComboBox* filterByCombo;

    QPushButton* createButton;

public:
    explicit OrganizerNewDialog(
        Ontology& ontology,
        QWidget* parent
    );
    OrganizerNewDialog(const OrganizerNewDialog&) = delete;
    OrganizerNewDialog(const OrganizerNewDialog&&) = delete;
    OrganizerNewDialog &operator=(const OrganizerNewDialog&) = delete;
    OrganizerNewDialog &operator=(const OrganizerNewDialog&&) = delete;
    ~OrganizerNewDialog();

    QPushButton* getFindButton() const { return createButton; }

    QString getOrganizerName() const { return nameEdit->text(); }
    std::vector<std::string>& getUpperLeftChosenTags(std::vector<std::string>& tags) const
    {
        return upperLeftTags->getTagsAsStrings(tags);
    }
    std::vector<std::string>& getUpperRightChosenTags(std::vector<std::string>& tags) const
    {
        return upperRightTags->getTagsAsStrings(tags);
    }
    std::vector<std::string>& getLowerLeftChosenTags(std::vector<std::string>& tags) const
    {
        return lowerLeftTags->getTagsAsStrings(tags);
    }
    std::vector<std::string>& getLowerRightChosenTags(std::vector<std::string>& tags) const
    {
        return lowerRightTags->getTagsAsStrings(tags);
    }
    int getFilterBy() const {
        return this->filterByCombo->currentData().toInt();
    }
    int getSortBy() const {
        return this->sortByCombo->currentData().toInt();
    }
    std::string getOutlineScope() const {
        return oScopeEdit->text().toStdString();
    }

    void show(
        const std::vector<Outline*>& outlines,
        const std::vector<const Tag*>* tags=nullptr
    );

signals:
    void createFinished();

private slots:
    void handleFindOutline();
    void handleFindOutlineChoice();

    void handleCreate();
};

}
#endif // M8RUI_ORGANIZER_NEW_DIALOG_H
