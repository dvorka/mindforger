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

NoteEditDialog::NoteEditDialog(Ontology& ontology, QWidget* parent)
    : QDialog(parent), ontology(ontology)
{
    QGroupBox* basicGroup = new QGroupBox{tr("Basic"), this};

    typeLabel = new QLabel{tr("Type")+":", this};
    typeCombo = new QComboBox(this);
    if(ontology.getNoteTypes().size()) {
        for(const NoteType* t:ontology.getNoteTypes().values()) {
            typeCombo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<const NoteType*>(t));
        }
        typeCombo->setCurrentText(QString::fromStdString(ontology.getDefaultNoteType()->getName()));
    }

    tagLabel = new QLabel{tr("Tag")+":", this};
    tagCombo = new QComboBox(this);
    if(ontology.getTags().size()) {
        tagCombo->addItem("", QVariant::fromValue<const Tag*>(nullptr));
        for(const Tag* t:ontology.getTags().values()) {
            if(!stringistring(string("none"), t->getName())) {
                tagCombo->addItem(QString::fromStdString(t->getName()), QVariant::fromValue<const Tag*>(t));
            }
        }
        tagCombo->setCurrentText("");
    }

    progressLabel = new QLabel{tr("Progress")+":", this};
    progressSpin = new QSpinBox(this);
    progressSpin->setMinimum(0);
    progressSpin->setMaximum(100);

    QGroupBox* advancedGroup = new QGroupBox{tr("Advanced"), this};

    createdLabel = new QLabel{tr("Created")+":", this};
    createdLine = new QLineEdit{this};
    createdLine->setDisabled(true);
    modifiedLabel = new QLabel{tr("Modified")+":", this};
    modifiedLine = new QLineEdit{this};
    modifiedLine->setDisabled(true);
    readLabel = new QLabel{tr("Read")+":", this};
    readLine = new QLineEdit{this};
    readLine->setDisabled(true);
    readsLabel = new QLabel{tr("Reads")+":", this};
    readsLine = new QLineEdit{this};
    readsLine->setDisabled(true);
    writesLabel = new QLabel{tr("Writes")+":", this};
    writesLine = new QLineEdit{this};
    writesLine->setDisabled(true);
    locationLabel = new QLabel{tr("Location")+":", this};
    locationLine = new QLineEdit{this};
    locationLine->setDisabled(true);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // assembly
    QVBoxLayout* basicLayout = new QVBoxLayout{};
    basicLayout->addWidget(typeLabel);
    basicLayout->addWidget(typeCombo);
    basicLayout->addWidget(tagLabel);
    basicLayout->addWidget(tagCombo);
    basicLayout->addWidget(progressLabel);
    basicLayout->addWidget(progressSpin);
    // TODO basicLayout->addStretch(1);
    basicGroup->setLayout(basicLayout);
    QVBoxLayout* advancedLayout = new QVBoxLayout{};
    advancedLayout->addWidget(createdLabel);
    advancedLayout->addWidget(createdLine);
    advancedLayout->addWidget(modifiedLabel);
    advancedLayout->addWidget(modifiedLine);
    advancedLayout->addWidget(readLabel);
    advancedLayout->addWidget(readLine);
    advancedLayout->addWidget(readsLabel);
    advancedLayout->addWidget(readsLine);
    advancedLayout->addWidget(writesLabel);
    advancedLayout->addWidget(writesLine);
    advancedLayout->addWidget(locationLabel);
    advancedLayout->addWidget(locationLine);
    advancedGroup->setLayout(advancedLayout);
    QVBoxLayout* boxesLayout = new QVBoxLayout{this};
    boxesLayout->addWidget(basicGroup);
    boxesLayout->addWidget(advancedGroup);
    boxesLayout->addWidget(buttonBox);
    setLayout(boxesLayout);

    // wire signals ensuring that close & set dialog status
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    setWindowTitle(tr("Edit Note Properties"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

NoteEditDialog::~NoteEditDialog()
{
    delete typeLabel;
    delete typeCombo;
    delete tagLabel;
    delete tagCombo;
}

void NoteEditDialog::show()
{
    if(currentNote) {
        // RDWR
        if(currentNote->getType()) {
            int i = typeCombo->findData(QVariant::fromValue<const NoteType*>(currentNote->getType()));
            if(i>=0) {
                typeCombo->setCurrentIndex(i);
            } else {
                qDebug() << "Unknown note type: " << QString::fromStdString(currentNote->getType()->getName());
            }
        }
        // IMPROVE handle MULTIPLE tags
        if(currentNote->getPrimaryTag()) {
            int i = tagCombo->findData(QVariant::fromValue<const Tag*>(currentNote->getPrimaryTag()));
            if(i>=0) {
                tagCombo->setCurrentIndex(i);
            } else {
                qDebug() << "Unknown tag: " << QString::fromStdString(currentNote->getPrimaryTag()->getName());
            }
        }

        progressSpin->setValue(currentNote->getProgress());
        // TODO deadline h
        // view->setNoteDeadline(note->getDeadline());
        // RDONLY
        createdLine->setText(QString::fromStdString(datetimeToString(currentNote->getCreated())));
        modifiedLine->setText(QString::fromStdString(datetimeToString(currentNote->getModified())));
        readLine->setText(QString::fromStdString(datetimeToString(currentNote->getRead())));
        readsLine->setText(QString::number(currentNote->getReads()));
        writesLine->setText(QString::number(currentNote->getRevision()));
        locationLine->setText(QString::fromStdString(currentNote->getOutlineKey()));
    }

    QDialog::show();
}

}
