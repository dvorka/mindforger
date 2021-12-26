/*
 insert_image_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "insert_image_dialog.h"

namespace m8r {

InsertImageDialog::InsertImageDialog(QWidget* parent)
    : QDialog(parent)
{
    // widgets
    alternateTextLabel = new QLabel{tr("Alternate text:")};
    alternateTextEdit = new QLineEdit{};
    pathLabel = new QLabel{tr("Image file path or web address (URL):")};
    pathEdit = new QLineEdit{};

    findFileButton = new QPushButton{tr("File")};
    copyToRepoCheckBox = new QCheckBox{tr("copy image to repository")};
    copyToRepoCheckBox->setChecked(true);
    copyToRepoCheckBox->setEnabled(true);

    // IMPROVE disable/enable find button if text/path is valid: freedom vs validation
    insertButton = new QPushButton{tr("&Insert")};
    insertButton->setDefault(true);
    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(findFileButton, SIGNAL(clicked()), this, SLOT(handleFindFile()));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(alternateTextLabel);
    mainLayout->addWidget(alternateTextEdit);
    mainLayout->addWidget(pathLabel);
    mainLayout->addWidget(pathEdit);
    QHBoxLayout* srcButtonLayout = new QHBoxLayout{};
    srcButtonLayout->addWidget(findFileButton);
    srcButtonLayout->addStretch();
    mainLayout->addLayout(srcButtonLayout);
    mainLayout->addWidget(copyToRepoCheckBox);

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(insertButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Insert Image"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

InsertImageDialog::~InsertImageDialog()
{
}

void InsertImageDialog::show()
{
    alternateTextEdit->setText(tr("Image"));
    alternateTextEdit->selectAll();
    alternateTextEdit->setFocus();
    pathEdit->clear();

    QDialog::show();
}

void InsertImageDialog::show(const QString& text, const QString& link)
{
    pathEdit->setText(link);
    alternateTextEdit->setText(text);
    alternateTextEdit->selectAll();
    alternateTextEdit->setFocus();

    QDialog::show();
}

void InsertImageDialog::handleFindFile()
{
    QString homeDirectory
        = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);

    QFileDialog fileDialog{this};
    fileDialog.setWindowTitle(tr("Choose File with Image"));
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setDirectory(homeDirectory);
    fileDialog.setViewMode(QFileDialog::Detail);

    QStringList imageFileNames{};
    if(fileDialog.exec()) {
        imageFileNames = fileDialog.selectedFiles();
        if(imageFileNames.size()==1) {
            pathEdit->setText(imageFileNames[0]);
        } // else too many files
    } // else directory closed / nothing choosen
}

} // m8r namespace
