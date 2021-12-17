/*
 rows_and_depth_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_ROWS_AND_DEPTH_DIALOG_H
#define M8RUI_ROWS_AND_DEPTH_DIALOG_H

#include <QtWidgets>

namespace m8r {

class RowsAndDepthDialog : public QDialog
{
    Q_OBJECT

public:
    enum Purpose {
        BULLETS,
        NUMBERS,
        TASKS,
        BLOCKQUOTE
    };

private:
    Purpose purpose;

    QLabel* label;

    QSpinBox* rowsSpin;
    QSpinBox* depthSpin;

    QPushButton* generateButton;
    QPushButton* closeButton;

public:
    explicit RowsAndDepthDialog(QWidget* parent);
    RowsAndDepthDialog(const RowsAndDepthDialog&) = delete;
    RowsAndDepthDialog(const RowsAndDepthDialog&&) = delete;
    RowsAndDepthDialog &operator=(const RowsAndDepthDialog&) = delete;
    RowsAndDepthDialog &operator=(const RowsAndDepthDialog&&) = delete;
    ~RowsAndDepthDialog();

    void setPurpose(Purpose p) { purpose = p; }
    Purpose getPurpose() const { return purpose; }

    void show();

    QPushButton* getGenerateButton() { return generateButton; }
    int getRows() const { return rowsSpin->value(); }
    int getDepth() const { return depthSpin->value(); }
};

}
#endif // M8RUI_ROWS_AND_DEPTH_DIALOG_H
