/*
 ner_result_dialog.cpp     MindForger thinking notebook

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
#include "ner_result_dialog.h"

namespace m8r {

NerResultDialog::NerResultDialog(QWidget* parent)
    : QDialog(parent)
{
    // widgets
    leaderboardModel = new NerLeaderboardModel(this);
    leaderboardView = new NerLeaderboardView(this);
    leaderboardView->setModel(leaderboardModel);

    label = new QLabel{tr("Named entities:")};

    findButton = new QPushButton{tr("&Find Entity in Notes")};
    findButton->setDefault(true);
    findButton->setEnabled(false);

    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    connect(findButton, SIGNAL(clicked()), this, SLOT(handleChoice()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // assembly
    QVBoxLayout *mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(label);
    mainLayout->addWidget(leaderboardView);

    QHBoxLayout *buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(findButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Find Named Entities"));
    // height is set to make sure listview gets enough lines
    resize(fontMetrics().averageCharWidth()*55, fontMetrics().height()*30);
    setModal(true);

}

NerResultDialog::~NerResultDialog()
{
    delete label;
    delete leaderboardView;
    delete leaderboardModel;
    delete closeButton;
}

void NerResultDialog::handleChoice()
{
    // TODO table validity / choice ~ disable button if(listView->currentIndex().isValid()) {
        QDialog::close();

        // TODO signal to get entity name "..." string as parameter
        emit searchFinished();
    //}
}


} // m8r namespace
