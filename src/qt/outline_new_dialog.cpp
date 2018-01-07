/*
 outline_new_dialog.cpp     MindForger thinking notebook

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
#include "outline_new_dialog.h"

using namespace std;

namespace m8r {

OutlineNewDialog::GeneralTab::GeneralTab(QWidget *parent)
    : QWidget(parent)
{
    QString s{};

    this->nameLabel = new QLabel(tr("Name")+":", this),
    this->nameEdit = new QLineEdit(tr("Outline"), this);

    this->typeLabel = new QLabel(tr("Type")+":", this);
    this->typeCombo = new QComboBox(this);

    this->importanceLabel = new QLabel(tr("Importance")+":", this);
    this->importanceCombo = new QComboBox(this);
    this->importanceCombo->addItem("");
    for(int i=1; i<=5; i++) {
        s.clear();
        for(int j=1; j<=5; j++) {
            if(j<=i) {
                s += QChar(9733);
            } else {
                s += QChar(9734);
            }
        }
        importanceCombo->addItem(s, QVariant::fromValue<int>(i));
    }

    this->urgencyLabel = new QLabel(tr("Urgency")+":", this);
    this->urgencyCombo = new QComboBox(this);
    this->urgencyCombo->addItem("");
    for(int i=1; i<=5; i++) {
        s.clear();
        for(int j=1; j<=5; j++) {
            if(j<=i) {
                s += QChar(0x29D7);
            } else {
                s += QChar(0x29D6);
            }
        }
        urgencyCombo->addItem(s, QVariant::fromValue<int>(i));
    }

    this->tagLabel = new QLabel(tr("Tag")+":", this);
    this->tagCombo = new QComboBox(this);

    this->stencilLabel = new QLabel(tr("Stencil")+":", this);
    this->stencilCombo = new QComboBox(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit);
    mainLayout->addWidget(typeLabel);
    mainLayout->addWidget(typeCombo);
    mainLayout->addWidget(importanceLabel);
    mainLayout->addWidget(importanceCombo);
    mainLayout->addWidget(urgencyLabel);
    mainLayout->addWidget(urgencyCombo);
    mainLayout->addWidget(tagLabel);
    mainLayout->addWidget(tagCombo);
    mainLayout->addWidget(stencilLabel);
    mainLayout->addWidget(stencilCombo);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void OutlineNewDialog::GeneralTab::clean(void)
{
    nameEdit->selectAll();
    nameEdit->setFocus();
}

OutlineNewDialog::GeneralTab::~GeneralTab(void)
{
    delete nameLabel;
    delete nameEdit;
    delete typeLabel;
    delete typeCombo;
    delete tagLabel;
    delete tagCombo;
    delete stencilLabel;
    delete stencilCombo;
}

OutlineNewDialog::AdvancedTab::AdvancedTab(const QString& memoryDirPath, QWidget *parent)
    : QWidget(parent), memoryDirPath(memoryDirPath)
{
    pathLabel = new QLabel(tr("File path:"));
    pathEdit = new QLabel(this->memoryDirPath);
    pathEdit->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(pathLabel);
    mainLayout->addWidget(pathEdit);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void OutlineNewDialog::AdvancedTab::refreshPath(const QString& name)
{
    pathEdit->setText(memoryDirPath+FILE_PATH_SEPARATOR+name+FILE_EXTENSION_MARKDOWN);
}

OutlineNewDialog::AdvancedTab::~AdvancedTab(void)
{
}

OutlineNewDialog::OutlineNewDialog(
        const QString& memoryDirPath,
        // IMPROVE Does model belong to View? Set values from presenter & keep model classes in presenter only?
        Ontology& ontology,
        vector<Stencil*>& stencils,
        QWidget *parent)
    : QDialog(parent), ontology(ontology)
{
    tabWidget = new QTabWidget;

    generalTab = new GeneralTab(this);
    if(ontology.getOutlineTypes().size()) {
        QComboBox* combo=generalTab->getTypeCombo();
        for(const OutlineType* t:ontology.getOutlineTypes().values()) {
            combo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<const OutlineType*>(t));
        }
        combo->setCurrentText(QString::fromStdString(ontology.getDefaultOutlineType()->getName()));
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
    if(stencils.size()) {
        QComboBox* combo=generalTab->getStencilCombo();
        combo->addItem("", QVariant::fromValue<const Stencil*>(nullptr));
        for(Stencil* t:stencils) {
            if(t->getType()==ResourceType::OUTLINE) {
                combo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<Stencil*>(t));
            }
        }
        combo->setCurrentText("");
    }
    tabWidget->addTab(generalTab, tr("General"));

    advancedTab = new AdvancedTab(memoryDirPath, this);
    tabWidget->addTab(advancedTab, tr("Advanced"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // wire signals ensuring that close & set dialog status
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(generalTab->getNameEdit(), SIGNAL(textChanged(const QString &)), this, SLOT(refreshPath(const QString &)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("New Outline Dialog"));
}

void OutlineNewDialog::show()
{
    generalTab->clean();
    QDialog::show();
}

void OutlineNewDialog::refreshPath(const QString& path)
{
    advancedTab->refreshPath(path);
}

OutlineNewDialog::~OutlineNewDialog(void)
{
    if(generalTab) delete generalTab;
}

}
