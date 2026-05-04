/*
 new_repository_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_NEW_REPOSITORY_DIALOG_H
#define M8RUI_NEW_REPOSITORY_DIALOG_H

#include "../../lib/src/gear/string_utils.h"
#include "../../lib/src/gear/file_utils.h"

#include <QtWidgets>

namespace m8r {

class NewRepositoryDialog : public QDialog
{
    Q_OBJECT

private:
    QString homeDirectory;

    QLabel* repositoryNameLabel;
    QLineEdit* repositoryNameEdit;
    QLabel* dirLabel;
    QLineEdit* dirEdit;
    QLabel* pathLabel;
    QLineEdit* pathEdit;

    QPushButton* findDirectoryButton;

    QCheckBox* copyStencilsCheckbox;
    QCheckBox* copyDocCheckbox;

    QPushButton* newButton;
    QPushButton* closeButton;

public:
    explicit NewRepositoryDialog(QWidget* parent);
    NewRepositoryDialog(const NewRepositoryDialog&) = delete;
    NewRepositoryDialog(const NewRepositoryDialog&&) = delete;
    NewRepositoryDialog &operator=(const NewRepositoryDialog&) = delete;
    NewRepositoryDialog &operator=(const NewRepositoryDialog&&) = delete;
    ~NewRepositoryDialog();

    void show();
    QPushButton* getNewButton() const { return newButton; }
    QString getRepositoryPath() const { return pathEdit->text(); }
    bool isCopyDoc() const { return copyDocCheckbox->isChecked(); }
    bool isCopyStencils() const { return copyStencilsCheckbox->isChecked(); }

private slots:
    void refreshPath();
    void handleFindDirectory();
};

}
#endif // M8RUI_NEW_REPOSITORY_DIALOG_H
