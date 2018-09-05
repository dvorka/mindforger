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
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "note_edit_dialog.h"

using namespace std;

namespace m8r {

/*
 * General tab
 */

NoteEditDialog::GeneralTab::GeneralTab(Ontology& ontology, QWidget *parent)
    : QWidget(parent), ontology(ontology)
{
    QGroupBox* basicGroup = new QGroupBox{tr("Basic"), this};

    typeLabel = new QLabel{tr("Type")+":", this};
    typeCombo = new QComboBox{this};
    if(ontology.getNoteTypes().size()) {
        for(const NoteType* t:ontology.getNoteTypes().values()) {
            typeCombo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<const NoteType*>(t));
        }
        typeCombo->setCurrentText(QString::fromStdString(ontology.getDefaultNoteType()->getName()));
    }

    progressLabel = new QLabel{tr("Progress")+": %", this};
    progressSpin = new QSpinBox{this};
    progressSpin->setMinimum(0);
    progressSpin->setMaximum(100);

    deadlineCheck = new QCheckBox{tr("Deadline")+":", this};
    deadlineEdit = new QDateEdit{QDate::currentDate(), this};

    parentRelLabel = new QLabel{tr("Parent-child Relationship")+":", this};
    parentRelCombo = new QComboBox(this);
    parentRelCombo->addItem(tr("Composition")+" "+QChar(9670));
    parentRelCombo->addItem(tr("Aggregation")+" "+QChar(9671));
    parentRelCombo->addItem(tr("Is-a")+" "+QChar(9651));

    editTagsGroup = new EditTagsPanel{ontology, this};

    // signals
    QObject::connect(deadlineCheck, SIGNAL(stateChanged(int)), this, SLOT(handleDeadlineCheck(int)));

    // assembly
    QVBoxLayout* basicLayout = new QVBoxLayout{this};
    basicLayout->addWidget(typeLabel);
    basicLayout->addWidget(typeCombo);
    basicLayout->addWidget(progressLabel);
    basicLayout->addWidget(progressSpin);
    basicLayout->addWidget(deadlineCheck);
    basicLayout->addWidget(deadlineEdit);
    basicLayout->addWidget(parentRelLabel);
    basicLayout->addWidget(parentRelCombo);
    basicGroup->setLayout(basicLayout);

    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(basicGroup);
    boxesLayout->addWidget(editTagsGroup);
    setLayout(boxesLayout);
}

NoteEditDialog::GeneralTab::~GeneralTab()
{
    if(typeLabel) delete typeLabel;
    if(typeCombo) delete typeCombo;
    if(progressLabel) delete progressLabel;
    if(progressSpin) delete progressSpin;
    if(deadlineCheck) delete deadlineCheck;
    if(deadlineEdit) delete deadlineEdit;
    if(parentRelLabel) delete parentRelLabel;
    if(parentRelCombo) delete parentRelCombo;
}

void NoteEditDialog::GeneralTab::handleDeadlineCheck(int state)
{
    if(!state) {
        deadlineEdit->setEnabled(false);
    } else {
        deadlineEdit->setEnabled(true);
    }
}

/*
 * Advanced tab
 */

NoteEditDialog::AdvancedTab::AdvancedTab(QWidget* parent)
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

void NoteEditDialog::AdvancedTab::refreshPath(const QString& path)
{
    fileLine->setText(path);
}

NoteEditDialog::AdvancedTab::~AdvancedTab()
{
}

/*
 * Dialog
 */

NoteEditDialog::NoteEditDialog(Ontology& ontology, QWidget* parent)
    : QDialog{parent}, ontology(ontology)
{
    tabWidget = new QTabWidget;

    generalTab = new GeneralTab(ontology, this);
    tabWidget->addTab(generalTab, tr("General"));

    advancedTab = new AdvancedTab{this};
    tabWidget->addTab(advancedTab, tr("Advanced"));

    buttonBox = new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel};

    // wire signals ensuring that close & set dialog status
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    // IMPROVE extra wiring below to be removed - I was unable to connect QDialog::accept from outside :-/
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &NoteEditDialog::handleAccepted);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &NoteEditDialog::handleRejected);
    //QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, [=](){ MF_DEBUG("NoteEditDialog OK" << endl); });

    QVBoxLayout *mainLayout = new QVBoxLayout{this};
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setWindowTitle(tr("Edit Note"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

NoteEditDialog::~NoteEditDialog()
{
}

void NoteEditDialog::show()
{
    if(currentNote) {
        // RDWR
        if(currentNote->getType()) {
            int i = generalTab->typeCombo->findData(QVariant::fromValue<const NoteType*>(currentNote->getType()));
            if(i>=0) {
                generalTab->typeCombo->setCurrentIndex(i);
            } else {
                MF_DEBUG("Unknown Note type: " << currentNote->getType()->getName() << endl);
            }
        }
        generalTab->editTagsGroup->refresh(currentNote->getTags());
        generalTab->progressSpin->setValue(currentNote->getProgress());
        if(currentNote->getDeadline()) {
            generalTab->deadlineCheck->setChecked(true);
            generalTab->deadlineEdit->setEnabled(true);

            QDate date{};
            timetToQDate(currentNote->getDeadline(),date);
            generalTab->deadlineEdit->setDate(date);
        } else {
            generalTab->deadlineCheck->setChecked(false);
            generalTab->deadlineEdit->setEnabled(false);

            generalTab->deadlineEdit->setDate(QDate::currentDate());
        }

        // RDONLY
        advancedTab->createdLine->setText(QString::fromStdString(datetimeToString(currentNote->getCreated())));
        advancedTab->modifiedPanel->setText(QString::fromStdString(datetimeToString(currentNote->getModified())));
        advancedTab->readPanel->setText(QString::fromStdString(datetimeToString(currentNote->getRead())));
        advancedTab->readsPanel->setText(QString::number(currentNote->getReads()));
        advancedTab->writesPanel->setText(QString::number(currentNote->getRevision()));
        advancedTab->fileLine->setText(QString::fromStdString(currentNote->getOutlineKey()));
    }

    generalTab->editTagsGroup->setFocusAddingTag();

    QDialog::show();
}

void NoteEditDialog::toNote()
{
    if(currentNote) {
        if(generalTab->typeCombo->currentIndex() != -1) {
            currentNote->setType((const NoteType*)(generalTab->typeCombo->itemData(generalTab->typeCombo->currentIndex(), Qt::UserRole).value<const NoteType*>()));
        }
        currentNote->setTags(&generalTab->editTagsGroup->getTags());
        currentNote->setProgress(generalTab->progressSpin->value());
        if(generalTab->deadlineCheck->isChecked()) {
            tm date {0,0,0,0,0,0,0,0,0,0,0}; // missing initializer required by older GCC versions (4.8.5 and older)
            qdateToTm(generalTab->deadlineEdit->dateTime().date(), date);
            currentNote->setDeadline(datetimeSeconds(&date));
        } else {
            currentNote->setDeadline(0);
        }
    } else {
        MF_DEBUG("Attempt to save data from dialog to Note, but no Note is set." << endl);
    }
}

void NoteEditDialog::handleAccepted()
{
    emit acceptedSignal();
}

void NoteEditDialog::handleRejected()
{
    emit rejectedSignal();
}

} // m8r namespace
