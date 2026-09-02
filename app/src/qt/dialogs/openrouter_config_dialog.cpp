/*
 openrouter_config_dialog.cpp     MindForger thinking notebook

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
#include "openrouter_config_dialog.h"

namespace m8r {

using namespace std;

OpenRouterConfigDialog::OpenRouterConfigDialog(QWidget* parent)
    : QDialog(parent),
      config(Configuration::getInstance()),
      configValid(false),
      editMode(false)
{
    QLabel* apiKeyLabel = new QLabel(tr("API Key:"), this);
    apiKeyEdit = new QLineEdit(this);
    apiKeyEdit->setEchoMode(QLineEdit::Password);
    apiKeyEdit->setPlaceholderText(tr("sk-or-..."));

    resetButton = new QPushButton(tr("Reset"), this);

    QHBoxLayout* apiKeyLayout = new QHBoxLayout();
    apiKeyLayout->addWidget(apiKeyEdit);
    apiKeyLayout->addWidget(resetButton);

    useEnvVarCheckbox = new QCheckBox(
        tr("Use environment variable %1").arg(ENV_VAR_OPENROUTER_API_KEY), this);
    envVarInfoLabel = new QLabel(
        tr("You can set env var: %1")
        .arg(ENV_VAR_OPENROUTER_API_KEY), this);
    envVarInfoLabel->setStyleSheet("QLabel { color: gray; font-size: small; }");

    QLabel* modelLabel = new QLabel(tr("LLM Model:"), this);
    llmModelCombo = new QComboBox(this);
    llmModelCombo->setEditable(true);

    llmModelCombo->addItem("openrouter/free");
    llmModelCombo->addItem("meta-llama/llama-3.3-70b-instruct:free");
    llmModelCombo->addItem("openai/gpt-oss-120b:free");
    llmModelCombo->addItem("nvidia/nemotron-3-nano-omni-30b-a3b-reasoning:free");
    llmModelCombo->addItem("openai/gpt-4");
    llmModelCombo->addItem("anthropic/claude-3-haiku");
    llmModelCombo->addItem("anthropic/claude-3-sonnet");
    llmModelCombo->addItem("google/gemini-pro");

    QLabel* modelHelpLabel = new QLabel(
        tr("Enter any model ID from <a href='https://openrouter.ai/models'>openrouter.ai/models</a>"), this);
    modelHelpLabel->setOpenExternalLinks(true);
    modelHelpLabel->setStyleSheet("QLabel { color: gray; font-size: small; }");

    // buttons
    probeButton = new QPushButton(tr("Probe"), this);
    addButton = new QPushButton(tr("Add"), this);
    addButton->setDefault(true);
    cancelButton = new QPushButton(tr("Cancel"), this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(probeButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(addButton);

    // main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(apiKeyLabel);
    mainLayout->addLayout(apiKeyLayout);
    mainLayout->addWidget(useEnvVarCheckbox);
    mainLayout->addWidget(envVarInfoLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(modelLabel);
    mainLayout->addWidget(llmModelCombo);
    mainLayout->addWidget(modelHelpLabel);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    // signals
    QObject::connect(useEnvVarCheckbox, &QCheckBox::stateChanged, this, &OpenRouterConfigDialog::handleEnvVarCheckbox);
    QObject::connect(resetButton, &QPushButton::clicked, this, &OpenRouterConfigDialog::handleReset);
    QObject::connect(probeButton, &QPushButton::clicked, this, &OpenRouterConfigDialog::handleProbe);
    QObject::connect(addButton, &QPushButton::clicked, this, &OpenRouterConfigDialog::handleAdd);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // dialog
    setWindowTitle(tr("Configure OpenRouter Provider"));
    resize(fontMetrics().averageCharWidth()*60, height());
    setModal(true);
}

OpenRouterConfigDialog::~OpenRouterConfigDialog()
{
}

void OpenRouterConfigDialog::setEditProvider(const LlmProviderConfig& provider)
{
    editMode = true;
    providerConfig = provider;
    configValid = provider.isValid;

    setWindowTitle(tr("Edit OpenRouter Provider"));
    addButton->setText(tr("Save"));
}

void OpenRouterConfigDialog::showEvent(QShowEvent* event)
{
    bool envVarExists = (std::getenv(ENV_VAR_OPENROUTER_API_KEY) != nullptr);
    bool showEnvVarOption = envVarExists || (editMode && providerConfig.useEnvVar);
    useEnvVarCheckbox->setEnabled(showEnvVarOption);
    useEnvVarCheckbox->setVisible(showEnvVarOption);
    if(!showEnvVarOption) {
        useEnvVarCheckbox->setChecked(false);
        useEnvVarCheckbox->setToolTip(
            tr("Set %1 environment variable to enable this option.")
            .arg(ENV_VAR_OPENROUTER_API_KEY));
    } else {
        useEnvVarCheckbox->setToolTip(QString{});
    }

    if(editMode) {
        useEnvVarCheckbox->setChecked(providerConfig.useEnvVar);
        apiKeyEdit->setText(
            providerConfig.useEnvVar
            ? QString{}
            : QString::fromStdString(providerConfig.apiKey));
        llmModelCombo->setCurrentText(QString::fromStdString(providerConfig.llmModel));
    } else {
        apiKeyEdit->clear();
        configValid = false;
    }
    apiKeyEdit->setEnabled(!useEnvVarCheckbox->isChecked());

    QDialog::showEvent(event);
}

void OpenRouterConfigDialog::handleEnvVarCheckbox(int state)
{
    bool useEnv = (state == Qt::Checked);
    apiKeyEdit->setEnabled(!useEnv);
    if(useEnv) {
        apiKeyEdit->clear();
    }
}

void OpenRouterConfigDialog::handleReset()
{
    useEnvVarCheckbox->setChecked(false);
    apiKeyEdit->clear();
    apiKeyEdit->setEnabled(true);
    llmModelCombo->setCurrentIndex(0);
}

void OpenRouterConfigDialog::handleProbe()
{
    string apiKey = useEnvVarCheckbox->isChecked()
        ? string{}
        : apiKeyEdit->text().trimmed().toStdString();
    string model = llmModelCombo->currentText().toStdString();
    string errorMessage;

    if (!config.validateOpenRouterProviderInput(apiKey, model, errorMessage)) {
        QMessageBox::critical(
            this,
            tr("Configuration Invalid"),
            tr("OpenRouter provider configuration is invalid: %1")
            .arg(QString::fromStdString(errorMessage)));
        configValid = false;
        return;
    }

    string effectiveApiKey = apiKey;
    if (useEnvVarCheckbox->isChecked()) {
        const char* envKey = std::getenv(ENV_VAR_OPENROUTER_API_KEY);
        if (envKey) {
            effectiveApiKey = string(envKey);
        }
    }

    OpenRouterWingman wingman{effectiveApiKey};
    if (wingman.didLastListModelsSucceed()) {
        QMessageBox::information(
            this,
            tr("Configuration Valid"),
            tr("OpenRouter provider configuration is valid - successfully connected to the OpenRouter API."));
        configValid = true;
    } else {
        QMessageBox::critical(
            this,
            tr("Configuration Invalid"),
            tr("Could not connect to the OpenRouter API with the given API key. Please check the key and try again."));
        configValid = false;
    }
}

void OpenRouterConfigDialog::handleAdd()
{
    bool useEnv = useEnvVarCheckbox->isChecked();
    string apiKey = useEnv ? string{} : apiKeyEdit->text().trimmed().toStdString();
    string model = llmModelCombo->currentText().toStdString();

    if(!useEnv && apiKey.empty()) {
        QMessageBox::warning(
            this,
            tr("API Key Required"),
            tr("Please enter an API key or check the environment variable option."));
        return;
    }

    if(model.empty()) {
        QMessageBox::warning(
            this,
            tr("Model Required"),
            tr("Please select or enter a model name."));
        return;
    }

    if(!editMode) {
        auto now = chrono::system_clock::now();
        auto timestamp = chrono::duration_cast<chrono::seconds>(now.time_since_epoch()).count();
        providerConfig.id = "openrouter-" + to_string(timestamp);
    }
    providerConfig.displayName = "OpenRouter " + model;
    providerConfig.providerType = WINGMAN_PROVIDER_OPENROUTER;
    providerConfig.apiKey = apiKey;  // empty when useEnvVar is true
    providerConfig.llmModel = model;
    providerConfig.isValid = configValid;
    providerConfig.useEnvVar = useEnv;

    accept();
}

}
