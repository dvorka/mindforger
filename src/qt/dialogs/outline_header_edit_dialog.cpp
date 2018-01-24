/*
 outline_header_edit_dialog.cpp     MindForger thinking notebook

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
#include "outline_header_edit_dialog.h"

namespace m8r {

OutlineHeaderEditDialog::OutlineHeaderEditDialog(Ontology& ontology, QWidget *parent)
    : QDialog{parent}, ontology(ontology)
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

    importanceLabel = new QLabel{tr("Importance")+": "+QChar(9733), this};
    importanceSpin = new QSpinBox{this};
    importanceSpin->setMinimum(0);
    importanceSpin->setMaximum(5);

    urgencyLabel = new QLabel{tr("Urgency")+": "+QChar(0x29D7), this};
    urgencySpin = new QSpinBox{this};
    urgencySpin->setMinimum(0);
    urgencySpin->setMaximum(5);

    progressLabel = new QLabel{tr("Progress")+": %", this};
    progressSpin = new QSpinBox{this};
    progressSpin->setMinimum(0);
    progressSpin->setMaximum(100);

    editTagsGroup = new EditTagsPanel{ontology, this};

    QGroupBox* advancedGroup = new QGroupBox{tr("Metadata"), this};

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
    locationLabel = new QLabel{tr("Location")+":", this};
    locationLine = new QLineEdit{this};
    locationLine->setDisabled(true);

    buttonBox = new QDialogButtonBox{QDialogButtonBox::Ok | QDialogButtonBox::Cancel};

    // assembly
    QVBoxLayout* basicLayout = new QVBoxLayout{this};
    basicLayout->addWidget(typeLabel);
    basicLayout->addWidget(typeCombo);
    basicLayout->addWidget(importanceLabel);
    basicLayout->addWidget(importanceSpin);
    basicLayout->addWidget(urgencyLabel);
    basicLayout->addWidget(urgencySpin);
    basicLayout->addWidget(progressLabel);
    basicLayout->addWidget(progressSpin);
    basicGroup->setLayout(basicLayout);
    QVBoxLayout* advancedLayout = new QVBoxLayout{this};
    advancedLayout->addWidget(createdLabel);
    advancedLayout->addWidget(createdLine);
    QWidget* w = new QWidget{this};
    QHBoxLayout* l = new QHBoxLayout(w);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(modifiedPanel);
    l->addWidget(readPanel);
    w->setLayout(l);
    advancedLayout->addWidget(w);
    w = new QWidget{this};
    l = new QHBoxLayout(w);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(readsPanel);
    l->addWidget(writesPanel);
    w->setLayout(l);
    advancedLayout->addWidget(w);
    advancedLayout->addWidget(locationLabel);
    advancedLayout->addWidget(locationLine);
    advancedGroup->setLayout(advancedLayout);
    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(basicGroup);
    boxesLayout->addWidget(editTagsGroup);
    boxesLayout->addWidget(advancedGroup);
    boxesLayout->addWidget(buttonBox);
    setLayout(boxesLayout);

    // wire signals ensuring that close & set dialog status
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    // IMPROVE extra wiring below to be removed - I was unable to connect QDialog::accept from outside :-/
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &OutlineHeaderEditDialog::handleAccepted);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &OutlineHeaderEditDialog::handleRejected);
    //QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, [=](){ qDebug("NoteEditDialog OK"); });

    setWindowTitle(tr("Edit Outline Properties"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

OutlineHeaderEditDialog::~OutlineHeaderEditDialog()
{
    delete typeLabel;
    delete typeCombo;
}

void OutlineHeaderEditDialog::show()
{
    if(currentOutline) {
        // RDWR
        if(currentOutline->getType()) {
            int i = typeCombo->findData(QVariant::fromValue<const OutlineType*>(currentOutline->getType()));
            if(i>=0) {
                typeCombo->setCurrentIndex(i);
            } else {
                qDebug() << "Unknown Outline type: " << QString::fromStdString(currentOutline->getType()->getName());
            }
        }
        editTagsGroup->refresh(currentOutline->getTags());
        progressSpin->setValue(currentOutline->getProgress());

        // RDONLY
        createdLine->setText(QString::fromStdString(datetimeToString(currentOutline->getCreated())));
        modifiedPanel->setText(QString::fromStdString(datetimeToString(currentOutline->getModified())));
        readPanel->setText(QString::fromStdString(datetimeToString(currentOutline->getRead())));
        readsPanel->setText(QString::number(currentOutline->getReads()));
        writesPanel->setText(QString::number(currentOutline->getRevision()));
        locationLine->setText(QString::fromStdString(currentOutline->getKey()));
    }

    QDialog::show();
}

void OutlineHeaderEditDialog::toOutline()
{
    if(currentOutline) {
        if(typeCombo->currentIndex() != -1) {
            currentOutline->setType((const OutlineType*)(typeCombo->itemData(typeCombo->currentIndex(), Qt::UserRole).value<const OutlineType*>()));
        }
        currentOutline->setTags((editTagsGroup->getTags()));
        currentOutline->setProgress(progressSpin->value());
    } else {
        qDebug("Attempt to save data from dialog to Outline, but no Outline is set.");
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
