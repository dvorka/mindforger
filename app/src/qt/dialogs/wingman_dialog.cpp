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

WingmanDialog::WingmanDialog(
    const vector<string>& predefinedOPrompts,
    const vector<string>& predefinedNPrompts,
    const vector<string>& predefinedTPrompts,
    QWidget* parent
):
    QDialog(parent),
    context{}
{
    for(string prompt:predefinedOPrompts) {
        outlinePrompts.push_back(QString::fromStdString(prompt));
    }
    for(string prompt:predefinedNPrompts) {
        notePrompts.push_back(QString::fromStdString(prompt));
    }
    for(string prompt:predefinedTPrompts) {
        textPrompts.push_back(QString::fromStdString(prompt));
    }

    // UI
    setWindowTitle(tr("Wingman"));

    preludeLabel = new QLabel{
        tr(
            "Wingman can run a predefined or custom prompt"
            " "
            "with the selected context."
            "<br>"
        ),
        parent
    };

    // GROUP: prompts
    QGroupBox* promptsGroup = new QGroupBox{tr("Prompt"), this};
    QVBoxLayout* promptsLayout = new QVBoxLayout{this};

    predefinedPromptsLabel = new QLabel{tr("Predefined:"), parent};
    predefinedPromptsCombo = new QComboBox{this};
    for(QString toolName:outlinePrompts) {
        predefinedPromptsCombo->addItem(toolName);
    }

    promptLabel = new QLabel{
        tr("Your (overrides Predefined, use #NAME or #TEXT to include context):"),
        parent};
    promptEdit = new QTextEdit{parent};
    promptEdit->setToolTip(
        tr("Type in your prompt like: 'Translate the following text to Spanish: #CONTENT."));

    promptsLayout->addWidget(predefinedPromptsLabel);
    promptsLayout->addWidget(predefinedPromptsCombo);
    promptsLayout->addWidget(promptLabel);
    promptsLayout->addWidget(promptEdit);
    promptsGroup->setLayout(promptsLayout);

    // GROUP: content
    QGroupBox* contentGroup = new QGroupBox{tr("Context"), this};
    QVBoxLayout* contentLayout = new QVBoxLayout{this};

    contextTypeLabel = new QLabel{tr("Type:"), parent};
    contextTypeEdit = new QLineEdit{parent};
    contextTypeEdit->setReadOnly(true);

    contextNameLabel = new QLabel{tr("Name (#NAME):"), parent};
    contextNameEdit = new QLineEdit{parent};
    contextNameEdit->setReadOnly(true);

    contextLabel = new QLabel{tr("Text (#TEXT):"), parent};
    contextEdit = new QLineEdit{parent};
    contextEdit->setReadOnly(true);

    contentLayout->addWidget(contextNameLabel);
    contentLayout->addWidget(contextNameEdit);
    contentLayout->addWidget(contextLabel);
    contentLayout->addWidget(contextEdit);
    contentLayout->addWidget(contextTypeLabel);
    contentLayout->addWidget(contextTypeEdit);
    contentGroup->setLayout(contentLayout);

    // IMPROVE disable/enable find button if text/path is valid: freedom vs validation
    runButton = new QPushButton{tr("&Ask Wingman")};
    runButton->setDefault(true);
    closeButton = new QPushButton{tr("&Cancel")};

    // signals
    QObject::connect(
        closeButton, SIGNAL(clicked()),
        this, SLOT(close()));

    // assembly
    QVBoxLayout* mainLayout = new QVBoxLayout{};

    QHBoxLayout* buttonLayout = new QHBoxLayout{};
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(runButton);
    buttonLayout->addStretch();

    mainLayout->addWidget(preludeLabel);
    mainLayout->addWidget(promptsGroup);
    mainLayout->addWidget(contentGroup);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // dialog
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

WingmanDialog::~WingmanDialog()
{
    delete preludeLabel;

    delete predefinedPromptsLabel;
    delete predefinedPromptsCombo;
    delete promptLabel;
    delete promptEdit;

    delete contextTypeLabel;
    delete contextTypeEdit;
    delete contextNameLabel;
    delete contextNameEdit;
    delete contextLabel;
    delete contextEdit;

    delete runButton;
    delete closeButton;
}

void WingmanDialog::clear()
{
    this->context.clear();

    this->promptEdit->clear();
    this->contextNameEdit->clear();
    this->contextEdit->clear();
}

void WingmanDialog::initForMode(WingmanDialogModes mode)
{
    this->mode=mode;

    switch(mode) {
        case WingmanDialogModes::WINGMAN_DIALOG_MODE_OUTLINE:
            contextTypeEdit->setText(tr("outline"));
            contextEdit->setText(tr("<Notebook text>"));
            break;
        case WingmanDialogModes::WINGMAN_DIALOG_MODE_NOTE:
            contextTypeEdit->setText(tr("note"));
            contextEdit->setText(tr("<Note text>"));
            break;
        case WingmanDialogModes::WINGMAN_DIALOG_MODE_TEXT:
            contextNameEdit->clear();
            contextEdit->clear();
            break;
    }
}

void WingmanDialog::setContextText(QString context) {
    this->context=context;
    this->contextEdit->setText(context.mid(0, 50).append("..."));
}

QString WingmanDialog::getContextText() const {
    return context;
}

void WingmanDialog::show()
{
    QDialog::show();
}

} // m8r namespace
