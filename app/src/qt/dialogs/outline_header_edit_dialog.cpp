/*
 outline_header_edit_dialog.cpp     MindForger thinking notebook

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
#include "outline_header_edit_dialog.h"

namespace m8r {

using namespace std;

/*
 * General tab
 */

OutlineHeaderEditDialog::GeneralTab::GeneralTab(Ontology& ontology, QWidget *parent)
    : QWidget(parent), ontology(ontology)
{
    QGroupBox* basicGroup = new QGroupBox{tr("Basic"), this};

    typeLabel = new QLabel{tr("Type")+":", this};
    typeCombo = new QComboBox{this};
    if(ontology.getOutlineTypes().size()) {
        for(const OutlineType* t:ontology.getOutlineTypes().values()) {
            typeCombo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<const OutlineType*>(t));
        }
        typeCombo->setCurrentText(QString::fromStdString(ontology.getDefaultOutlineType()->getName()));
    }

    importanceLabel = new QLabel(tr("Importance")+":", this);
    importanceCombo = new ImportanceComboBox{this};

    urgencyLabel = new QLabel(tr("Urgency")+":", this);
    urgencyCombo = new UrgencyComboBox{this};

    progressLabel = new QLabel{tr("Progress")+": %", this};
    progressSpin = new QSpinBox{this};
    progressSpin->setMinimum(0);
    progressSpin->setMaximum(100);

    editTagsGroup = new EditTagsPanel{MfWidgetMode::EDIT_MODE, ontology, this};

    // assembly
    QVBoxLayout* basicLayout = new QVBoxLayout{this};
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
    basicGroup->setLayout(basicLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(basicGroup);
    boxesLayout->addWidget(editTagsGroup);
    setLayout(boxesLayout);
}

OutlineHeaderEditDialog::GeneralTab::~GeneralTab()
{
    delete typeLabel;
    delete typeCombo;
    delete importanceLabel;
    delete importanceCombo;
    delete urgencyLabel;
    delete urgencyCombo;
    delete progressLabel;
    delete progressSpin;
}

/*
 * Preamble tab
 */

OutlineHeaderEditDialog::PreambleTab::PreambleTab(QWidget *parent)
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

OutlineHeaderEditDialog::PreambleTab::~PreambleTab()
{
}

/*
 * Advanced tab
 */

OutlineHeaderEditDialog::AdvancedTab::AdvancedTab(QWidget* parent)
    : QWidget(parent)
{
    QGroupBox* metadataGroup = new QGroupBox{tr("Metadata"), this};

    createdLabel = new QLabel{tr("Created")+":", this};
    createdLine = new QLineEdit{this};
    createdLine->setDisabled(true);
    modifiedPanel = new LabeledEditLinePanel{tr("Last Modified")+":", this};
    modifiedPanel->setEnabled(false);
    readPanel = new LabeledEditLinePanel{tr("Last Read")+":", this};
    readPanel->setEnabled(false);
    readsPanel = new LabeledEditLinePanel{tr("Reads")+":", this};
    readsPanel->setDisabled(true);
    writesPanel = new LabeledEditLinePanel{tr("Writes")+":", this};
    writesPanel->setDisabled(true);
    fileLabel = new QLabel{tr("File")+":", this};
    fileLine = new QLineEdit{this};
    fileLine->setDisabled(true);

    QVBoxLayout* metadataLayout = new QVBoxLayout{this};
    metadataLayout->addWidget(createdLabel);
    metadataLayout->addWidget(createdLine);
    QWidget* w = new QWidget{this};
    QHBoxLayout* l = new QHBoxLayout(w);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(modifiedPanel);
    l->addWidget(readPanel);
    w->setLayout(l);
    metadataLayout->addWidget(w);
    w = new QWidget{this};
    l = new QHBoxLayout(w);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(readsPanel);
    l->addWidget(writesPanel);
    w->setLayout(l);
    metadataLayout->addWidget(w);
    metadataGroup->setLayout(metadataLayout);

    QGroupBox* locationGroup = new QGroupBox{tr("Location"), this};
    QVBoxLayout* locationLayout = new QVBoxLayout{this};
    locationLayout->addWidget(fileLabel);
    locationLayout->addWidget(fileLine);
    locationGroup->setLayout(locationLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(metadataGroup);
    boxesLayout->addWidget(locationGroup);
    boxesLayout->addStretch();
    setLayout(boxesLayout);
}

void OutlineHeaderEditDialog::AdvancedTab::refreshPath(const QString& path)
{
    fileLine->setText(path);
}

OutlineHeaderEditDialog::AdvancedTab::~AdvancedTab()
{
}

/*
 * Dialog
 */

OutlineHeaderEditDialog::OutlineHeaderEditDialog(Ontology& ontology, QWidget *parent)
    : QDialog{parent}, ontology(ontology)
{
    tabWidget = new QTabWidget;

    generalTab = new GeneralTab(ontology, this);
    tabWidget->addTab(generalTab, tr("General"));

    preambleTab = new PreambleTab(this);
    tabWidget->addTab(preambleTab, tr("Preamble"));

    advancedTab = new AdvancedTab{this};
    tabWidget->addTab(advancedTab, tr("Advanced"));

    buttonBox = new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel};

    // wire signals ensuring that close & set dialog status
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    // IMPROVE extra wiring below to be removed - I was unable to connect QDialog::accept from outside :-/
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &OutlineHeaderEditDialog::handleAccepted);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &OutlineHeaderEditDialog::handleRejected);
    //QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, [=](){ MF_DEBUG("NoteEditDialog OK" << endl); });

    QVBoxLayout *mainLayout = new QVBoxLayout{this};
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Edit Notebook"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

