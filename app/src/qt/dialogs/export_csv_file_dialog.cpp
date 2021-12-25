/*
 export_csv_file_dialog.cpp     MindForger thinking notebook

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
#include "export_csv_file_dialog.h"

using namespace m8r::filesystem;

namespace m8r {

ExportCsvFileDialog::ExportCsvFileDialog(
     QString title,
     QString button,
     QString extension,
     QWidget* parent
)
    : QDialog(parent)
{
    homeDirectory = QStandardPaths::locate(
        QStandardPaths::HomeLocation,
        QString(),
        QStandardPaths::LocateDirectory
    );

    if(title.size()) {
        setWindowTitle(title);
    } else {
        setWindowTitle(tr("Create New Markdown File"));
    }
    if(!extension.size()) {
        this->extension = QString::fromStdString(File::EXTENSION_MD_MD);
    } else {
        this->extension = extension;
    }

    // widgets
    fileNameLabel = new QLabel{tr("File name:")};
    fileNameEdit = new QLineEdit{};
    dirLabel = new QLabel{tr("Target directory:")};
    dirEdit = new QLineEdit{};
    pathLabel = new QLabel{tr("File to be created:")};
    pathEdit = new QLineEdit{};
    pathEdit->setEnabled(false);

    findDirectoryButton = new QPushButton{tr("Find Directory")};

    oheTagsCheck = new QCheckBox(tr("export OHE (one hot encoded) tags"), this);
    oheTagsCardinalityLabel = new QLabel{tr("Minimum tag cardinality to be OHE exported:")};
    oheTagsCardinalitySpin = new QSpinBox(this);
    oheTagsCardinalitySpin->setMinimum(DEFAULT_OHE_CARDINALITY);
    oheTagsCardinalitySpin->setMaximum(10000);

    // IMPROVE disable/enable find button if text/path is valid: freedom vs validation
    exportButton = new QPushButton{tr("Export")};
    exportButton->setDefault(true);
    if(button.size()) {
        exportButton->setText(button);
    }
    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(
        oheTagsCheck, SIGNAL(clicked(bool)),
        this, SLOT(enableDisableOheCardinality(bool)));
    QObject::connect(
        fileNameEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(refreshPath()));
    QObject::connect(
        dirEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(refreshPath()));
    QObject::connect(
        findDirectoryButton, SIGNAL(clicked()),
        this, SLOT(handleFindDirectory()));
    QObject::connect(
        exportButton, SIGNAL(clicked()),
        this, SLOT(close()));
    QObject::connect(
        closeButton, SIGNAL(clicked()),
        this, SLOT(close()));

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
    mainLayout->addWidget(oheTagsCheck);
    mainLayout->addWidget(oheTagsCardinalityLabel);
    mainLayout->addWidget(oheTagsCardinalitySpin);

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    resize(fontMetrics().averageCharWidth()*60, height());
    setModal(true);
}

ExportCsvFileDialog::~ExportCsvFileDialog()
{
}

void ExportCsvFileDialog::show()
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

    oheTagsCheck->setChecked(false);
    oheTagsCardinalitySpin->setValue(DEFAULT_OHE_CARDINALITY);
    oheTagsCardinalitySpin->setEnabled(false);

    QDialog::show();
}

void ExportCsvFileDialog::refreshPath()
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

void ExportCsvFileDialog::handleFindDirectory()
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

void ExportCsvFileDialog::enableDisableOheCardinality(bool enable)
{
    oheTagsCardinalitySpin->setEnabled(enable);
}

} // m8r namespace