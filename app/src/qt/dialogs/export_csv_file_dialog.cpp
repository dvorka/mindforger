/*
 export_csv_file_dialog.cpp     MindForger thinking notebook

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
#include "export_csv_file_dialog.h"

using namespace m8r::filesystem;

namespace m8r {

ExportCsvFileDialog::ExportCsvFileDialog(
     QString title,
     QString button,
     QString extension,
     QWidget* parent
)
    : QDialog(parent),
      csvExport{nullptr}
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

    progressBar = new QProgressBar(this);
    progressBar->setFormat("%p%  (%v / %m)");
    progressBar->setVisible(false);

    // timeout 0 ~ run slice whenever UI event loop is idle
    exportTimer = new QTimer(this);
    exportTimer->setInterval(0);

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
        closeButton, SIGNAL(clicked()),
        this, SLOT(reject()));
    QObject::connect(
        exportTimer, SIGNAL(timeout()),
        this, SLOT(handleExportSlice()));

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
    mainLayout->addWidget(progressBar);

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
    // unique_ptr deletes (and thus cancels) running export
    exportTimer->stop();
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

    setExportRunning(false);

    QDialog::show();
}

void ExportCsvFileDialog::runExport(CsvOutlinesExport* csvExport)
{
    if(isExportRunning()) {
        delete csvExport;
        return;
    }

    this->csvExport.reset(csvExport);
    setExportRunning(true);
    progressBar->setMaximum(static_cast<int>(csvExport->getTotalRows()));
    progressBar->setValue(0);

    if(csvExport->start()) {
        exportTimer->start();
    } else {
        finishExport();
    }
}

void ExportCsvFileDialog::handleExportSlice()
{
    if(!isExportRunning()) {
        exportTimer->stop();
        return;
    }

    QElapsedTimer sliceTimer{};
    sliceTimer.start();
    bool hasMore{true};
    do {
        hasMore = csvExport->step(EXPORT_ROWS_PER_STEP);
    } while(hasMore && sliceTimer.elapsed() < EXPORT_SLICE_MILLIS);

    progressBar->setValue(static_cast<int>(csvExport->getExportedRows()));

    if(!hasMore) {
        finishExport();
    }
}

void ExportCsvFileDialog::finishExport()
{
    exportTimer->stop();

    QString filePath = QString::fromStdString(csvExport->getFileName());
    CsvOutlinesExport::Status status = csvExport->getStatus();
    csvExport.reset();
    setExportRunning(false);

    switch(status) {
    case CsvOutlinesExport::Status::FINISHED:
        QDialog::accept();
        emit signalExportFinished(
            true, tr("Export to CSV file '%1' successfully finished").arg(filePath));
        break;
    case CsvOutlinesExport::Status::CANCELLED:
        QDialog::reject();
        emit signalExportFinished(
            false, tr("Export to CSV file '%1' cancelled").arg(filePath));
        break;
    default:
        // keep the dialog open so that the user can fix the target path
        QMessageBox::critical(
            this,
            tr("Export Error"),
            tr("Unable to write CSV file '%1'!").arg(filePath));
        emit signalExportFinished(
            false, tr("Export to CSV file '%1' failed").arg(filePath));
    }
}

void ExportCsvFileDialog::reject()
{
    if(isExportRunning()) {
        csvExport->cancel();
        finishExport();
    } else {
        QDialog::reject();
    }
}

void ExportCsvFileDialog::setExportRunning(bool running)
{
    fileNameEdit->setEnabled(!running);
    dirEdit->setEnabled(!running);
    findDirectoryButton->setEnabled(!running);
    oheTagsCheck->setEnabled(!running);
    oheTagsCardinalitySpin->setEnabled(!running && oheTagsCheck->isChecked());
    exportButton->setEnabled(!running);
    progressBar->setVisible(running);
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
