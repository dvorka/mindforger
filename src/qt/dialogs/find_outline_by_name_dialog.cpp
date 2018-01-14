/*
 find_outline_by_name_dialog.cpp     MindForger thinking notebook

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
#include "find_outline_by_name_dialog.h"

namespace m8r {

using namespace std;

FindOutlineByNameDialog::FindOutlineByNameDialog(QWidget *parent)
    : QDialog(parent)
{
    // widgets
    listView = new QListView(this);
    listView->setModel(&listViewModel);

    label = new QLabel{tr("Outline name:")};
    lineEdit = new MyLineEdit{listView, this};
    label->setBuddy(lineEdit);

    caseCheckBox = new QCheckBox{tr("&Ignore case")};
    caseCheckBox->setChecked(true);

    findButton = new QPushButton{tr("&Open Outline")};
    findButton->setDefault(true);
    findButton->setEnabled(false);

    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableFindButton(const QString&)));
    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(handleReturn()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(handleChoice()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // assembly
    QVBoxLayout *mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    mainLayout->addWidget(listView);
    mainLayout->addWidget(caseCheckBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(findButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Find Outline by Name"));
    // height is set to make sure listview gets enough lines
    resize(fontMetrics().averageCharWidth()*55, fontMetrics().height()*30);
    setModal(true);
}

void FindOutlineByNameDialog::enableFindButton(const QString& text)
{
    // IMPROVE use HSTR algorithm to be smarter
    listViewStrings.clear();
    if(!text.isEmpty()) {
        Qt::CaseSensitivity c = caseCheckBox->isChecked()?Qt::CaseInsensitive:Qt::CaseSensitive;
        // IMPROVE find a list view method giving # of visible rows
        int visible = 0;
        int row = 0;
        for(Outline* o:outlines) {
            QString s = QString::fromStdString(o->getTitle());
            if(s.startsWith(text,c)) {
                listView->setRowHidden(row, false);
                visible++;
            } else {
                listView->setRowHidden(row, true);
            }
            row++;
        }
        findButton->setEnabled(visible);
    } else {
        for(size_t row = 0; row<outlines.size(); row++) {
            listView->setRowHidden(row, false);
        }
        findButton->setEnabled(outlines.size());
    }
}

void FindOutlineByNameDialog::show(vector<Outline*>& os)
{
    choice = nullptr;
    outlines.clear();
    listViewStrings.clear();
    if(os.size()) {
        for(Outline* o:os) {
            outlines.push_back(o);
            if(o->getTitle().size()) {
                listViewStrings << QString::fromStdString(o->getTitle());
            }
        }
        ((QStringListModel*)listView->model())->setStringList(listViewStrings);
    }

    findButton->setEnabled(outlines.size());
    lineEdit->clear();
    lineEdit->setFocus();
    QDialog::show();
}

void FindOutlineByNameDialog::handleReturn(void)
{
    if(findButton->isEnabled()) {
        for(size_t row = 0; row<outlines.size(); row++) {
            if(!listView->isRowHidden(row)) {
                choice = outlines[row];
                break;
            }
        }

        QDialog::close();
        emit searchFinished();
    }
}

void FindOutlineByNameDialog::handleChoice(void)
{
    if(listView->currentIndex().isValid()) {
        choice = outlines[listView->currentIndex().row()];

        QDialog::close();
        emit searchFinished();
    }
}

FindOutlineByNameDialog::~FindOutlineByNameDialog()
{
    delete label;
    delete lineEdit;
    delete listView;
    delete caseCheckBox;
    delete findButton;
    delete closeButton;
}

}
