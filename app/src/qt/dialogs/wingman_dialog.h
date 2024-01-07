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

    static const std::vector<QString> outlinePrompts;

    WingmanDialogModes mode;

    std::vector<QString> promptsOutline;
    std::vector<QString> promptsNote;
    std::vector<QString> promptsText;

    QLabel* phraseLabel;
    QLineEdit* phraseEdit;

    QLabel* toolLabel;
    QComboBox* toolCombo;

    QLabel* templateLabel;
    QLineEdit* templateEdit;

    QPushButton* runButton;
    QPushButton* closeButton;

public:
    explicit WingmanDialog(QWidget* parent);
    WingmanDialog(const WingmanDialog&) = delete;
    WingmanDialog(const WingmanDialog&&) = delete;
    WingmanDialog& operator =(const WingmanDialog&) = delete;
    WingmanDialog& operator =(const WingmanDialog&&) = delete;
    ~WingmanDialog();

    void initForMode(WingmanDialogModes mode);

    void show();

    QString getTemplateTextForToolName(std::string selectedTool) const;

    QPushButton* getRunButton() const { return runButton; }
    QString getSelectedTool() const {
        return this->toolCombo->itemText(
            this->toolCombo->currentIndex()
        );
    }
    QString getTemplateText() const {
        return this->templateEdit->text();
    }
    QString getPhraseText() const { return phraseEdit->text(); }
    void setPhraseText(QString phrase) {
        this->phraseEdit->setText(phrase);
    }
    void setTemplateText(QString templateText) {
        this->templateEdit->setText(templateText);
    }

private slots:
    void handleChangeToolCombo(const QString& text);
};

}
#endif // M8RUI_WINGMAN_DIALOG_H
