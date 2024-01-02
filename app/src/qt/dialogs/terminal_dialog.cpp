/*
 terminal_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "terminal_dialog.h"

namespace m8r {

using namespace std;

TerminalDialog::TerminalDialog(QWidget* parent)
    : QDialog(parent)
{
    cmdEdit = new MyLineEdit(this, this);

    cmdCompleter = new QCompleter(new QStandardItemModel(cmdEdit), this);
    cmdCompleter->setCompletionMode(QCompleter::CompletionMode::InlineCompletion);
    cmdCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QStandardItemModel* cmdCompleterModel =
        dynamic_cast<QStandardItemModel*>(cmdCompleter->model());
    if(cmdCompleterModel) {
        completerCommands.clear();
        completerCommands << QString::fromStdString("pwd");
        completerCommands << QString::fromStdString("git add .");
        completerCommands << QString::fromStdString("push origin HEAD");
        completerCommands << QString::fromStdString(
            "git add . && git commit -m 'Updates.' && git push origin HEAD"
        );
        completerCommands << QString::fromStdString(
            "cd ~/mf && git add . && git commit -m 'Updates.' && git push origin HEAD"
        );
        completerCommands << QString::fromStdString("echo ${MIDFORGER_REPOSITORY}");
        completerCommands << QString::fromStdString("mindforger --help");
#ifdef _WIN32
        completerCommands << QString::fromStdString("cls");
#else
        completerCommands << QString::fromStdString("clear");
#endif
        completerCommands << QString::fromStdString("exit");
        completerCommands << QString::fromStdString("quit");

        for(auto c:completerCommands) {
            cmdCompleterModel->appendRow(new QStandardItem(c));
        }
    }

    cmdEdit->setCompleter(cmdCompleter);

    terminalWindow = new QTextEdit(this);
    terminalWindow->setReadOnly(true);
    terminalWindow->clear();
    terminalWindow->insertHtml(QString::fromStdString(getPrompt()));

    QVBoxLayout* layout = new QVBoxLayout{this};
    layout->addWidget(terminalWindow);
    layout->addWidget(cmdEdit);
    setLayout(layout);

    resize(fontMetrics().averageCharWidth()*100, fontMetrics().height()*35);
}

TerminalDialog::~TerminalDialog()
{
}

void TerminalDialog::show()
{
    // /home/dvorka/mf-devel
    // $ ls
    //   README.md
    // /home/dvorka/mf-devel
    // $
    //   ^ cursor

    cmdEdit->clear();
    cmdEdit->setFocus();

    setModal(true);
    QDialog::show();
}

string TerminalDialog::getPrompt(bool error)
{
    string prompt{"<font color='#bbbbbb'>" + getCwd() + "</font>"};
    prompt.append("<br/><font color='");
    if(error) {
        prompt.append("#ff0000");
    } else {
        prompt.append("#00ff00");
    }
    prompt.append("'>$</font> ");
    return prompt;
}

void TerminalDialog::runCommand()
{
    if(cmdEdit->text().size()) {
        if(cmdEdit->text() == QString::fromStdString("clear")
           || cmdEdit->text() == QString::fromStdString("cls")
        ) {
            terminalWindow->clear();
            terminalWindow->insertHtml(QString::fromStdString(getPrompt()));
        } else if(cmdEdit->text() == QString::fromStdString("exit")
          || cmdEdit->text() == QString::fromStdString("quit")
       ) {
            QDialog::close();
       } else {
            terminalWindow->insertHtml(
                cmdEdit->text() + QString::fromStdString("<br/><br/>")
            );

            string cmd{cmdEdit->text().toStdString()};

            // add command to completer
            QStandardItemModel* completerModel
                = dynamic_cast<QStandardItemModel*>(cmdCompleter->model());
            if(!completerModel) {
                completerModel = new QStandardItemModel();
            }
            completerModel->insertRow(
                0, new QStandardItem(cmdEdit->text())
            );

            // run command
            if(!system(NULL)) {
                string errorMessage{
                    "Error: unable to run terminal command as C++ command processor "
                    "is not available"
                };
                MF_DEBUG(errorMessage);
                QMessageBox::critical(
                    this,
                    tr("Terminal Command Error"),
                    tr(errorMessage.c_str())
                );
                return;
            }

            MF_DEBUG("Running terminal command: '" << cmd << "'" << endl);
            int statusCode{0};
            string cmdStdOut{};
#ifdef CMD_RUN_NO_OUTPUT
            // run command
            statusCode = system(cmd.c_str());
#else
            // run command AND catch STDOUT
            char buffer[128];
            // TODO _popen WINDOWS
    #ifdef _WIN32
            FILE* pipe = _popen(cmd.c_str(), "r");
    # else
            FILE* pipe = popen(cmd.c_str(), "r");
    #endif
            if(!pipe) {
                cmdStdOut.append(
                    "Error: unable to run command as popen() failed"
                );
                statusCode = 1;
            } else {
                try {
                    while(fgets(buffer, sizeof buffer, pipe) != NULL) {
                        cmdStdOut += buffer;
                    }
                } catch (...) {
                    cmdStdOut.append(
                        "Error: unable to run command as execution failed"
                    );
                }
#ifdef _WIN32
                statusCode = _pclose(pipe);
#else
                statusCode = pclose(pipe);
#endif
            }
#endif

            MF_DEBUG("Terminal command finished with status: " << statusCode << endl);
            terminalWindow->insertHtml(QString::fromStdString("<br/>"));

            if(cmdStdOut.size()) {
                replaceAll("\n", "<br/>", cmdStdOut);
                terminalWindow->insertHtml(
                    QString::fromStdString(cmdStdOut + "<br/>")
                );
            }

            if(statusCode) {
                cerr << "Terminal command failed with status: " << statusCode << endl;
                terminalWindow->insertHtml(
                    QString::fromStdString(getPrompt(true))
                );
            } else {
                terminalWindow->insertHtml(
                    QString::fromStdString(getPrompt())
                );
            }

            // scroll down by moving cursor to the end AND ensuring it's visible
            terminalWindow->moveCursor(QTextCursor::End);
            terminalWindow->ensureCursorVisible();
        }
    }

    cmdEdit->clear();
    MF_DEBUG("Terminal prompt cleared: " << cmdEdit->text().toStdString() << endl);
}

} // m8r namespace
