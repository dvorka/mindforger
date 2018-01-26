/*
 note_new_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_NOTE_NEW_DIALOG_H
#define M8RUI_NOTE_NEW_DIALOG_H

#include <QtWidgets>

#include "lib/src/model/tag.h"
#include "lib/src/model/note_type.h"
#include "lib/src/mind/ontology/ontology_vocabulary.h"
#include "lib/src/mind/ontology/ontology.h"
#include "lib/src/model/stencil.h"

#include "../model_meta_definitions.h"
#include "../widgets/edit_tags_panel.h"

class QDialogButtonBox;
class QTabWidget;

namespace m8r {

class NoteNewDialog : public QDialog
{
    Q_OBJECT

    class GeneralTab;
    class AdvancedTab;

private:
    Ontology& ontology;

    GeneralTab* generalTab;
    AdvancedTab* advancedTab;

    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;

public:
    explicit NoteNewDialog(
            Ontology& ontology,
            std::vector<Stencil*>& stencils,
            QWidget* parent);
    NoteNewDialog(const NoteNewDialog&) = delete;
    NoteNewDialog(const NoteNewDialog&&) = delete;
    NoteNewDialog &operator=(const NoteNewDialog&) = delete;
    NoteNewDialog &operator=(const NoteNewDialog&&) = delete;
    ~NoteNewDialog(void);

    QString getNoteName() const;
    Stencil* getStencil() const;
    const NoteType* getNoteType() const;
    const std::vector<const Tag*>* getTags() const;
    int getProgress() const;

    void show(const QString& path);
};

/**
 * @brief General tab of new Note dialog.
 */
class NoteNewDialog::GeneralTab : public QWidget
{
    Q_OBJECT

private:
    Ontology& ontology;

    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QLabel* typeLabel;
    QComboBox* typeCombo;
    QLabel* progressLabel;
    QSpinBox* progressSpin;
    QLabel* stencilLabel;
    QComboBox* stencilCombo;
    QLabel* positionLabel;
    QComboBox* positionCombo;

    EditTagsPanel* editTagsGroup;

public:
    explicit GeneralTab(Ontology& ontology, QWidget* parent);
    ~GeneralTab(void);

    QLineEdit* getNameEdit() const { return nameEdit; }
    QComboBox* getTypeCombo() const { return typeCombo; }
    const std::vector<const Tag*>* getTags() { return editTagsGroup->getTags(); }
    QSpinBox* getProgressSpin() const { return progressSpin; }
    QComboBox* getStencilCombo() const { return stencilCombo; }
    QComboBox* getPositionCombo() const { return positionCombo; }

    void clean();
};

/**
 * @brief Advanced tab of new Note dialog.
 */
class NoteNewDialog::AdvancedTab : public QWidget
{
    Q_OBJECT

private:
    QLabel *pathLabel;
    QLabel *pathEdit;

public:
    explicit AdvancedTab(QWidget *parent);
    void refreshPath(const QString &path);
    ~AdvancedTab();
};

}
#endif // M8RUI_NOTE_NEW_DIALOG_H
