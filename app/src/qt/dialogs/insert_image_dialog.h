/*
 insert_image_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_INSERT_IMAGE_DIALOG_H
#define M8RUI_INSERT_IMAGE_DIALOG_H

#include <QtWidgets>

namespace m8r {

class InsertImageDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel* alternateTextLabel;
    QLineEdit* alternateTextEdit;
    QLabel* pathLabel;
    QLineEdit* pathEdit;

    QPushButton* findFileButton;

    QCheckBox* copyToRepoCheckBox;

    QPushButton* insertButton;
    QPushButton* closeButton;

public:
    explicit InsertImageDialog(QWidget* parent);
    InsertImageDialog(const InsertImageDialog&) = delete;
    InsertImageDialog(const InsertImageDialog&&) = delete;
    InsertImageDialog &operator=(const InsertImageDialog&) = delete;
    InsertImageDialog &operator=(const InsertImageDialog&&) = delete;
    ~InsertImageDialog();

    void show();
    QPushButton* getInsertButton() const { return insertButton; }
    QString getAlternateText() { return alternateTextEdit->text(); }
    QString getPathText() { return pathEdit->text(); }

private slots:
    void handleFindFile();
};

}
#endif // M8RUI_INSERT_IMAGE_DIALOG_H
