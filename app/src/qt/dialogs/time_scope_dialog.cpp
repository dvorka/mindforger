/*
 forget_dialog.cpp     MindForger thinking notebook

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
#include "time_scope_dialog.h"

namespace m8r {

TimeScopeDialog::TimeScopeDialog(QWidget *parent)
    : QDialog{parent}
{
    QVBoxLayout* timepointThresholdLayout = new QVBoxLayout{this};

    enableCheck = new QCheckBox{tr("Show only Notebooks/Notes modified or viewed in recent")+":", this};
    enableCheck->setChecked(false);

    QWidget* w = new QWidget(this);
    QHBoxLayout* spinsLayout = new QHBoxLayout{this};
    yearSpin = new QSpinBox{this};
    yearSpin->setEnabled(false);
    yearSpin->setMinimum(0);
    yearSpin->setMaximum(50);
    spinsLayout->addWidget(yearSpin);
    spinsLayout->addWidget(new QLabel(tr("year(s)")));
    monthSpin = new QSpinBox{this};
    monthSpin->setEnabled(false);
    monthSpin->setMinimum(0);
    monthSpin->setMaximum(11);
    spinsLayout->addWidget(monthSpin);
    spinsLayout->addWidget(new QLabel(tr("month(s)")));
    daySpin = new QSpinBox{this};
    daySpin->setEnabled(false);
    daySpin->setMinimum(0);
    daySpin->setMaximum(30);
    spinsLayout->addWidget(daySpin);
    spinsLayout->addWidget(new QLabel(tr("day(s)")));
    hourSpin = new QSpinBox{this};
    hourSpin->setEnabled(false);
    hourSpin->setMinimum(0);
    hourSpin->setMaximum(23);
    spinsLayout->addWidget(hourSpin);
    spinsLayout->addWidget(new QLabel(tr("hour(s)")));
    minuteSpin = new QSpinBox{this};
    minuteSpin->setEnabled(false);
    minuteSpin->setMinimum(0);
    minuteSpin->setMaximum(59);
    spinsLayout->addWidget(minuteSpin);
    spinsLayout->addWidget(new QLabel(tr("minute(s)")));
    w->setLayout(spinsLayout);

    setButton = new QPushButton{tr("&Set")};
    setButton->setDefault(true);

    closeButton = new QPushButton{tr("&Cancel")};

    QHBoxLayout *buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(setButton);
    buttonLayout->addStretch();

    timepointThresholdLayout->addWidget(enableCheck);
    timepointThresholdLayout->addWidget(w);
    timepointThresholdLayout->addWidget(w);
    timepointThresholdLayout->addLayout(buttonLayout);

    setLayout(timepointThresholdLayout);

    // signals
    QObject::connect(enableCheck, SIGNAL(clicked(bool)), this, SLOT(enableDisable(bool)));
    connect(setButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    setWindowTitle(tr("Scope Mind"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

TimeScopeDialog::~TimeScopeDialog()
{
    // TODO delete all widgets
}

void TimeScopeDialog::enableDisable(bool enable)
{
    yearSpin->setEnabled(enable);
    monthSpin->setEnabled(enable);
    daySpin->setEnabled(enable);
    hourSpin->setEnabled(enable);
    minuteSpin->setEnabled(enable);
}

} // m8r namespace
