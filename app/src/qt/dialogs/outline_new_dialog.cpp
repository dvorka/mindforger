/*
 outline_new_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "outline_new_dialog.h"

using namespace std;
using namespace m8r::filesystem;

namespace m8r {

/*
 * General tab
 */

OutlineNewDialog::GeneralTab::GeneralTab(Ontology& ontology, QWidget *parent)
    : QWidget(parent), ontology(ontology)
{
    QGroupBox* basicGroup = new QGroupBox{tr("Basic"), this};

    nameLabel = new QLabel(tr("Name")+":", this);
    nameEdit = new QLineEdit(tr("Notebook"), this);

    typeLabel = new QLabel(tr("Type")+":", this);
    typeCombo = new QComboBox{this};

    importanceLabel = new QLabel(tr("Importance")+":", this);
    importanceCombo = new ImportanceComboBox{this};

    urgencyLabel = new QLabel(tr("Urgency")+":", this);
    urgencyCombo = new UrgencyComboBox{this};

    progressLabel = new QLabel{tr("Progress")+": %", this};
    progressSpin = new QSpinBox{this};
    progressSpin->setMinimum(0);
    progressSpin->setMaximum(100);

    stencilLabel = new QLabel(tr("Stencil")+":", this);
    stencilCombo = new QComboBox(this);

    editTagsGroup = new EditTagsPanel{MfWidgetMode::CREATE_MODE, ontology, this};
    editTagsGroup->refreshOntologyTags();

    // assembly
    QVBoxLayout* basicLayout = new QVBoxLayout{this};
    basicLayout->addWidget(nameLabel);
    basicLayout->addWidget(nameEdit);
    basicLayout->addWidget(typeLabel);
    basicLayout->addWidget(typeCombo);
    QWidget* wiu = new QWidget{this};
    QHBoxLayout* liu = new QHBoxLayout(wiu);
    liu->setContentsMargins(0,0,0,0);
    QWidget* w = new QWidget{this};
    QVBoxLayout* l = new QVBoxLayout(w);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(importanceLabel);
    l->addWidget(importanceCombo);
    w->setLayout(l);
    liu->addWidget(w);
    w = new QWidget{this};
    l = new QVBoxLayout(w);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(urgencyLabel);
    l->addWidget(urgencyCombo);
    w->setLayout(l);
    liu->addWidget(w);
    basicLayout->addWidget(wiu);
    basicLayout->addWidget(progressLabel);
    basicLayout->addWidget(progressSpin);
    basicLayout->addWidget(stencilLabel);
    basicLayout->addWidget(stencilCombo);
    basicGroup->setLayout(basicLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(basicGroup);
    boxesLayout->addWidget(editTagsGroup);
    setLayout(boxesLayout);
}

OutlineNewDialog::GeneralTab::~GeneralTab()
{
    delete nameLabel;
    delete nameEdit;
    delete typeLabel;
    delete typeCombo;
    delete importanceLabel;
    delete importanceCombo;
    delete urgencyLabel;
    delete urgencyCombo;
    delete progressLabel;
    delete progressSpin;
    delete stencilLabel;
    delete stencilCombo;
}

void OutlineNewDialog::GeneralTab::clean()
{
    nameEdit->selectAll();
    nameEdit->setFocus();
    stencilCombo->clear();
    editTagsGroup->clearTagList();
}

void OutlineNewDialog::GeneralTab::showFacet(Repository::RepositoryType repositoryType)
{
    bool visibility =
        Repository::RepositoryType::MINDFORGER == repositoryType
        ? true
        : false;

    editTagsGroup->setVisible(visibility);
    progressLabel->setVisible(visibility);
    progressSpin->setVisible(visibility);
    stencilLabel->setVisible(visibility);
    stencilCombo->setVisible(visibility);
    typeLabel->setVisible(visibility);
    typeCombo->setVisible(visibility);
    importanceLabel->setVisible(visibility);
    importanceCombo->setVisible(visibility);
    urgencyLabel->setVisible(visibility);
    urgencyCombo->setVisible(visibility);
    progressLabel->setVisible(visibility);
    progressSpin->setVisible(visibility);
}

/*
 * Preamble tab
 */

OutlineNewDialog::PreambleTab::PreambleTab(QWidget *parent)
    : QWidget(parent)
{
    preambleLabel = new QLabel{tr("Text")+":", this};
    preambleText = new QTextEdit{this};

    QGroupBox* preambleGroup = new QGroupBox{tr("Preamble"), this};
    QVBoxLayout* locationLayout = new QVBoxLayout{this};
    locationLayout->addWidget(preambleLabel);
    locationLayout->addWidget(preambleText);
    preambleGroup->setLayout(locationLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(preambleGroup);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

OutlineNewDialog::PreambleTab::~PreambleTab()
{
}

/*
 * Advanced tab
 */

OutlineNewDialog::AdvancedTab::AdvancedTab(const QString& memoryDirPath, QWidget *parent)
    : QWidget(parent), memoryDirPath(memoryDirPath)
{
    fileLabel = new QLabel{tr("Expected file name")+":", this};
    fileLine = new QLineEdit{this->memoryDirPath, this};
    fileLine->setDisabled(true);

    QGroupBox* locationGroup = new QGroupBox{tr("Location"), this};
    QVBoxLayout* locationLayout = new QVBoxLayout{this};
    locationLayout->addWidget(fileLabel);
    locationLayout->addWidget(fileLine);
    locationGroup->setLayout(locationLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(locationGroup);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

OutlineNewDialog::AdvancedTab::~AdvancedTab()
{
}

void OutlineNewDialog::AdvancedTab::refreshPath(const QString& name)
{
    fileLine->setText(
        memoryDirPath
        + FILE_PATH_SEPARATOR
        + QString::fromStdString(
            normalizeToNcName(name.toStdString(),'-')
            + File::EXTENSION_MD_MD
        )
    );
}

/*
 * Dialog
 */

OutlineNewDialog::OutlineNewDialog(
        const QString& memoryDirPath,
        // IMPROVE Does model belong to View? Set values from presenter & keep model classes in presenter only?
        Ontology& ontology,
        QWidget *parent)
    : QDialog(parent), ontology(ontology)
{
    tabWidget = new QTabWidget;

    generalTab = new GeneralTab{ontology, this};
    if(ontology.getOutlineTypes().size()) {
        QComboBox* combo=generalTab->getTypeCombo();
        for(const OutlineType* t:ontology.getOutlineTypes().values()) {
            combo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<const OutlineType*>(t));
        }
        combo->setCurrentText(QString::fromStdString(ontology.getDefaultOutlineType()->getName()));
    }

    // stencils may be added/removed/changed therefore they are updated anytime dialog is shown
    tabWidget->addTab(generalTab, tr("General"));

    preambleTab = new PreambleTab(this);
    tabWidget->addTab(preambleTab, tr("Preamble"));

    advancedTab = new AdvancedTab(memoryDirPath, this);
    tabWidget->addTab(advancedTab, tr("Advanced"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // signals
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QObject::connect(generalTab->getNameEdit(), SIGNAL(textChanged(const QString&)), this, SLOT(refreshPath(const QString&)));

    QVBoxLayout *mainLayout = new QVBoxLayout{this};
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("New Notebook"));
    resize(fontMetrics().averageCharWidth()*55, fontMetrics().height()*30);
    setModal(true);
}

OutlineNewDialog::~OutlineNewDialog()
{
    if(generalTab) delete generalTab;
    if(preambleTab) delete preambleTab;
    if(advancedTab) delete advancedTab;
}

QString OutlineNewDialog::getOutlineName() const
{
    return generalTab->getNameEdit()->text();
}

Stencil* OutlineNewDialog::getStencil() const
{
    return generalTab->getStencilCombo()->itemData(generalTab->getStencilCombo()->currentIndex()).value<Stencil*>();
}

QString OutlineNewDialog::getPreamble() const
{
    return preambleTab->getPreambleText();
}

const OutlineType* OutlineNewDialog::getOutlineType() const
{
    return static_cast<const OutlineType*>(generalTab->getTypeCombo()->itemData(
        generalTab->getTypeCombo()->currentIndex(),
        Qt::UserRole
    ).value<const OutlineType*>());
}

int8_t OutlineNewDialog::getImportance() const
{
    return static_cast<int8_t>(generalTab->getImportanceCombo()->itemData(
        generalTab->getImportanceCombo()->currentIndex(),
        Qt::UserRole
    ).value<int>());
}

int8_t OutlineNewDialog::getUrgency() const
{
    return static_cast<int8_t>(generalTab->getUrgencyCombo()->itemData(
        generalTab->getUrgencyCombo()->currentIndex(),
        Qt::UserRole).value<int>()
    );
}

int OutlineNewDialog::getProgress() const
{
    return generalTab->getProgressSpin()->value();
}

const std::vector<const Tag*>& OutlineNewDialog::getTags() const
{
    return generalTab->getTags();
}

void OutlineNewDialog::show(
    vector<Stencil*>& stencils,
    Repository::RepositoryType repositoryType
) {
    generalTab->clean();

    // IMPROVE reload stencils only if dirty
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

    refreshPath(generalTab->getNameEdit()->text());

    // widgets visibility: Markdown vs. MindForger (repository) mode
    generalTab->showFacet(repositoryType);

    QDialog::show();
}

void OutlineNewDialog::refreshPath(const QString& path)
{
    advancedTab->refreshPath(path);
}

} // m8r namespace
