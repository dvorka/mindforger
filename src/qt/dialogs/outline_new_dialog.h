/*
 outline_new_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_NEW_DIALOG_H
#define M8RUI_OUTLINE_NEW_DIALOG_H

#include <QtWidgets>

#include "../../../lib/src/model/tag.h"
#include "../../../lib/src/model/outline_type.h"
#include "../../../lib/src/mind/ontology/ontology_vocabulary.h"
#include "../../../lib/src/mind/ontology/ontology.h"
#include "../../../lib/src/model/stencil.h"
#include "../../../lib/src/gear/string_utils.h"

#include "../model_meta_definitions.h"
#include "../widgets/edit_tags_panel.h"
#include "../widgets/importance_combo_box.h"
#include "../widgets/urgency_combo_box.h"

class QDialogButtonBox;
class QTabWidget;

namespace m8r {

class OutlineNewDialog : public QDialog
{
    Q_OBJECT

    class GeneralTab;
    class PreambleTab;
    class AdvancedTab;

private:
    Ontology& ontology;

    QTabWidget* tabWidget;
    GeneralTab* generalTab;
    PreambleTab* preambleTab;
    AdvancedTab* advancedTab;

    QDialogButtonBox *buttonBox;

public:
    explicit OutlineNewDialog(
            const QString& memoryDirPath,
            Ontology& ontology,
            QWidget* parent);
    OutlineNewDialog(const OutlineNewDialog&) = delete;
    OutlineNewDialog(const OutlineNewDialog&&) = delete;
    OutlineNewDialog &operator=(const OutlineNewDialog&) = delete;
    OutlineNewDialog &operator=(const OutlineNewDialog&&) = delete;
    ~OutlineNewDialog();

    QString getOutlineName() const;
    Stencil* getStencil() const;
    const OutlineType* getOutlineType() const;
    int8_t getImportance() const;
    int8_t getUrgency() const;
    int getProgress() const;
    const std::vector<const Tag*>* getTags() const;

    void show(std::vector<Stencil*>& stencils);

private slots:
    void refreshPath(const QString &);
};

/**
 * @brief General tab of new Outline dialog.
 */
class OutlineNewDialog::GeneralTab : public QWidget
{
    Q_OBJECT

private:
    Ontology& ontology;

    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QLabel* typeLabel;
    QComboBox* typeCombo;
    QLabel* importanceLabel;
    ImportanceComboBox* importanceCombo;
    QLabel* urgencyLabel;
    UrgencyComboBox* urgencyCombo;
    QLabel* progressLabel;
    QSpinBox* progressSpin;
    QLabel* stencilLabel;
    QComboBox* stencilCombo;

    EditTagsPanel* editTagsGroup;

public:
    explicit GeneralTab(Ontology& ontology, QWidget *parent);
    ~GeneralTab();

    QLineEdit* getNameEdit() const { return nameEdit; }
    QComboBox* getTypeCombo() const { return typeCombo; }
    ImportanceComboBox* getImportanceCombo() const { return importanceCombo; }
    UrgencyComboBox* getUrgencyCombo() const { return urgencyCombo; }
    QComboBox* getStencilCombo() const { return stencilCombo; }
    QSpinBox* getProgressSpin() const { return progressSpin; }
    const std::vector<const Tag*>* getTags() { return editTagsGroup->getTags(); }

    void clean();
};

/**
 * @brief Preamble tab of new Outline dialog.
 */
class OutlineNewDialog::PreambleTab : public QWidget
{
    Q_OBJECT

private:
    QLabel *fileLabel;
    QLineEdit *fileLine;

public:
    explicit PreambleTab(QWidget *parent) {}
    ~PreambleTab() {}
};

/**
 * @brief Advanced tab of new Outline dialog.
 */
class OutlineNewDialog::AdvancedTab : public QWidget
{
    Q_OBJECT

private:
    QLabel *fileLabel;
    QLineEdit *fileLine;

private:
    const QString memoryDirPath;

public:
    explicit AdvancedTab(const QString& memoryDirPath, QWidget *parent);
    void refreshPath(const QString &name);
    ~AdvancedTab();
};

}
#endif // M8RUI_OUTLINE_NEW_DIALOG_H
