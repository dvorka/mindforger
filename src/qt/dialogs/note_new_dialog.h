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

#include "src/qt/model_meta_definitions.h"

class QDialogButtonBox;
class QTabWidget;

namespace m8r {

class NoteNewDialog : public QDialog
{
    Q_OBJECT

    class GeneralTab : public QWidget
    {

    private:
        QVBoxLayout *mainLayout;

        QLabel* nameLabel;
        QLineEdit* nameEdit;
        QLabel* typeLabel;
        QComboBox* typeCombo;
        QLabel* tagLabel;
        QComboBox* tagCombo;
        QLabel* positionLabel;
        QComboBox* positionCombo;
        QLabel* stencilLabel;
        QComboBox* stencilCombo;

    public:
        explicit GeneralTab(QWidget* parent);
        ~GeneralTab(void);

        QLineEdit* getNameEdit(void) const { return nameEdit; }
        QComboBox* getTypeCombo(void) const { return typeCombo; }
        QComboBox* getTagCombo(void) const { return tagCombo; }
        QComboBox* getPositionCombo(void) const { return positionCombo; }
        QComboBox* getStencilCombo(void) const { return stencilCombo; }

        void clean(void);
    };

private:
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;

    GeneralTab* generalTab;

    Ontology& ontology;

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

    QString getNoteName(void) const { return generalTab->getNameEdit()->text(); }
    Stencil* getStencil(void) const { return generalTab->getStencilCombo()->itemData(generalTab->getStencilCombo()->currentIndex()).value<Stencil*>(); }
    const NoteType* getNoteType(void) const {
        return (const NoteType*)(generalTab->getTypeCombo()->itemData(generalTab->getTypeCombo()->currentIndex(), Qt::UserRole).value<const NoteType*>());
    }
    const Tag* getTag(void) const {
        return (const Tag*)(generalTab->getTagCombo()->itemData(generalTab->getTagCombo()->currentIndex(), Qt::UserRole).value<const Tag*>());
    }

    void show();
};

}
#endif // M8RUI_NOTE_NEW_DIALOG_H
