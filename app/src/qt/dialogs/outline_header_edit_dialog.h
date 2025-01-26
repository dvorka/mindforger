/*
 outline_header_edit_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_OUTLINE_HEADER_EDIT_DIALOG_H
#define M8RUI_OUTLINE_HEADER_EDIT_DIALOG_H

#include <QtWidgets>

#include "../../lib/src/mind/ontology/ontology_vocabulary.h"
#include "../../lib/src/mind/ontology/ontology.h"
#include "../../lib/src/config/repository.h"

#include "../model_meta_definitions.h"
#include "../gear/qutils.h"
#include "../widgets/edit_tags_panel.h"
#include "../widgets/labeled_edit_line_panel.h"
#include "../widgets/importance_combo_box.h"
#include "../widgets/urgency_combo_box.h"

namespace m8r {

class OutlineHeaderEditDialog : public QDialog
{
    Q_OBJECT

    class GeneralTab;
    class PreambleTab;
    class AdvancedTab;

private:
    Outline* currentOutline;
    Ontology& ontology;

    QTabWidget* tabWidget;
    PreambleTab* preambleTag;
    GeneralTab* generalTab;
    PreambleTab* preambleTab;
    AdvancedTab* advancedTab;

    QDialogButtonBox *buttonBox;

public:
    explicit OutlineHeaderEditDialog(Ontology& ontology, QWidget* parent);
    OutlineHeaderEditDialog(const OutlineHeaderEditDialog&) = delete;
    OutlineHeaderEditDialog(const OutlineHeaderEditDialog&&) = delete;
    OutlineHeaderEditDialog &operator=(const OutlineHeaderEditDialog&) = delete;
    OutlineHeaderEditDialog &operator=(const OutlineHeaderEditDialog&&) = delete;
    ~OutlineHeaderEditDialog();

    void setOutline(Outline* outline) { currentOutline = outline; }
    void toOutline();

    void show(Repository::RepositoryType repositoryType);

signals:
    void acceptedSignal();
    void rejectedSignal();

private slots:
    void handleAccepted();
    void handleRejected();
};

/**
 * @brief General tab of edit Outline dialog.
 */
class OutlineHeaderEditDialog::GeneralTab : public QWidget
{
    Q_OBJECT

    friend class OutlineHeaderEditDialog;

private:
    Ontology& ontology;

    QLabel* typeLabel;
    QComboBox* typeCombo;
    QLabel* importanceLabel;
    ImportanceComboBox* importanceCombo;
    QLabel* urgencyLabel;
    UrgencyComboBox* urgencyCombo;
    QLabel* progressLabel;
    QSpinBox* progressSpin;

    EditTagsPanel* editTagsGroup;

public:
    explicit GeneralTab(Ontology& ontology, QWidget *parent);
    ~GeneralTab();
};

/**
 * @brief Preamble tab of edit Outline dialog.
 */
class OutlineHeaderEditDialog::PreambleTab : public QWidget
{
    Q_OBJECT

    friend class OutlineHeaderEditDialog;

private:
    QLabel* preambleLabel;
    QTextEdit* preambleText;

public:
    explicit PreambleTab(QWidget* parent);
    ~PreambleTab();

    void refreshPreambleText(QString& t) { preambleText->setText(t); }
    QString getPreambleText() const { return preambleText->toPlainText(); }
};

/**
 * @brief Advanced tab of edit Outline dialog.
 */
class OutlineHeaderEditDialog::AdvancedTab : public QWidget
{
    Q_OBJECT

    friend class OutlineHeaderEditDialog;

private:
    QLabel* createdLabel;
    QLineEdit* createdLine;
    LabeledEditLinePanel* modifiedPanel;
    LabeledEditLinePanel* readPanel;
    LabeledEditLinePanel* readsPanel;
    LabeledEditLinePanel* writesPanel;
    QLineEdit* writesLine;
    QLabel* fileLabel;
    QLineEdit* fileLine;

public:
    explicit AdvancedTab(QWidget* parent);
    void refreshPath(const QString &path);
    ~AdvancedTab();
};

}

#endif // M8RUI_OUTLINE_HEADER_EDIT_DIALOG_H
