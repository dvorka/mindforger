/*
 note_edit_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "note_edit_dialog.h"

using namespace std;

namespace m8r {

NoteEditDialog::GeneralTab::GeneralTab(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox* basicGroup = new QGroupBox{tr("Basic"), this};

    this->typeLabel = new QLabel(tr("Type")+":", this);
    this->typeCombo = new QComboBox(this);

    this->tagLabel = new QLabel(tr("Tag")+":", this);
    this->tagCombo = new QComboBox(this);

    this->progressLabel = new QLabel(tr("Progress")+":", this);
    this->progressSpin = new QSpinBox(this);
    progressSpin->setMinimum(0);
    progressSpin->setMaximum(100);

    QGroupBox* advancedGroup = new QGroupBox{tr("Advanced"), this};

    // assembly
    mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(basicGroup);
    mainLayout->addWidget(advancedGroup);
    QVBoxLayout* basicLayout = new QVBoxLayout{};
    basicLayout->addWidget(typeLabel);
    basicLayout->addWidget(typeCombo);
    basicLayout->addWidget(tagLabel);
    basicLayout->addWidget(tagCombo);
    basicGroup->setLayout(basicLayout);
    QVBoxLayout* advancedLayout = new QVBoxLayout{};
    advancedLayout->addWidget(progressLabel);
    advancedLayout->addWidget(progressSpin);
    advancedLayout->addStretch(1);
    advancedGroup->setLayout(advancedLayout);

    setLayout(mainLayout);
}

void NoteEditDialog::GeneralTab::clean(void)
{
    typeCombo->setFocus();
}

NoteEditDialog::GeneralTab::~GeneralTab(void)
{
    delete typeLabel;
    delete typeCombo;
    delete tagLabel;
    delete tagCombo;

    delete mainLayout;
}

NoteEditDialog::NoteEditDialog(
        Ontology& ontology,
        QWidget* parent)
    : QDialog(parent), ontology(ontology)
{
    tabWidget = new QTabWidget;

    generalTab = new GeneralTab(this);
    if(ontology.getNoteTypes().size()) {
        QComboBox* combo=generalTab->getTypeCombo();
        for(const NoteType* t:ontology.getNoteTypes().values()) {
            combo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<const NoteType*>(t));
        }
        combo->setCurrentText(QString::fromStdString(ontology.getDefaultNoteType()->getName()));
    }
    if(ontology.getTags().size()) {
        QComboBox* combo=generalTab->getTagCombo();
        combo->addItem("", QVariant::fromValue<const Tag*>(nullptr));
        for(const Tag* t:ontology.getTags().values()) {
            if(!stringistring(string("none"), t->getName())) {
                combo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<const Tag*>(t));
            }
        }
        combo->setCurrentText("");
    }
    tabWidget->addTab(generalTab, tr("General"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // wire signals ensuring that close & set dialog status
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Edit Note Properties"));
    setModal(true);
}

void NoteEditDialog::show()
{
    generalTab->clean();
    QDialog::show();
}

NoteEditDialog::~NoteEditDialog()
{
    if(generalTab) delete generalTab;
}

}
