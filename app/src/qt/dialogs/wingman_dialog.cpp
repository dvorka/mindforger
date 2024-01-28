/*
 wingman_dialog.cpp     MindForger thinking notebook

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
#include "wingman_dialog.h"

namespace m8r {

using namespace std;

const string COLOR_PROMPT_GREEN{"#00bb00"};
const string COLOR_PROMPT_BLUE{"#00aaaa"};
const string COLOR_PROMPT_GRAY{"#777777"};

WingmanDialog::WingmanDialog(QWidget* parent)
: QDialog(parent),
  firstRun{true},
  mode{WingmanDialogModes::WINGMAN_DIALOG_MODE_TEXT},
  context{},
  lastAnswer{}
{
    setWindowTitle(tr("Wingman Chat"));

    promptsLabel = new QLabel{tr("Prompt:"), parent};

    predefinedPromptsCombo = new QComboBox{this};
    predefinedPromptsCombo->hide();

    cmdEdit = new MyLineEdit(this, this);
    cmdEdit->setToolTip(
        tr(
            "Use 'clear' or 'cls' to clear the chat window."
            " Use 'exit', 'quit' or 'bye' to close the dialog."));
    QPalette p = cmdEdit->palette();
    p.setColor(QPalette::Text, QColor(COLOR_PROMPT_GREEN.c_str()));
    cmdEdit->setPalette(p);

    cmdCompleter = new QCompleter(new QStandardItemModel(cmdEdit), this);
    cmdCompleter->setCompletionMode(QCompleter::CompletionMode::InlineCompletion);
    cmdCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QStandardItemModel* cmdCompleterModel =
        dynamic_cast<QStandardItemModel*>(cmdCompleter->model());
    if(cmdCompleterModel) {
        completerCommands.clear();
        // prompts based on the context

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

    askButton = new QPushButton(tr("&Run Prompt"), this);
    copyButton = new QPushButton(tr("C&opy"), this);
    copyButton->setToolTip(tr("Copy last answer to the clipboard"));
    togglePromptSourceButton = new QPushButton(tr("Show Predefined &Prompts"), this);
    toggleContextButton = new QPushButton(tr("Show &Context"), this);
    closeButton = new QPushButton(tr("Close"), this);

    // GROUP: context
    contextGroup = new QGroupBox{tr("Context"), this};

    contextTypeLabel = new QLabel{tr("Type:"), parent};
    contextTypeEdit = new QLineEdit{parent};
    contextTypeEdit->setReadOnly(true);

    contextNameLabel = new QLabel{tr("Name (#NAME):"), parent};
    contextNameEdit = new QLineEdit{parent};
    contextNameEdit->setReadOnly(true);

    contextLabel = new QLabel{tr("Text (#TEXT):"), parent};
    contextEdit = new QLineEdit{parent};
    contextEdit->setReadOnly(true);

    progressBar = new QProgressBar(this);
    progressBar->setTextVisible(true);
    progressBar->setMinimum(0);
    progressBar->setMaximum(100);
    progressBar->hide();

    QHBoxLayout* contentLayout = new QHBoxLayout{this};
    QVBoxLayout* contentLayout1 = new QVBoxLayout{this};
    contentLayout1->addWidget(contextNameLabel);
    contentLayout1->addWidget(contextNameEdit);
    contentLayout->addLayout(contentLayout1);
    QVBoxLayout* contentLayout2 = new QVBoxLayout{this};
    contentLayout2->addWidget(contextLabel);
    contentLayout2->addWidget(contextEdit);
    contentLayout->addLayout(contentLayout2);
    QVBoxLayout* contentLayout3 = new QVBoxLayout{this};
    contentLayout3->addWidget(contextTypeLabel);
    contentLayout3->addWidget(contextTypeEdit);
    contentLayout->addLayout(contentLayout3);
    contextGroup->setLayout(contentLayout);
    contextGroup->hide();

    // assembly
    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    //buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(toggleContextButton);
    buttonLayout->addWidget(togglePromptSourceButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(copyButton);
    buttonLayout->addWidget(askButton);
    buttonLayout->addStretch();

    QVBoxLayout* layout = new QVBoxLayout{this};
    layout->addWidget(chatWindow);
    layout->addWidget(promptsLabel);
    layout->addWidget(predefinedPromptsCombo);
    layout->addWidget(cmdEdit);
    layout->addWidget(progressBar);
    layout->addLayout(buttonLayout);
    layout->addWidget(contextGroup);
    setLayout(layout);

    resize(fontMetrics().averageCharWidth()*100, fontMetrics().height()*35);

    this->initForMode(mode);
    chatWindow->clear();

    // signals
    QObject::connect(
        closeButton, SIGNAL(clicked()),
        this, SLOT(close()));
    QObject::connect(
        togglePromptSourceButton, SIGNAL(clicked()),
        this, SLOT(handleTogglePromptSource()));
    QObject::connect(
        toggleContextButton, SIGNAL(clicked()),
        this, SLOT(handleToggleContextGroup()));
    QObject::connect(
        askButton, SIGNAL(clicked()),
        this, SLOT(handleRunPrompt()));
    QObject::connect(
        copyButton, SIGNAL(clicked()),
        this, SLOT(handleCopyLastAnswer()));
}

WingmanDialog::~WingmanDialog()
{
    delete promptsLabel;
    delete predefinedPromptsCombo;
    delete cmdEdit;
    delete chatWindow;
    delete askButton;
    delete togglePromptSourceButton;
    delete toggleContextButton;
    delete closeButton;
    delete contextGroup; // deletes children automatically
}

void WingmanDialog::initForMode(WingmanDialogModes mode)
{
    this->mode=mode;

    predefinedPromptsCombo->clear();
    vector<QString>* prompts{};

    switch(mode) {
        case WingmanDialogModes::WINGMAN_DIALOG_MODE_OUTLINE:
            prompts = &outlinePrompts;
            contextTypeEdit->setText(tr("notebook"));
            contextEdit->setText(tr("<Notebook text>"));
            break;
        case WingmanDialogModes::WINGMAN_DIALOG_MODE_NOTE:
            prompts = &notePrompts;
            contextTypeEdit->setText(tr("note"));
            contextEdit->setText(tr("<Note text>"));
            break;
        case WingmanDialogModes::WINGMAN_DIALOG_MODE_TEXT:
            prompts = &textPrompts;
            contextNameEdit->clear();
            contextEdit->clear();
            break;
    }

    for(QString prompt:*prompts) {
        predefinedPromptsCombo->addItem(prompt);
    }
}

void WingmanDialog::setContextText(QString context) {
    this->context=context;

    int limit{25};
    if(context.length()>limit) {
        context=context.mid(0, limit).append("...");
    }
    this->contextEdit->setText(context);
}

QString WingmanDialog::getContextText() const {
    return context;
}

void WingmanDialog::show(
    WingmanDialogModes contextType,
    QString& contextName,
    QString& context,
    const vector<string>& predefinedOPrompts,
    const vector<string>& predefinedNPrompts,
    const vector<string>& predefinedTPrompts
) {
    outlinePrompts.clear();
    for(string prompt:predefinedOPrompts) {
        outlinePrompts.push_back(QString::fromStdString(prompt));
    }
    notePrompts.clear();
    for(string prompt:predefinedNPrompts) {
        notePrompts.push_back(QString::fromStdString(prompt));
    }
    textPrompts.clear();
    for(string prompt:predefinedTPrompts) {
        textPrompts.push_back(QString::fromStdString(prompt));
    }

    this->initForMode(contextType);

    setContextText(context);
    setContextNameText(contextName);

    cmdEdit->clear();
    cmdEdit->setFocus();

    setModal(true);
    QDialog::show();
}

std::string WingmanDialog::getPrompt()
{
    if(predefinedPromptsCombo->isVisible()) {
        return predefinedPromptsCombo->currentText().toStdString();
    } else {
        return cmdEdit->text().toStdString();
    }
}

string WingmanDialog::getChatPromptPrefix(bool error)
{
    string prompt{};

    if(!firstRun) {
        prompt.append("<hr/>");
    } else {
        firstRun=false;
    }

    if(contextTypeEdit->text().size()>0) {
        string promptDescriptor{
            "[context type: " + contextTypeEdit->text().toStdString() +
            ", context name: " + contextNameEdit->text().toStdString() +
            "]"
        };

        prompt.append(
            "<font color='" + COLOR_PROMPT_GRAY + "'>" +
            promptDescriptor +
            "</font>" +
            "<br/>"
        );
    }

    prompt.append("<font color='");
    if(error) {
        prompt.append("#ff0000");
    } else {
        prompt.append(COLOR_PROMPT_GREEN);
    }
    prompt.append("'>&gt;</font> ");

    return prompt;
}

void WingmanDialog::appendPromptToChat(const std::string& prompt)
{
    chatWindow->insertHtml(
        QString::fromStdString(
            getChatPromptPrefix(
                false)) +
        QString::fromStdString(
            "<font color='"+COLOR_PROMPT_GREEN+"'>" +
            prompt +
            "</font>"
            "<br/><br/>"
        ));

    chatWindow->moveCursor(QTextCursor::End);
    chatWindow->ensureCursorVisible();
}

void WingmanDialog::appendAnswerToChat(
    const string& answer,
    const string& answerDescriptor,
    const WingmanDialogModes& contextType,
    bool error
) {
    this->lastAnswer=answer;

    string contextTypeString;
    switch(contextType) {
        case WingmanDialogModes::WINGMAN_DIALOG_MODE_OUTLINE:
            contextTypeString = "outline";
            break;
        case WingmanDialogModes::WINGMAN_DIALOG_MODE_NOTE:
            contextTypeString = "note";
            break;
        default:
            contextTypeString = "text";
            break;
    }

    const string errorPrefix{"<font color='#ff0000'>ERROR: "};
    const string errorSuffix{"</font>"};

    chatWindow->insertHtml(
        QString::fromStdString(
            "<br/>"
            "<b>"
            "<font color='" + COLOR_PROMPT_BLUE + "'>"
            "wingman@" + contextTypeString +
            "</font>" +
            "</b>:"
            "<br/>"
            "<br/>" +
            (error?errorPrefix:"") +
            answer +
            (error?errorSuffix:"") +
            "<br/>" +
            "<br/>" +
            "<font color='" + COLOR_PROMPT_GRAY + "'>" +
            answerDescriptor +
            "</font>" +
            "<br/>"
        ));

    chatWindow->moveCursor(QTextCursor::End);
    chatWindow->ensureCursorVisible();
}

void WingmanDialog::runPrompt()
{
    // TODO help
    if(cmdEdit->text() == QString::fromStdString("clear")
        || cmdEdit->text() == QString::fromStdString("cls")
    ) {
        chatWindow->clear();
    } else if(cmdEdit->text() == QString::fromStdString("exit")
        || cmdEdit->text() == QString::fromStdString("quit")
        || cmdEdit->text() == QString::fromStdString("bye")
    ) {
        QDialog::close();
    } else {
        string prompt = getPrompt();

        appendPromptToChat(prompt);

        // add command to completer
        QStandardItemModel* completerModel
            = dynamic_cast<QStandardItemModel*>(cmdCompleter->model());
        if(!completerModel) {
            completerModel = new QStandardItemModel();
        }
        completerModel->insertRow(
            0, new QStandardItem(cmdEdit->text())
        );

        // run prompt by sending SIGNAL to main window presenter
        MF_DEBUG("RUNNING prompt from the CHAT dialog: '" << prompt << "'" << endl);
        emit signalRunWingman();
        MF_DEBUG("SIGNAL to RUN prompt sent" << endl);
    }

    cmdEdit->clear();
    MF_DEBUG("Chat prompt dispatched" << endl);
}

void WingmanDialog::handleRunPrompt()
{
    MF_DEBUG("SLOT handle: run" << endl);

    runPrompt();
}

void WingmanDialog::handleCopyLastAnswer()
{
    MF_DEBUG("SLOT handle: copy" << endl);

    // get a pointer to the clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(QString::fromStdString(this->lastAnswer));
}

void WingmanDialog::handleTogglePromptSource()
{
    MF_DEBUG("SLOT handle: toggle" << endl);

    if(predefinedPromptsCombo->isVisible()) {
        predefinedPromptsCombo->hide();
        promptsLabel->setText(tr("Prompt:"));
        cmdEdit->show();
        cmdEdit->setFocus();
        togglePromptSourceButton->setText(tr("Show &Predefined Prompts"));
    } else {
        cmdEdit->hide();
        promptsLabel->setText(tr("Predefined prompts:"));
        predefinedPromptsCombo->show();
        predefinedPromptsCombo->setFocus();
        togglePromptSourceButton->setText(tr("Write Your &Prompt"));
    }
}

void WingmanDialog::handleToggleContextGroup()
{
    MF_DEBUG("SLOT handle: toggle context" << endl);

    if(contextGroup->isVisible()) {
        contextGroup->hide();
        toggleContextButton->setText(tr("Show &Context"));
    } else {
        contextGroup->show();
        toggleContextButton->setText(tr("Hide &Context"));
    }
}

} // m8r namespace
