/*
add_library_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_ADD_LIBRARY_DIALOG_H
#define M8RUI_ADD_LIBRARY_DIALOG_H

#include <vector>

#include "../../lib/src/model/outline.h"
#include "../../lib/src/config/repository.h"

#include <QtWidgets>

namespace m8r {

/**
 * @brief Add document library dialog.
 *
 * The dialog is instantiated in main window presenter.
 */
class AddLibraryDialog : public QDialog
{
    Q_OBJECT

private:
    const Repository* activeRepository;
    const Outline* currentOutline;
    std::vector<Thing*> outlines;
    std::vector<Note*> notes;

    QLabel* findLibrarySourceLabel;
    QLabel* libraryNameLabel;
    QLineEdit* libraryNameEdit;
    QLabel* uriLabel;
    QLineEdit* uriEdit;

    QPushButton* findDirectoryButton;

    // TODO extensions
    QCheckBox* pdfCheckBox;

    QPushButton* createButton;
    QPushButton* closeButton;

public:
    explicit AddLibraryDialog(QWidget* parent);
    AddLibraryDialog(const AddLibraryDialog&) = delete;
    AddLibraryDialog(const AddLibraryDialog&&) = delete;
    AddLibraryDialog& operator=(const AddLibraryDialog&) = delete;
    AddLibraryDialog& operator=(const AddLibraryDialog&&) = delete;
    ~AddLibraryDialog();

    void show();

    QPushButton* getCreateButton() const { return createButton; }
    QString getLibraryNameText() const { return libraryNameEdit->text(); }
    QString getLibraryUriText() const { return uriEdit->text(); }
    bool isIndexPdf() const { return pdfCheckBox->isChecked(); }

private slots:
    void handleFindDirectory();
};

}
#endif // M8RUI_ADD_LIBRARY_DIALOG_H
