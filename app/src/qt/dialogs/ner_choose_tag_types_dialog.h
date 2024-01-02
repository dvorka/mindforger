/*
 ner_choose_tag_types_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_NER_CHOOSE_TAG_TYPES_DIALOG_H
#define M8RUI_NER_CHOOSE_TAG_TYPES_DIALOG_H

#include <QtWidgets>

#include "../../lib/src/gear/lang_utils.h"
#include "../../lib/src/debug.h"

namespace m8r {

class NerChooseTagTypesDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel* label;
    // IMPROVE tag types are hardcoded - allow loading them dynamically for generic NER models
    QCheckBox* personsCheckBox;
    QCheckBox* locationsCheckBox;
    QCheckBox* organizationsCheckBox;
    QCheckBox* miscCheckBox;
    QPushButton* chooseButton;
    QPushButton* closeButton;

public:
    explicit NerChooseTagTypesDialog(QWidget* parent);
    NerChooseTagTypesDialog(const NerChooseTagTypesDialog&) = delete;
    NerChooseTagTypesDialog(const NerChooseTagTypesDialog&&) = delete;
    NerChooseTagTypesDialog &operator=(const NerChooseTagTypesDialog&) = delete;
    NerChooseTagTypesDialog &operator=(const NerChooseTagTypesDialog&&) = delete;
    ~NerChooseTagTypesDialog();

    void clearCheckboxes() {
        personsCheckBox->setChecked(false);
        locationsCheckBox->setChecked(false);
        organizationsCheckBox->setChecked(false);
        miscCheckBox->setChecked(false);
    }

    QCheckBox* getPersonsCheckbox() const { return personsCheckBox; }
    QCheckBox* getLocationsCheckbox() const { return locationsCheckBox; }
    QCheckBox* getOrganizationsCheckbox() const { return organizationsCheckBox; }
    QCheckBox* getMiscCheckbox() const { return miscCheckBox; }

    QPushButton* getChooseButton() const { return chooseButton; }

    void show() { slotChooseButtonEnable(0); QDialog::show(); }

private slots:
    void slotChooseButtonEnable(int);
};

}
#endif // M8RUI_NER_CHOOSE_TAG_TYPES_DIALOG_H
