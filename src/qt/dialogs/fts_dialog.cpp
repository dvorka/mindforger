/*
 fts_dialog.cpp     MindForger thinking notebook

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
#include "fts_dialog.h"

namespace m8r {

FtsDialog::FtsDialog(QWidget *parent)
    : QDialog(parent)
{
    // widgets
    label = new QLabel(tr("&Find string:"));
    lineEdit = new QLineEdit;
    label->setBuddy(lineEdit);

    caseCheckBox = new QCheckBox(tr("&Ignore case"));

    findButton = new QPushButton(tr("&Search"));
    findButton->setDefault(true);
    findButton->setEnabled(false);

    closeButton = new QPushButton(tr("&Cancel"));

    // signals
    connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableFindButton(const QString&)));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // assembly
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    mainLayout->addLayout(mainLayout);
    mainLayout->addWidget(caseCheckBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(findButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Full-text Search"));
    setModal(true);
}

void FtsDialog::enableFindButton(const QString& text)
{
    findButton->setEnabled(!text.isEmpty());
}

FtsDialog::~FtsDialog()
{
    delete label;
    delete lineEdit;
    delete caseCheckBox;
    delete findButton;
    delete closeButton;
}

}
