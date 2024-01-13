/*
 wingman_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_WINGMAN_DIALOG_H
#define M8RUI_WINGMAN_DIALOG_H

#include <vector>
#include <string>

#include <QtWidgets>

#include "../../lib/src/config/configuration.h"

namespace m8r {

enum WingmanDialogModes {
    WINGMAN_DIALOG_MODE_OUTLINE,
    WINGMAN_DIALOG_MODE_NOTE,
    WINGMAN_DIALOG_MODE_TEXT
};

class WingmanDialog : public QDialog
{
    Q_OBJECT

private:
    std::vector<QString> outlinePrompts;
    std::vector<QString> notePrompts;
    std::vector<QString> textPrompts;

    WingmanDialogModes mode;

    QString context;

    QLabel* preludeLabel;

    QLabel* predefinedPromptsLabel;
    QComboBox* predefinedPromptsCombo;

    QLabel* promptLabel;
    QTextEdit* promptEdit;

    QLabel* contextTypeLabel;
    QLineEdit* contextTypeEdit;
    QLabel* contextNameLabel;
    QLineEdit* contextNameEdit;
    QLabel* contextLabel;
    QLineEdit* contextEdit;

    QPushButton* runButton;
    QPushButton* closeButton;

public:
    explicit WingmanDialog(
        const std::vector<std::string>& predefinedOPrompts,
        const std::vector<std::string>& predefinedNPrompts,
        const std::vector<std::string>& predefinedTPrompts,
        QWidget* parent);
    WingmanDialog(const WingmanDialog&) = delete;
    WingmanDialog(const WingmanDialog&&) = delete;
    WingmanDialog& operator =(const WingmanDialog&) = delete;
    WingmanDialog& operator =(const WingmanDialog&&) = delete;
    ~WingmanDialog();

    void clear();
    void initForMode(WingmanDialogModes mode);
    WingmanDialogModes getMode() const { return mode; }

    void setPromptText(QString phrase) {
        this->promptEdit->setText(phrase);
    }
    QString getPromptText() const {
        if(this->promptEdit->toPlainText().isEmpty()) {
            return predefinedPromptsCombo->currentText();
        }
        return this->promptEdit->toPlainText();
    }

    void setContextType(WingmanDialogModes contextType) {
        this->mode = contextType;
    }
    WingmanDialogModes getContextType() const {
        return this->mode;
    }
    void setContextNameText(QString contentName) {
        this->contextNameEdit->setText(contentName);
    }
    QString getContextNameText() const {
        return this->contextNameEdit->text();
    }
    void setContextText(QString context);
    QString getContextText() const;

    void show();

    QPushButton* getRunButton() const { return runButton; }
};

}
#endif // M8RUI_WINGMAN_DIALOG_H
