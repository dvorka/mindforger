/*
 new_file_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "new_file_dialog.h"

using namespace m8r::filesystem;

namespace m8r {

NewFileDialog::NewFileDialog(QWidget* parent)
    : QDialog(parent)
{
    homeDirectory = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);

    setWindowTitle(tr("Create New Markdown File"));
    extension = QString::fromStdString(File::EXTENSION_MD_MD);

    // widgets
    fileNameLabel = new QLabel{tr("Markdown name:")};
    fileNameEdit = new QLineEdit{};
    dirLabel = new QLabel{tr("Target directory:")};
    dirEdit = new QLineEdit{};
    pathLabel = new QLabel{tr("File to be created:")};
    pathEdit = new QLineEdit{};
    pathEdit->setEnabled(false);

    findDirectoryButton = new QPushButton{tr("Find Directory")};

    // IMPROVE disable/enable find button if text/path is valid: freedom vs validation
    newButton = new QPushButton{tr("New")};
    newButton->setDefault(true);
    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(fileNameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(refreshPath()));
    QObject::connect(dirEdit, SIGNAL(textChanged(const QString&)), this, SLOT(refreshPath()));
    QObject::connect(findDirectoryButton, SIGNAL(clicked()), this, SLOT(handleFindDirectory()));
    QObject::connect(newButton, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(fileNameLabel);
    mainLayout->addWidget(fileNameEdit);
    mainLayout->addWidget(dirLabel);
    mainLayout->addWidget(dirEdit);
    QHBoxLayout* srcButtonLayout = new QHBoxLayout{};
    srcButtonLayout->addWidget(findDirectoryButton);
    srcButtonLayout->addStretch();
    mainLayout->addLayout(srcButtonLayout);
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
    resize(fontMetrics().averageCharWidth()*60, height());
    setModal(true);
}

NewFileDialog::~NewFileDialog()
{
}

void NewFileDialog::show()
{
    fileNameEdit->setText(tr("name"));
    fileNameEdit->selectAll();
    fileNameEdit->setFocus();
#if defined(_WIN32)
    dirEdit->setText(homeDirectory.replace("/", "\\"));
#else
    dirEdit->setText(homeDirectory);
#endif

    refreshPath();

    QDialog::show();
}

void NewFileDialog::refreshPath()
{
    // dir
    QString directory{dirEdit->text()};
    if(directory.isEmpty()) {
        directory = QStandardPaths::locate(QStandardPaths::HomeLocation, QString(), QStandardPaths::LocateDirectory);
    }
#if defined(_WIN32)
    directory = directory.replace("/", "\\");
#endif
    if(!directory.endsWith(FILE_PATH_SEPARATOR)) {
        directory.append(FILE_PATH_SEPARATOR);
    }
    // name
    QString name{fileNameEdit->text()};
    if(name.isEmpty()) {
        name = tr("name");
    } else {
        name = QString::fromStdString(normalizeToNcName(name.toStdString(),'-'));
    }
    // path = dir + name
    QString path = directory+name+extension;

    pathEdit->setText(path);
}

void NewFileDialog::handleFindDirectory()
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
#if defined(_WIN32)
            dirEdit->setText(fileNames[0].replace("/", "\\"));
#else
            dirEdit->setText(fileNames[0]);
#endif
        } // else too many files
    } // else directory closed / nothing choosen
}

} // m8r namespace
