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

#include "lib/src/model/tag.h"
#include "lib/src/model/outline_type.h"
#include "lib/src/mind/ontology/ontology_vocabulary.h"
#include "lib/src/mind/ontology/ontology.h"
#include "lib/src/model/stencil.h"

#include "src/qt/model_meta_definitions.h"

class QDialogButtonBox;
class QTabWidget;

namespace m8r {

class OutlineNewDialog : public QDialog
{
    Q_OBJECT

    class GeneralTab : public QWidget
    {
    private:
        QLabel* nameLabel;
        QLineEdit* nameEdit;
        QLabel* typeLabel;
        QComboBox* typeCombo;
        QLabel* importanceLabel;
        QComboBox* importanceCombo;
        QLabel* urgencyLabel;
        QComboBox* urgencyCombo;
        QLabel* tagLabel;
        QComboBox* tagCombo;
        QLabel* stencilLabel;
        QComboBox* stencilCombo;

        // TODO progress

    public:
        explicit GeneralTab(QWidget *parent);
        ~GeneralTab(void);

        QLineEdit* getNameEdit(void) const { return nameEdit; }
        QComboBox* getTypeCombo(void) const { return typeCombo; }
        QComboBox* getImportanceCombo(void) const { return importanceCombo; }
        QComboBox* getUrgencyCombo(void) const { return urgencyCombo; }
        QComboBox* getTagCombo(void) const { return tagCombo; }
        QComboBox* getStencilCombo(void) const { return stencilCombo; }

        void clean(void);
    };

    // TODO tags tab

    class AdvancedTab : public QWidget
    {
    private:
        QLabel *pathLabel;
        QLabel *pathEdit;

    private:
        const QString memoryDirPath;

        // TODO read reads written writes

    public:
        explicit AdvancedTab(const QString& memoryDirPath, QWidget *parent);
        void refreshPath(const QString &);
        ~AdvancedTab(void);
    };

private:
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;

    GeneralTab* generalTab;
    AdvancedTab* advancedTab;

    Ontology& ontology;

public:
    explicit OutlineNewDialog(
            const QString& memoryDirPath,
            Ontology& ontology,
            std::vector<Stencil*>& stencils,
            QWidget* parent);
    OutlineNewDialog(const OutlineNewDialog&) = delete;
    OutlineNewDialog(const OutlineNewDialog&&) = delete;
    OutlineNewDialog &operator=(const OutlineNewDialog&) = delete;
    OutlineNewDialog &operator=(const OutlineNewDialog&&) = delete;
    ~OutlineNewDialog(void);

    QString getOutlineName(void) const { return generalTab->getNameEdit()->text(); }
    Stencil* getStencil(void) const { return generalTab->getStencilCombo()->itemData(generalTab->getStencilCombo()->currentIndex()).value<Stencil*>(); }
    const OutlineType* getOutlineType(void) const {
        return (const OutlineType*)(generalTab->getTypeCombo()->itemData(generalTab->getTypeCombo()->currentIndex(), Qt::UserRole).value<const OutlineType*>());
    }
    int8_t getImportance(void) const {
        return (int8_t)(generalTab->getImportanceCombo()->itemData(generalTab->getImportanceCombo()->currentIndex(), Qt::UserRole).value<int>());
    }
    int8_t getUrgency(void) const {
        return (int8_t)(generalTab->getUrgencyCombo()->itemData(generalTab->getUrgencyCombo()->currentIndex(), Qt::UserRole).value<int>());
    }
    const Tag* getTag(void) const {
        return (const Tag*)(generalTab->getTagCombo()->itemData(generalTab->getTagCombo()->currentIndex(), Qt::UserRole).value<const Tag*>());
    }

    void show();

private slots:
    void refreshPath(const QString &);
};

}
#endif // M8RUI_OUTLINE_NEW_DIALOG_H
