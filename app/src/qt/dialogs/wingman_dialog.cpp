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

const vector<QString> WingmanDialog::outlinePrompts(
    {
        QString{"Summarize."},
        QString{"Generate tags from the text."},
        QString{"Find persons."},
        QString{"Find locations."},
        QString{"Find organizations."},
        QString{"Chat with the content."},
    }
);
const vector<QString> WingmanDialog::notePrompts(
    {
        QString{"Summarize."},
        QString{"Shorten."},
        QString{"Explain #NAME like I'm 5."},
        QString{"Generate tags."},
        QString{"Fix grammar."},
        QString{"Rewrite formally."},
        QString{"Rewrite informally."},
        QString{"Rewrite to be funny."},
        QString{"Chat with the content."},
        // other UCs:
        // - simplify
        // - beautify
        // - translate
        // - fix spelling
        // - fix style
        // - create plan ...
    }
);
const vector<QString> WingmanDialog::textPrompts(
    {
        QString{"Complete the text."},
        QString{"Complete the last text line."},
        QString{"Explain like I'm 5."},
        QString{"Fix grammar."},
        QString{"Generate tags."},
        QString{"Rewrite formally."},
        QString{"Rewrite informally."},
        QString{"Rewrite to Kafka style."},
    }
);

WingmanDialog::WingmanDialog(QWidget* parent)
    : QDialog(parent)
{
    // UI
    setWindowTitle(tr("Wingman"));

    preludeLabel = new QLabel{
        tr("Ask Wingman a predefined prompt or type your own phrase."),
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

    promptLabel = new QLabel{tr("Your:"), parent};
    promptEdit = new QLineEdit{parent};
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

    postmortemLabel = new QLabel{
        tr("Use #NAME or #TEXT to include it to your prompt."),
        parent};

    contentLayout->addWidget(contextTypeLabel);
    contentLayout->addWidget(contextTypeEdit);
    contentLayout->addWidget(contextNameLabel);
    contentLayout->addWidget(contextNameEdit);
    contentLayout->addWidget(contextLabel);
    contentLayout->addWidget(contextEdit);
    contentLayout->addWidget(postmortemLabel);
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

    delete postmortemLabel;

    delete runButton;
    delete closeButton;
}

void WingmanDialog::initForMode(WingmanDialogModes mode)
{
    this->mode=mode;

    switch(mode) {
        case WingmanDialogModes::WINGMAN_DIALOG_MODE_OUTLINE:
            contextTypeEdit->setText(tr("outline"));
            contextEdit->setText(tr("<Notebook document>"));
            break;
        case WingmanDialogModes::WINGMAN_DIALOG_MODE_NOTE:
            contextTypeEdit->setText(tr("note"));
            contextEdit->setText(tr("<Note description>"));
            break;
        case WingmanDialogModes::WINGMAN_DIALOG_MODE_TEXT:
            contextEdit->setText(tr("<selected / current text>"));
            break;
    }
}

void WingmanDialog::show()
{
    QDialog::show();
}

} // m8r namespace
