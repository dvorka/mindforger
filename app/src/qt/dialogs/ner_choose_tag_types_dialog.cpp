/*
 ner_choose_tag_types_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "ner_choose_tag_types_dialog.h"

namespace m8r {

NerChooseTagTypesDialog::NerChooseTagTypesDialog(QWidget* parent)
    : QDialog(parent)
{
    // widgets
    label = new QLabel{tr("Choose entity types to be extracted:")};

    personsCheckBox = new QCheckBox{tr("persons")};
    locationsCheckBox = new QCheckBox{tr("locations")};
    organizationsCheckBox = new QCheckBox{tr("organizations")};
    miscCheckBox = new QCheckBox{tr("other entities")};

    chooseButton = new QPushButton{tr("&Choose")};
    chooseButton->setDefault(true);

    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(personsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotChooseButtonEnable(int)));
    connect(locationsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotChooseButtonEnable(int)));
    connect(organizationsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotChooseButtonEnable(int)));
    connect(miscCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotChooseButtonEnable(int)));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(label);
    mainLayout->addWidget(personsCheckBox);
    mainLayout->addWidget(locationsCheckBox);
    mainLayout->addWidget(organizationsCheckBox);
    mainLayout->addWidget(miscCheckBox);

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(chooseButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Choose Entity Type"));
    resize(fontMetrics().averageCharWidth()*35, height());
    setModal(true);
}

NerChooseTagTypesDialog::~NerChooseTagTypesDialog()
{
}

void NerChooseTagTypesDialog::slotChooseButtonEnable(int state)
{
    UNUSED_ARG(state);

    if(personsCheckBox->isChecked() || locationsCheckBox->isChecked() || organizationsCheckBox->isChecked() || miscCheckBox->isChecked()) {
        chooseButton->setEnabled(true);
    } else {
        chooseButton->setEnabled(false);
    }
}

} // m8r namespace
