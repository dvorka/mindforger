/*
 rows_and_depth_dialog.cpp     MindForger thinking notebook

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
#include "rows_and_depth_dialog.h"

namespace m8r {

RowsAndDepthDialog::RowsAndDepthDialog(QWidget* parent)
    : QDialog{parent}
{
    purpose = BULLETS;

    QVBoxLayout* mainLayout = new QVBoxLayout{this};

    label = new QLabel{tr("Specify number of rows and depth to generate")+":", this};
    mainLayout->addWidget(label);

    QWidget* w = new QWidget(this);
    QHBoxLayout* spinsLayout = new QHBoxLayout{this};
    rowsSpin = new QSpinBox{this};
    rowsSpin->setMinimum(1);
    rowsSpin->setMaximum(50);
    spinsLayout->addWidget(rowsSpin);
    spinsLayout->addWidget(new QLabel(tr("rows")));
    depthSpin = new QSpinBox{this};
    depthSpin->setMinimum(1);
    depthSpin->setMaximum(10);
    spinsLayout->addWidget(depthSpin);
    spinsLayout->addWidget(new QLabel(tr("depth")));
    w->setLayout(spinsLayout);

    generateButton = new QPushButton{tr("&Generate")};
    generateButton->setDefault(true);

    closeButton = new QPushButton{tr("&Cancel")};

    QHBoxLayout *buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(generateButton);
    buttonLayout->addStretch();

    mainLayout->addWidget(w);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    // signals
    connect(generateButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    setWindowTitle(tr("Format Generator"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

RowsAndDepthDialog::~RowsAndDepthDialog()
{
}

void RowsAndDepthDialog::show() {
    switch(purpose) {
    case BULLETS:
        setWindowTitle(tr("Bulleted List Generator"));
        break;
    case NUMBERS:
        setWindowTitle(tr("Numbered List Generator"));
        break;
    case TASKS:
        setWindowTitle(tr("Tasklist Generator"));
        break;
    case BLOCKQUOTE:
        setWindowTitle(tr("Block Quote Generator"));
        break;
    }

    rowsSpin->setValue(1);
    depthSpin->setValue(1);

    QDialog::show();
}

} // m8r namespace
