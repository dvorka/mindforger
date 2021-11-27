/*
 terminal_dialog.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8RUI_TERMINAL_DIALOG_H
#define M8RUI_TERMINAL_DIALOG_H


#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

#include "../../lib/src/debug.h"
#include "../../lib/src/gear/file_utils.h"
#include "../../lib/src/gear/string_utils.h"

#include <QtWidgets>

namespace m8r {

class TerminalDialog : public QDialog
{
    Q_OBJECT

    class MyLineEdit : public QLineEdit
    {
    private:
        TerminalDialog* terminalDialog;
    public:
        explicit MyLineEdit(TerminalDialog* terminalDialog, QWidget* parent)
            : QLineEdit(parent), terminalDialog(terminalDialog)
        {}
        virtual void keyPressEvent(QKeyEvent* event) override {
            switch(event->key()) {
            case Qt::Key_Return: // Qt::Key_Enter is keypad Enter
                terminalDialog->runCommand();
                break;
            }
            QLineEdit::keyPressEvent(event);
        }
    };

private:
    MyLineEdit* cmdEdit;
    QCompleter* cmdCompleter;
    QStringList completerCommands;

    QTextEdit* terminalWindow;

public:
    explicit TerminalDialog(QWidget* parent);
    TerminalDialog(const TerminalDialog&) = delete;
    TerminalDialog(const TerminalDialog&&) = delete;
    TerminalDialog& operator =(const TerminalDialog&) = delete;
    TerminalDialog& operator =(const TerminalDialog&&) = delete;
    ~TerminalDialog();

    void show();

private:
    void runCommand();
    std::string getPrompt(bool error=false);
};

}
#endif // M8RUI_TERMINAL_DIALOG_H
