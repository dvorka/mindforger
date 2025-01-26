/*
 organizer_new_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "../../lib/src/model/eisenhower_matrix.h"

#include "../widgets/edit_tags_panel.h"
#include "find_outline_by_name_dialog.h"

namespace m8r {

/**
 * @brief Dialog to create new organizer or edit an existing one.
 */
class OrganizerNewDialog : public QDialog
{
    Q_OBJECT

    static constexpr const auto TITLE_UPPER_LEFT_EM = "Upper left quadrant tags";
    static constexpr const auto TITLE_UPPER_RIGHT_EM = "Upper right quadrant tags";
    static constexpr const auto TITLE_LOWER_LEFT_EM = "Lower left quadrant tags";
    static constexpr const auto TITLE_LOWER_RIGHT_EM = "Lower right quadrant tags";

    static constexpr const auto TITLE_UPPER_LEFT_KANBAN = "The first column";
    static constexpr const auto TITLE_UPPER_RIGHT_KANBAN = "The second column";
    static constexpr const auto TITLE_LOWER_LEFT_KANBAN = "The third column";
    static constexpr const auto TITLE_LOWER_RIGHT_KANBAN = "The fourth column";

private:
    Ontology& ontology;

    QPushButton* closeButton;

    std::vector<Outline*> outlines;

    Outline* oScopeOutline;
    Organizer* organizerToEdit;

protected:
    enum ThingsMode {
        OUTLINES,
        NOTES
    };

    ThingsMode mode;

    FindOutlineByNameDialog* findOutlineByNameDialog;

    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QLabel* typeLabel;
    QComboBox* typeCombo;
    EditTagsPanel* upperRightTags;
    EditTagsPanel* lowerRightTags;
    EditTagsPanel* upperLeftTags;
    EditTagsPanel* lowerLeftTags;
    QLabel* oScopeLabel;
    QLineEdit* oScopeEdit;
    QPushButton* findOutlineButton;
    QPushButton* clearOutlineButton;
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
    int getOrganizerType() const {
        return this->typeCombo->currentData().toInt();
    }
    std::set<std::string>& getUpperLeftChosenTags(std::set<std::string>& tags) const {
        return upperLeftTags->getTagsAsStringSet(tags);
    }
    std::set<std::string>& getUpperRightChosenTags(std::set<std::string>& tags) const {
        return upperRightTags->getTagsAsStringSet(tags);
    }
    std::set<std::string>& getLowerLeftChosenTags(std::set<std::string>& tags) const {
        return lowerLeftTags->getTagsAsStringSet(tags);
    }
    std::set<std::string>& getLowerRightChosenTags(std::set<std::string>& tags) const {
        return lowerRightTags->getTagsAsStringSet(tags);
    }
    int getFilterBy() const {
        return this->filterByCombo->currentData().toInt();
    }
    int getSortBy() const {
        return this->sortByCombo->currentData().toInt();
    }
    std::string getOutlineScope() const {
        return oScopeOutline?oScopeOutline->getKey():"";
    }

    Organizer* getOrganizerToEdit() const {
        return this->organizerToEdit;
    }

    void show(
        const std::vector<Outline*>& outlines,
        const std::vector<const Tag*>* tags=nullptr,
        Organizer* organizerToEdit=nullptr,
        Outline* oScopeOutline=nullptr
    );

private:
    void toEisenhowerMatrixMode();
    void toKanbanMode();

signals:
    void createFinished();

private slots:
    void handleFindOutline();
    void handleFindOutlineChoice();
    void handleClearOutline();
    void handleChangeTypeCombo(const QString& text);

    void handleCreate();
};

}
#endif // M8RUI_ORGANIZER_NEW_DIALOG_H
