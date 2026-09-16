/*
 export_csv_file_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef M8RUI_EXPORT_CSV_FILE_DIALOG_H
#define M8RUI_EXPORT_CSV_FILE_DIALOG_H

#include <memory>

#include <QtWidgets>

#include "../../lib/src/config/configuration.h"
#include "../../lib/src/representations/csv/csv_outline_representation.h"

namespace m8r {

class ExportCsvFileDialog : public QDialog
{
    Q_OBJECT

    static constexpr const int DEFAULT_OHE_CARDINALITY = 3;
    // UI event loop is blocked by export for at most this time per slice
    static constexpr const int EXPORT_SLICE_MILLIS = 30;
    static constexpr const int EXPORT_ROWS_PER_STEP = 50;

protected:
    QString extension;
    QString homeDirectory;

protected:
    QLabel* fileNameLabel;
    QLineEdit* fileNameEdit;
    QLabel* dirLabel;
    QLineEdit* dirEdit;
    QLabel* pathLabel;
    QLineEdit* pathEdit;

    QPushButton* findDirectoryButton;

    QLabel* oheTagsLabel;
    QCheckBox* oheTagsCheck;
    QLabel* oheTagsCardinalityLabel;
    QSpinBox* oheTagsCardinalitySpin;

    QProgressBar* progressBar;

    QPushButton* exportButton;
    QPushButton* closeButton;

    std::unique_ptr<CsvOutlinesExport> csvExport;
    QTimer* exportTimer;

public:
    explicit ExportCsvFileDialog(QString title, QString button, QString extension, QWidget* parent);
    ExportCsvFileDialog(const ExportCsvFileDialog&) = delete;
    ExportCsvFileDialog(const ExportCsvFileDialog&&) = delete;
    ExportCsvFileDialog& operator =(const ExportCsvFileDialog&) = delete;
    ExportCsvFileDialog& operator =(const ExportCsvFileDialog&&) = delete;
    ~ExportCsvFileDialog();

    void show();
    QPushButton* getNewButton() const { return exportButton; }
    QString getFilePath() const { return pathEdit->text(); }
    bool isOheTags() const { return oheTagsCheck->isChecked(); }
    int getOheTagsCardinality() const { return oheTagsCardinalitySpin->value(); }

    /**
     * @brief Run export in UI event loop w/ progress and cancellation.
     *
     * Export is performed in short time slices driven by timer so that
     * UI is responsive. Dialog takes ownership of the export.
     */
    void runExport(CsvOutlinesExport* csvExport);
    bool isExportRunning() const { return csvExport != nullptr; }

public slots:
    /**
     * @brief Cancel running export or close the dialog (Cancel, ESC, window close).
     */
    void reject() override;

signals:
    void signalExportFinished(bool success, QString message);

private:
    void setExportRunning(bool running);
    void finishExport();

private slots:
    void handleExportSlice();
    void enableDisableOheCardinality(bool enable);
    void refreshPath();
    void handleFindDirectory();
};

}
#endif // M8RUI_EXPORT_CSV_FILE_DIALOG_H
