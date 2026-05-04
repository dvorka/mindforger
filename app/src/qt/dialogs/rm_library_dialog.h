/*
rm_library_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_RM_LIBRARY_DIALOG_H
#define M8RUI_RM_LIBRARY_DIALOG_H

#include <vector>

#include "../../lib/src/model/outline.h"
#include "../../lib/src/config/repository.h"
#include "../../lib/src/mind/dikw/filesystem_information.h"

#include <QtWidgets>

namespace m8r {

/**
 * @brief Synchronize document library dialog.
 *
 * The dialog is instantiated in main window presenter.
 */
class RemoveLibraryDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel* findLibrarySourceLabel;
    QLabel* librariesLabel;
    QComboBox* libraryPathsCombo;

    QPushButton* rmButton;
    QPushButton* closeButton;

    std::vector<FilesystemInformationSource*> libsToRemove;

public:
    explicit RemoveLibraryDialog(QWidget* parent);
    RemoveLibraryDialog(const RemoveLibraryDialog&) = delete;
    RemoveLibraryDialog(const RemoveLibraryDialog&&) = delete;
    RemoveLibraryDialog& operator=(const RemoveLibraryDialog&) = delete;
    RemoveLibraryDialog& operator=(const RemoveLibraryDialog&&) = delete;
    ~RemoveLibraryDialog();

    void reset();
    std::string getLibraryMfPathForLocator(std::string& locator);
    void show();

    QPushButton* getRemoveButton() const { return rmButton; }
    QComboBox* getLibraryPathsCombo() const { return libraryPathsCombo; }
    void addLibraryToRemove(FilesystemInformationSource* libSrc);
};

}
#endif // M8RUI_RM_LIBRARY_DIALOG_H
