/*
 run_tool_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2023 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "run_tool_dialog.h"

namespace m8r {

using namespace std;

RunToolDialog::RunToolDialog(QWidget* parent)
    : QDialog(parent)
{
    vector<QString> toolNames  = {
        QString{TOOL_ARXIV},
        QString{TOOL_CHAT_GPT_WEB},
        QString{TOOL_DEEPL},
        QString{TOOL_DUCKDUCKGO},
        QString{TOOL_GOOGLE_BARD},
        QString{TOOL_GOOGLE_SEARCH},
        QString{TOOL_GH_REPOS},
        QString{TOOL_GH_TOPICS},
        QString{TOOL_WIKIPEDIA}
    };


    // UI

    phraseLabel = new QLabel{tr("Phrase:"), parent};
    phraseEdit = new QLineEdit{parent};

    toolLabel = new QLabel{tr("Knowledge source:"), parent};
    toolCombo = new QComboBox{this};
    for(QString toolName:toolNames) {
        toolCombo->addItem(toolName);
    }

    // TODO change of the tool changes the template

    templateLabel = new QLabel{tr("Template:"), parent};
    templateEdit = new QLineEdit{parent};

    // IMPROVE disable/enable find button if text/path is valid: freedom vs validation
    runButton = new QPushButton{tr("&Get")};
    runButton->setDefault(true);
    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(
        closeButton, SIGNAL(clicked()),
        this, SLOT(close()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};
    mainLayout->addWidget(phraseLabel);
    mainLayout->addWidget(phraseEdit);
    mainLayout->addWidget(toolLabel);
    mainLayout->addWidget(toolCombo);
    mainLayout->addWidget(templateLabel);
    mainLayout->addWidget(templateEdit);

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(runButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // signals
    QObject::connect(
        toolCombo, SIGNAL(currentIndexChanged(QString)),
        this, SLOT(handleChangeToolCombo(QString))
    );

    // dialog
    setWindowTitle(tr("Get Knowledge"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

RunToolDialog::~RunToolDialog()
{
}

void RunToolDialog::show()
{
    QDialog::show();
}

QString RunToolDialog::getTemplateTextForToolName(string selectedTool) const
{
    if(selectedTool == TOOL_ARXIV) {
        QString templateText{"https://arxiv.org/search/?query="};
        templateText.append(TOOL_PHRASE);
        return templateText;
    } else if(selectedTool == TOOL_DEEPL) {
        return QString{"https://www.deepl.com/en/translator"};
    } else if(selectedTool == TOOL_STACK_OVERFLOW) {
        QString templateText{"https://stackoverflow.com/search?q="};
        templateText.append(TOOL_PHRASE);
        return templateText;
    } else if(selectedTool == TOOL_DUCKDUCKGO) {
        QString templateText{"https://www.duckduckgo.com/?q="};
        templateText.append(TOOL_PHRASE);
        return templateText;
    } else if(selectedTool == TOOL_GH_TOPICS) {
        // TODO fix search URL
        QString templateText{"https://www.github.com/search?q="};
        templateText.append(TOOL_PHRASE);
        return templateText;
    } else if(selectedTool == TOOL_GH_REPOS) {
        // TODO fix search URL
        QString templateText{"https://www.github.com/search?q="};
        templateText.append(TOOL_PHRASE);
        return templateText;
    } else if(selectedTool == TOOL_CHAT_GPT_WEB) {
        return QString{"https://chat.openai.com/"};
    } else if(selectedTool == TOOL_GOOGLE_BARD) {
        return QString{"https://bard.google.com/chat"};
    } else if(selectedTool == TOOL_GOOGLE_SEARCH) {
        QString temlateText{"https://www.google.com/search?q="};
        temlateText.append(TOOL_PHRASE);
        return temlateText;
    } else if(selectedTool == TOOL_WIKIPEDIA) {
        // TODO: URL
        QString temlateText{"https://en.wikipedia.org/w/index.php?search="};
        temlateText.append(TOOL_PHRASE);
        return temlateText;
    }

    string msg{
        "Tool '" + selectedTool + "' to search/explain/process "
        "the phrase is not supported."};
    QMessageBox msgBox{
        QMessageBox::Critical,
        QObject::tr("Unsupported Knowledge Tool"),
        QObject::tr(msg.c_str()),
    };

    return QString{};
}

void RunToolDialog::handleChangeToolCombo(const QString& text) {
    MF_DEBUG("Tool changed: " << text.toStdString() << endl);

    this->templateEdit->setText(getTemplateTextForToolName(text.toStdString()));
}

} // m8r namespace
