/*
 notebook_tree_new_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include "notebook_tree_new_dialog.h"

namespace m8r {

using namespace std;

NotebookTreeNewDialog::NotebookTreeNewDialog(QWidget* parent)
    : QDialog{parent},
      notebookTreeToEdit{nullptr}
{
    // widgets
    nameLabel = new QLabel(tr("Name")+":", this);
    nameEdit = new QLineEdit(tr("Notebook Tree"), this);

    createButton = new QPushButton{tr("&Create")};
    createButton->setDefault(true);
    createButton->setEnabled(true);

    closeButton = new QPushButton{tr("&Cancel")};

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{this};

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(createButton);
    buttonLayout->addStretch();

    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // signals
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(createButton, SIGNAL(clicked()), this, SLOT(handleCreate()));

    // dialog
    setWindowTitle(tr("New Notebook Tree"));
    resize(fontMetrics().averageCharWidth()*60, fontMetrics().height()*6);
    setModal(true);
}

NotebookTreeNewDialog::~NotebookTreeNewDialog()
{
    delete createButton;
    delete closeButton;
}

void NotebookTreeNewDialog::show(NotebookTree* notebookTreeToEdit)
{
    this->notebookTreeToEdit = notebookTreeToEdit;

    if(!notebookTreeToEdit) {
        createButton->setText(tr("Create"));
        setWindowTitle(tr("New Notebook Tree"));

        nameEdit->setText(tr("Notebook Tree"));
    } else {
        createButton->setText(tr("Update"));
        setWindowTitle(tr("Rename Notebook Tree"));

        nameEdit->setText(QString::fromStdString(notebookTreeToEdit->getName()));
    }

    nameEdit->selectAll();
    nameEdit->setFocus();

    QDialog::show();
}

void NotebookTreeNewDialog::handleCreate()
{
    // VALIDATION of dialog data
    if(!nameEdit->text().size()) {
        QMessageBox::critical(
            this,
            tr("Notebook Tree Error"),
            tr("Notebook tree must have non-empty name.")
        );
        return;
    }

    emit createFinished();
}

} // m8r namespace
