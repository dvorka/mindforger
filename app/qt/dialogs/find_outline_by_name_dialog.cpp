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
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "find_outline_by_name_dialog.h"

#include "../gear/qutils.h"

namespace m8r {

using namespace std;

FindOutlineByNameDialog::FindOutlineByNameDialog(QWidget *parent)
    : QDialog(parent)
{
    // widgets
    listView = new QListView(this);
    // list view model must be set - use of this type of mode enable the use of string lists controlling its content
    listView->setModel(&listViewModel);
    // disable editation of the list item on doble click
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    label = new QLabel{tr("Outline &name:")};
    lineEdit = new MyLineEdit{listView, this};
    label->setBuddy(lineEdit);

    caseCheckBox = new QCheckBox{tr("&Ignore case")};
    caseCheckBox->setChecked(true);
    keywordsCheckBox = new QCheckBox{tr("&Keywords match")};
    keywordsCheckBox->setChecked(true);

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
    mainLayout->addWidget(keywordsCheckBox);

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

FindOutlineByNameDialog::~FindOutlineByNameDialog()
{
    delete label;
    delete lineEdit;
    delete listView;
    delete caseCheckBox;
    delete findButton;
    delete closeButton;
}

void FindOutlineByNameDialog::enableFindButton(const QString& text)
{
    listViewStrings.clear();
    if(!text.isEmpty()) {
        if(keywordsCheckBox->isEnabled() && keywordsCheckBox->isChecked()) {
            int visible = 0;
            int row = 0;
            for(Thing* e:things) {
                QString s = QString::fromStdString(e->getName());
                if(stringMatchByKeywords(text, s, caseCheckBox->isChecked())) {
                    listView->setRowHidden(row, false);
                    visible++;
                } else {
                    listView->setRowHidden(row, true);
                }
                row++;
            }
            findButton->setEnabled(visible);
        } else {
            Qt::CaseSensitivity c = caseCheckBox->isChecked()?Qt::CaseInsensitive:Qt::CaseSensitive;
            // IMPROVE find a list view method giving # of visible rows
            int visible = 0;
            int row = 0;
            for(Thing* e:things) {
                QString s = QString::fromStdString(e->getName());
                if(s.startsWith(text,c)) {
                    listView->setRowHidden(row, false);
                    visible++;
                } else {
                    listView->setRowHidden(row, true);
                }
                row++;
            }
            findButton->setEnabled(visible);
        }
    } else {
        for(size_t row = 0; row<things.size(); row++) {
            listView->setRowHidden(row, false);
        }
        findButton->setEnabled(things.size());
    }
}

void FindOutlineByNameDialog::show(vector<Thing*>& outlines, vector<string>* customizedNames)
{
    choice = nullptr;
    things.clear();
    listViewStrings.clear();
    bool useCustomNames = customizedNames!=nullptr && customizedNames->size()>0;
    if(outlines.size()) {
        for(size_t i=0; i<outlines.size(); i++) {
            things.push_back(outlines[i]);
            if(useCustomNames) {
                listViewStrings << QString::fromStdString(customizedNames->at(i));
            } else {
                if(outlines.at(i)->getName().size()) {
                    listViewStrings << QString::fromStdString(outlines[i]->getName());
                } else {
                    listViewStrings << "";
                }
            }
        }
        ((QStringListModel*)listView->model())->setStringList(listViewStrings);
    }

    findButton->setEnabled(things.size());
    lineEdit->clear();
    lineEdit->setFocus();
    QDialog::show();
}

void FindOutlineByNameDialog::handleReturn()
{
    if(findButton->isEnabled()) {
        for(size_t row = 0; row<things.size(); row++) {
            if(!listView->isRowHidden(row)) {
                choice = things[row];
                break;
            }
        }

        QDialog::close();
        emit searchFinished();
    }
}

void FindOutlineByNameDialog::handleChoice()
{
    if(listView->currentIndex().isValid()) {
        choice = things[listView->currentIndex().row()];

        QDialog::close();
        emit searchFinished();
    }
}

} // m8r namespace
