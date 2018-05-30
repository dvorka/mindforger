/*
 new_repository_dialog.cpp     MindForger thinking notebook

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
#include "new_repository_dialog.h"

namespace m8r {

NewRepositoryDialog::NewRepositoryDialog(QWidget* parent)
    : QDialog(parent)
{
    homeDirectory = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);

    // widgets
    repositoryNameLabel = new QLabel{tr("Repository name:")};
    repositoryNameEdit = new QLineEdit{};
    dirLabel = new QLabel{tr("Repository directory:")};
    dirEdit = new QLineEdit{};
    pathLabel = new QLabel{tr("Repository to be created in:")};
    pathEdit = new QLineEdit{};
    pathEdit->setEnabled(false);

    findDirectoryButton = new QPushButton{tr("Find Directory")};

    copyStencilsCheckbox= new QCheckBox{tr("include stencils")};
    copyDocCheckbox = new QCheckBox{tr("include documentation")};

    // IMPROVE disable/enable find button if text/path is valid: freedom vs validation
    newButton = new QPushButton{tr("&New")};
    newButton->setDefault(true);
    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(repositoryNameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(refreshPath()));
    QObject::connect(dirEdit, SIGNAL(textChanged(const QString&)), this, SLOT(refreshPath()));
    QObject::connect(findDirectoryButton, SIGNAL(clicked()), this, SLOT(handleFindDirectory()));
    QObject::connect(newButton, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(repositoryNameLabel);
    mainLayout->addWidget(repositoryNameEdit);
    mainLayout->addWidget(dirLabel);
    mainLayout->addWidget(dirEdit);
    QHBoxLayout* srcButtonLayout = new QHBoxLayout{};
    srcButtonLayout->addWidget(findDirectoryButton);
    srcButtonLayout->addStretch();
    mainLayout->addLayout(srcButtonLayout);
    mainLayout->addWidget(copyStencilsCheckbox);
    mainLayout->addWidget(copyDocCheckbox);
    mainLayout->addWidget(pathLabel);
    mainLayout->addWidget(pathEdit);

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(newButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    setWindowTitle(tr("Create New Repository"));
    resize(fontMetrics().averageCharWidth()*60, height());
    setModal(true);
}

NewRepositoryDialog::~NewRepositoryDialog()
{
}

void NewRepositoryDialog::show()
{
    repositoryNameEdit->setText(tr("mindforger-repository"));
    repositoryNameEdit->selectAll();
    repositoryNameEdit->setFocus();
    dirEdit->setText(homeDirectory);

    refreshPath();

    copyStencilsCheckbox->setChecked(true);
    copyDocCheckbox->setChecked(true);

    QDialog::show();
}

void NewRepositoryDialog::refreshPath()
{
    // dir
    QString directory{dirEdit->text()};
    if(directory.isEmpty()) {
        directory = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
    }
    if(!directory.endsWith(FILE_PATH_SEPARATOR)) {
        directory.append(FILE_PATH_SEPARATOR);
    }
    // name
    QString name{repositoryNameEdit->text()};
    if(name.isEmpty()) {
        name = tr("mindforger-repository");
    } else {
        name = QString::fromStdString(normalizeToNcName(name.toStdString(),'-'));
    }
    // path = dir + name
    QString path = directory+name;

    pathEdit->setText(path);
}

void NewRepositoryDialog::handleFindDirectory()
{
    QString homeDirectory
        = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);

    QFileDialog fileDialog{this};
    fileDialog.setWindowTitle(tr("Choose Directory"));
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setDirectory(homeDirectory);
    fileDialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames{};
    if(fileDialog.exec()) {
        fileNames = fileDialog.selectedFiles();
        if(fileNames.size()==1) {
            dirEdit->setText(fileNames[0]);
        } // else too many files
    } // else directory closed / nothing choosen
}

} // m8r namespace