OutlineHeaderEditDialog::~OutlineHeaderEditDialog()
{
    if(generalTab) delete generalTab;
    if(preambleTab) delete preambleTab;
    if(advancedTab) delete advancedTab;
}

void OutlineHeaderEditDialog::show()
{
    if(currentOutline) {
        // RDWR
        if(currentOutline->getType()) {
            int i = generalTab->typeCombo->findData(QVariant::fromValue<const OutlineType*>(currentOutline->getType()));
            if(i>=0) {
                generalTab->typeCombo->setCurrentIndex(i);
            } else {
                MF_DEBUG("Unknown Outline type: " << currentOutline->getType()->getName() << endl);
            }
        }
        generalTab->importanceCombo->setCurrentIndex(currentOutline->getImportance());
        generalTab->urgencyCombo->setCurrentIndex(currentOutline->getUrgency());
        generalTab->progressSpin->setValue(currentOutline->getProgress());
        generalTab->editTagsGroup->refresh(currentOutline->getTags());

        preambleTab->preambleText->setText(QString::fromStdString(currentOutline->getPreambleAsString()));

        // RDONLY
        advancedTab->createdLine->setText(QString::fromStdString(datetimeToString(currentOutline->getCreated())));
        advancedTab->modifiedPanel->setText(QString::fromStdString(datetimeToString(currentOutline->getModified())));
        advancedTab->readPanel->setText(QString::fromStdString(datetimeToString(currentOutline->getRead())));
        advancedTab->readsPanel->setText(QString::number(currentOutline->getReads()));
        advancedTab->writesPanel->setText(QString::number(currentOutline->getRevision()));
        advancedTab->fileLine->setText(QString::fromStdString(currentOutline->getKey()));
    }

    generalTab->editTagsGroup->setFocusAddingTag();

    QDialog::show();
}

void OutlineHeaderEditDialog::toOutline()
{
    if(currentOutline) {
        if(generalTab->typeCombo->currentIndex() != -1) {
            currentOutline->setType((const OutlineType*)(generalTab->typeCombo->itemData(generalTab->typeCombo->currentIndex(), Qt::UserRole).value<const OutlineType*>()));
        }
        currentOutline->setImportance(generalTab->importanceCombo->currentIndex());
        currentOutline->setUrgency(generalTab->urgencyCombo->currentIndex());
        currentOutline->setProgress(generalTab->progressSpin->value());
        currentOutline->setTags(&generalTab->editTagsGroup->getTags());

        // preamble
        std::vector<std::string*> preamble;
        if(preambleTab->getPreambleText().size()) {
            std::string* preambleText = new std::string{preambleTab->getPreambleText().toStdString()};
            stringToLines(preambleText, preamble);
            delete preambleText;
        }
        currentOutline->setPreamble(preamble);
    } else {
        MF_DEBUG("Attempt to save data from dialog to Outline, but no Outline is set." << endl);
    }
}

void OutlineHeaderEditDialog::handleAccepted()
{
    emit acceptedSignal();
}

void OutlineHeaderEditDialog::handleRejected()
{
    emit rejectedSignal();
}

}
