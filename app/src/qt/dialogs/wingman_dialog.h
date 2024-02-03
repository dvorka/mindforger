/*
 chat_dialog.h     MindForger thinking notebook

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


#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

#include "../../lib/src/debug.h"
#include "../../lib/src/gear/file_utils.h"
#include "../../lib/src/gear/string_utils.h"

#include <QtWidgets>

namespace m8r {

enum WingmanDialogModes {
    WINGMAN_DIALOG_MODE_OUTLINE,
    WINGMAN_DIALOG_MODE_NOTE,
    WINGMAN_DIALOG_MODE_TEXT
};

class WingmanDialog : public QDialog // TODO rename to WingmanDialog
{
    Q_OBJECT

    class MyLineEdit : public QLineEdit
    {
    private:
        WingmanDialog* chatDialog;
    public:
        explicit MyLineEdit(WingmanDialog* chatDialog, QWidget* parent)
            : QLineEdit(parent),
              chatDialog(chatDialog)
        {}
        virtual void keyPressEvent(QKeyEvent* event) override {
            switch(event->key()) {
                case Qt::Key_Return: // Qt::Key_Enter is keypad Enter
                    chatDialog->runPrompt();
                    setFocus();
                    return;
            }

            // continue event dispatch (completer needs to get the event)
            QLineEdit::keyPressEvent(event);
        }
    };

private:
    std::vector<QString> outlinePrompts;
    std::vector<QString> notePrompts;
    std::vector<QString> textPrompts;

    bool firstRun;
    WingmanDialogModes mode;
    QString context;
    QString lastPromptLabel;
    std::string lastAnswer;

    QTextEdit* chatWindow;

    QLabel* promptsLabel;

    MyLineEdit* cmdEdit;
    QCompleter* cmdCompleter;
    QStringList completerCommands;

    QComboBox* predefinedPromptsCombo;

    QPushButton* askButton;
    QPushButton* copyButton;
    QPushButton* appendButton;
    QPushButton* replaceButton;

    QPushButton* togglePromptSourceButton;
    QPushButton* toggleContextButton;
    QPushButton* closeButton;

    QGroupBox* contextGroup;
    QLabel* contextTypeLabel;
    QLineEdit* contextTypeEdit;
    QLabel* contextNameLabel;
    QLineEdit* contextNameEdit;
    QLabel* contextLabel;
    QLineEdit* contextEdit;

    QProgressBar* progressBar;
    bool cmdEditVisibilityCache;
    bool predefPromptsVisibilityCache;

public:
    explicit WingmanDialog(QWidget* parent);
    WingmanDialog(const WingmanDialog&) = delete;
    WingmanDialog(const WingmanDialog&&) = delete;
    WingmanDialog& operator =(const WingmanDialog&) = delete;
    WingmanDialog& operator =(const WingmanDialog&&) = delete;
    ~WingmanDialog();

    void initForMode(WingmanDialogModes mode);
    WingmanDialogModes getMode() const { return mode; }
    void setMode(WingmanDialogModes mode) { this->mode=mode; }

    void appendPromptToChat(const std::string& prompt);
    void appendAnswerToChat(
        const std::string& answer,
        const std::string& answerDescriptor,
        const WingmanDialogModes& contextType,
        bool error=false);

    void setPromptsLabel(const QString& label) {
        lastPromptLabel = promptsLabel->text();
        promptsLabel->setText(label);
    }
    void setLastPromptLabel() {
        promptsLabel->setText(lastPromptLabel);
    }

    std::string getPrompt();
    void setPrompt(const std::string& prompt);
    void selectPrompt() {
        if(cmdEdit->isVisible() && !cmdEdit->text().isEmpty()) {
            cmdEdit->setFocus();
            cmdEdit->selectAll();
        }
    }
    std::string getLastAnswer() { return this->lastAnswer; }

    QPushButton* getAppendButton() const { return appendButton; }
    QPushButton* getReplaceButton() const { return replaceButton; }

    WingmanDialogModes getContextType() const {
        return this->mode;
    }
    void setContextNameText(QString contextName) {
        this->contextNameEdit->setText(contextName);
    }
    QString getContextNameText() const {
        return this->contextNameEdit->text();
    }
    void setContextText(QString context);
    QString getContextText() const;

    void setup(
        WingmanDialogModes contextType,
        QString& contextName,
        QString& context,
        const std::vector<std::string>& predefinedOPrompts,
        const std::vector<std::string>& predefinedNPrompts,
        const std::vector<std::string>& predefinedTPrompts);
    void show(
        WingmanDialogModes contextType,
        QString& contextName,
        QString& context,
        const std::vector<std::string>& predefinedOPrompts,
        const std::vector<std::string>& predefinedNPrompts,
        const std::vector<std::string>& predefinedTPrompts);
    void show() {
        setModal(true);
        QDialog::show();
    }

    void resetProgress(int maximum) {
        progressBar->setValue(0);
        progressBar->setMinimum(0);
        progressBar->setMaximum(maximum);
        progressBar->setValue(0);
    }
    void setProgressValue(int progress) {
        progressBar->setValue(progress);
    }
    void setProgressVisible(bool visible) {
        if(visible) {
            predefPromptsVisibilityCache = predefinedPromptsCombo->isVisible();
            cmdEditVisibilityCache = cmdEdit->isVisible();
            predefinedPromptsCombo->setVisible(false);
            cmdEdit->setVisible(false);
        } else {
            predefinedPromptsCombo->setVisible(predefPromptsVisibilityCache);
            cmdEdit->setVisible(cmdEditVisibilityCache);
        }

        progressBar->setVisible(visible);
    }

private:
    void runPrompt();
    std::string getChatPromptPrefix(bool error=false);

signals:
    void signalRunWingman();

private slots:
    void handleRunPrompt();
    void handleCopyLastAnswer();
    void handleTogglePromptSource();
    void handleToggleContextGroup();

};

}
#endif // M8RUI_WINGMAN_DIALOG_H
