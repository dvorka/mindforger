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

const string COLOR_PROMPT_GREEN{"#00bb00"};
const string COLOR_PROMPT_BLUE{"#00aaaa"};
const string COLOR_PROMPT_GRAY{"#777777"};

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
    chatWindow->insertHtml(QString::fromStdString(getTerminalPrompt()));

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
    // Notebook/<name> OpenAI chatGPT3.5    [blue, yellow]
    // $ Summarize.                         [green]
    //
    // Lorem ipsum dolor sit amet,          [gray]
    // consectetur adipiscing elit.
    //
    // ~/notebook/<name> OpenAI chatGPT3.5
    // $ Explain 'NLP' like I'm 5.
    //
    // Lorem ipsum dolor sit amet,
    //
    // ~/notebook/<name> OpenAI chatGPT3.5
    // $
    //  ^ cursor

    cmdEdit->clear();
    cmdEdit->setFocus();

    setModal(true);
    QDialog::show();
}

string ChatDialog::getTerminalPrompt(bool error)
{
    string thing{"notebook"}; // TODO notebook / note
    string thingName{"My notebook"}; // TODO name based on ctx
    string wingmanModel{"OpenAI gpt-3"}; // TODO get provider from mind (virtual method @ base class)

    string prompt{
        "<hr/>"
        "<font color='" + COLOR_PROMPT_BLUE + "'>@" + thing + "</font> " +
        "<font color='" + COLOR_PROMPT_GREEN + "'><b>" + thingName + "</b></font>"
        "<br/>"
    };

    prompt.append("<font color='");
    if(error) {
        prompt.append("#ff0000");
    } else {
        prompt.append(COLOR_PROMPT_GREEN);
    }
    prompt.append("'>&gt;</font> ");
    return prompt;
}

void ChatDialog::insertPrompt(const std::string& prompt)
{
    chatWindow->insertHtml(
        QString::fromStdString(
            "<font color='"+COLOR_PROMPT_GREEN+"'>" +
            prompt +
            "</font>"
            "<br/><br/>"
        ));
    chatWindow->moveCursor(QTextCursor::End);
    chatWindow->ensureCursorVisible();
}

void ChatDialog::insertOutput(
    const string& output, const string& outputDescriptor, bool error)
{
    chatWindow->insertHtml(
        QString::fromStdString(
            "<br/>"
            "<b>Answer:</b>"
            " <font color='" + COLOR_PROMPT_GRAY + "'>" +
            outputDescriptor +
            "</font>" +
            "<br/>" +
            "<br/>" +
            output +
            "<br/>" +
            getTerminalPrompt(error)
        ));
    chatWindow->moveCursor(QTextCursor::End);
    chatWindow->ensureCursorVisible();
}

// TODO this code should be in a presenter, not in the view class
void ChatDialog::runCommand()
{
    if(cmdEdit->text().size()) {
        // TODO help
        if(cmdEdit->text() == QString::fromStdString("clear")
           || cmdEdit->text() == QString::fromStdString("cls")
        ) {
            chatWindow->clear();
            chatWindow->insertHtml(QString::fromStdString(getTerminalPrompt()));
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
            // TODO status bar
            string answerHtml{"Foo result Lorem ipsum dolor sit amet, consectetur adipiscing elit."};
            string answerDescriptor{"[foo model]"};
            int statusCode{0};

            // TODO run prompt
            // TODO run prompt
            // TODO run prompt

            MF_DEBUG("Chat command finished with status: " << statusCode << endl);
            if(answerHtml.size()) {
                // replaceAll("\n", "<br/>", cmdStdOut);
                this->insertOutput(
                    answerHtml,
                    answerDescriptor,
                    statusCode==0?false:true);
            }
        }
    }

    cmdEdit->clear();
    MF_DEBUG("Chat prompt cleared: " << cmdEdit->text().toStdString() << endl);
}

} // m8r namespace
