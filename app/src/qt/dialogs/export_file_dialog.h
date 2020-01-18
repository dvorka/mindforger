/*
 export_file_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_EXPORT_FILE_DIALOG_H
#define M8RUI_EXPORT_FILE_DIALOG_H

#include "new_file_dialog.h"

#include <QtWidgets>

namespace m8r {

class ExportFileDialog : public NewFileDialog
{
    Q_OBJECT

public:
    explicit ExportFileDialog(QString title, QString button, QString extension, QWidget* parent);
    ExportFileDialog(const ExportFileDialog&) = delete;
    ExportFileDialog(const ExportFileDialog&&) = delete;
    ExportFileDialog &operator=(const ExportFileDialog&) = delete;
    ExportFileDialog &operator=(const ExportFileDialog&&) = delete;
    ~ExportFileDialog();
};

}
#endif // M8RUI_EXPORT_FILE_DIALOG_H
