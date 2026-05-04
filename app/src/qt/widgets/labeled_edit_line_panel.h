/*
 labeled_edit_line_panel.h     MindForger thinking notebook

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
#ifndef M8RUI_LABELED_EDIT_LINE_PANEL_H
#define M8RUI_LABELED_EDIT_LINE_PANEL_H

#include <QtWidgets>

namespace m8r {

class LabeledEditLinePanel : public QWidget
{
    Q_OBJECT

private:
    QLabel* label;
    QLineEdit* edit;

public:
    explicit LabeledEditLinePanel(const QString& label, QWidget* parent);
    LabeledEditLinePanel(const LabeledEditLinePanel&) = delete;
    LabeledEditLinePanel(const LabeledEditLinePanel&&) = delete;
    LabeledEditLinePanel &operator=(const LabeledEditLinePanel&) = delete;
    LabeledEditLinePanel &operator=(const LabeledEditLinePanel&&) = delete;
    ~LabeledEditLinePanel();

    void setText(const QString& text) { edit->setText(text); }
    void setEnabled(bool b) { edit->setEnabled(b); }
};

}
#endif // M8RUI_LABELED_EDIT_LINE_PANEL_H
