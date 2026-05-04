/*
 add_llm_provider_dialog.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "add_llm_provider_dialog.h"

namespace m8r {

using namespace std;

AddLlmProviderDialog::AddLlmProviderDialog(QWidget* parent)
    : QDialog(parent),
      selectedProviderType(WINGMAN_PROVIDER_NONE)
{
    questionLabel = new QLabel(tr("Which provider do you want to configure?"), this);

    providerTypeCombo = new QComboBox(this);
    providerTypeCombo->addItem(tr("ollama"), WINGMAN_PROVIDER_OLLAMA);
    providerTypeCombo->addItem(tr("OpenRouter"), WINGMAN_PROVIDER_OPENROUTER);
    providerTypeCombo->addItem(tr("OpenAI"), WINGMAN_PROVIDER_OPENAI);

    nextButton = new QPushButton(tr("Next >"), this);
    nextButton->setDefault(true);

    cancelButton = new QPushButton(tr("Cancel"), this);

    // layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(questionLabel);
    mainLayout->addWidget(providerTypeCombo);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(nextButton);

    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // signals
    QObject::connect(nextButton, &QPushButton::clicked, this, &AddLlmProviderDialog::handleNext);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // dialog
    setWindowTitle(tr("New LLM Provider"));
    resize(fontMetrics().averageCharWidth()*55, height());
    setModal(true);
}

AddLlmProviderDialog::~AddLlmProviderDialog()
{
}

void AddLlmProviderDialog::show()
{
    providerTypeCombo->setCurrentIndex(0);
    selectedProviderType = WINGMAN_PROVIDER_NONE;

    QDialog::show();
}

void AddLlmProviderDialog::handleNext()
{
    selectedProviderType = static_cast<WingmanLlmProviders>(
        providerTypeCombo->itemData(providerTypeCombo->currentIndex()).toInt());

    accept();
}

}
