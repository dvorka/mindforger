/*
 chat_dialog.cpp     MindForger thinking notebook

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
#include "chat_dialog.h"

namespace m8r {

using namespace std;

ChatDialog::ChatDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Wingman Chat"));

    cmdEdit = new MyLineEdit(this, this);

    cmdCompleter = new QCompleter(new QStandardItemModel(cmdEdit), this);
    cmdCompleter->setCompletionMode(QCompleter::CompletionMode::InlineCompletion);
    cmdCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QStandardItemModel* cmdCompleterModel =
        dynamic_cast<QStandardItemModel*>(cmdCompleter->model());
    if(cmdCompleterModel) {
        // prompts
        completerCommands.clear();
        completerCommands << QString::fromStdString("Explain like I'm five: ");
        completerCommands << QString::fromStdString("Create plan in bullet list for ");

        completerCommands << QString::fromStdString("exit");
        completerCommands << QString::fromStdString("quit");
        completerCommands << QString::fromStdString("bye");

        for(auto c:completerCommands) {
            cmdCompleterModel->appendRow(new QStandardItem(c));
        }
    }

    cmdEdit->setCompleter(cmdCompleter);

    chatWindow = new QTextEdit(this);
    chatWindow->setReadOnly(true);
    chatWindow->clear();
    chatWindow->insertHtml(QString::fromStdString(getPrompt()));

    QVBoxLayout* layout = new QVBoxLayout{this};
    layout->addWidget(chatWindow);
    layout->addWidget(cmdEdit);
    setLayout(layout);

    resize(fontMetrics().averageCharWidth()*100, fontMetrics().height()*35);
}

ChatDialog::~ChatDialog()
{
}

void ChatDialog::show()
{
    // > Summarize.                         [green]
    // 🤖 Lorem ipsum dolor sit amet,       [gray]
    // consectetur adipiscing elit.
    //
    // Explain like I'm 5: NLP.
    // 🤖 Lorem ipsum dolor sit amet,
    //
    // >
    //   ^ cursor

    cmdEdit->clear();
    cmdEdit->setFocus();

    setModal(true);
    QDialog::show();
}

string ChatDialog::getPrompt(bool error)
{
    string prompt{"<font color='#0000ff'>" + getCwd() + "</font>"};
    prompt.append("<br/><font color='");
    if(error) {
        prompt.append("#ff0000");
    } else {
        prompt.append("#00ff00");
    }
    prompt.append("'>&gt;</font> ");
    return prompt;
}

void ChatDialog::insertPrompt(const std::string& prompt)
{
    chatWindow->insertHtml(
        QString::fromStdString(
            "<font color='#00bb00'>"
            "&gt; " + prompt +
            "</font>"
        ));
    chatWindow->insertHtml(QString::fromStdString("<br/><br/>"));
}

void ChatDialog::insertOutput(const std::string& output)
{
    chatWindow->insertHtml(QString::fromStdString("🤖 " + output));
    chatWindow->insertHtml(QString::fromStdString("<br/><br/>"));
}

void ChatDialog::runCommand()
{
    if(cmdEdit->text().size()) {
        // TODO help
        if(cmdEdit->text() == QString::fromStdString("clear")
           || cmdEdit->text() == QString::fromStdString("cls")
        ) {
            chatWindow->clear();
            chatWindow->insertHtml(QString::fromStdString(getPrompt()));
        } else if(cmdEdit->text() == QString::fromStdString("exit")
          || cmdEdit->text() == QString::fromStdString("quit")
          || cmdEdit->text() == QString::fromStdString("bye")
       ) {
            QDialog::close();
       } else {
            chatWindow->insertHtml(
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

            // run prompt
            MF_DEBUG("Running prompt: '" << cmd << "'" << endl);
            int statusCode{0};
            string cmdStdOut{};

            // TODO run prompt
            // TODO run prompt
            // TODO run prompt

            MF_DEBUG("Chat command finished with status: " << statusCode << endl);
            chatWindow->insertHtml(QString::fromStdString("<br/>"));

            if(cmdStdOut.size()) {
                replaceAll("\n", "<br/>", cmdStdOut);
                chatWindow->insertHtml(
                    QString::fromStdString("🤖 " + cmdStdOut + "<br/>")
                );
            }

            if(statusCode) {
                cerr << "Chat command failed with status: " << statusCode << endl;
                chatWindow->insertHtml(
                    QString::fromStdString(getPrompt(true))
                );
            } else {
                chatWindow->insertHtml(
                    QString::fromStdString(getPrompt())
                );
            }

            // scroll down by moving cursor to the end AND ensuring it's visible
            chatWindow->moveCursor(QTextCursor::End);
            chatWindow->ensureCursorVisible();
        }
    }

    cmdEdit->clear();
    MF_DEBUG("Chat prompt cleared: " << cmdEdit->text().toStdString() << endl);
}

} // m8r namespace
