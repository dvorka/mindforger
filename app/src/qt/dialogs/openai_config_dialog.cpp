/*
 openai_config_dialog.cpp     MindForger thinking notebook

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
#include "openai_config_dialog.h"

namespace m8r {

using namespace std;

OpenAiConfigDialog::OpenAiConfigDialog(QWidget* parent)
    : QDialog(parent),
      config(Configuration::getInstance()),
      configValid(false),
      editMode(false)
{
    QLabel* apiKeyLabel = new QLabel(tr("API Key:"), this);
    apiKeyEdit = new QLineEdit(this);
    apiKeyEdit->setEchoMode(QLineEdit::Password);

    resetButton = new QPushButton(tr("Reset"), this);

    QHBoxLayout* apiKeyLayout = new QHBoxLayout();
    apiKeyLayout->addWidget(apiKeyEdit);
    apiKeyLayout->addWidget(resetButton);

    useEnvVarCheckbox = new QCheckBox(
        tr("Use environment variable %1").arg(ENV_VAR_OPENAI_API_KEY), this);
    envVarInfoLabel = new QLabel(
        tr("You can set env var: %1")
        .arg(ENV_VAR_OPENAI_API_KEY), this);
    envVarInfoLabel->setStyleSheet("QLabel { color: gray; font-size: small; }");

    QLabel* modelLabel = new QLabel(tr("LLM Model:"), this);
    llmModelCombo = new QComboBox(this);
    llmModelCombo->setEditable(true);
    llmModelCombo->addItem(LLM_MODEL_GPT35_TURBO);
    llmModelCombo->addItem(LLM_MODEL_GPT4);

    refreshModelsButton = new QPushButton(tr("Refresh"), this);

    QHBoxLayout* modelLayout = new QHBoxLayout();
    modelLayout->addWidget(llmModelCombo);
    modelLayout->addWidget(refreshModelsButton);

    QLabel* modelHelpLabel = new QLabel(
        tr("You can type model name or select from list"), this);
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
    mainLayout->addLayout(modelLayout);
    mainLayout->addWidget(modelHelpLabel);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    // signals
    QObject::connect(useEnvVarCheckbox, &QCheckBox::stateChanged, this, &OpenAiConfigDialog::handleEnvVarCheckbox);
    QObject::connect(resetButton, &QPushButton::clicked, this, &OpenAiConfigDialog::handleReset);
    QObject::connect(refreshModelsButton, &QPushButton::clicked, this, &OpenAiConfigDialog::handleRefresh);
    QObject::connect(probeButton, &QPushButton::clicked, this, &OpenAiConfigDialog::handleProbe);
    QObject::connect(addButton, &QPushButton::clicked, this, &OpenAiConfigDialog::handleAdd);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // dialog
    setWindowTitle(tr("Configure OpenAI Provider"));
    resize(fontMetrics().averageCharWidth()*60, height());
    setModal(true);
}

OpenAiConfigDialog::~OpenAiConfigDialog()
{
}

void OpenAiConfigDialog::setEditProvider(const LlmProviderConfig& provider)
{
    editMode = true;
    providerConfig = provider;
    configValid = provider.isValid;

    setWindowTitle(tr("Edit OpenAI Provider"));
    addButton->setText(tr("Save"));
}

void OpenAiConfigDialog::showEvent(QShowEvent* event)
{
    bool envVarExists = (std::getenv(ENV_VAR_OPENAI_API_KEY) != nullptr);
    useEnvVarCheckbox->setEnabled(envVarExists || (editMode && providerConfig.useEnvVar));
    if(!envVarExists && !(editMode && providerConfig.useEnvVar)) {
        useEnvVarCheckbox->setChecked(false);
        useEnvVarCheckbox->setToolTip(
            tr("Set %1 environment variable to enable this option.")
            .arg(ENV_VAR_OPENAI_API_KEY));
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
        llmModelCombo->setCurrentText(LLM_MODEL_GPT35_TURBO);
        configValid = false;
    }
    apiKeyEdit->setEnabled(!useEnvVarCheckbox->isChecked());

    QDialog::showEvent(event);
}

void OpenAiConfigDialog::handleEnvVarCheckbox(int state)
{
    bool useEnv = (state == Qt::Checked);
    apiKeyEdit->setEnabled(!useEnv);
    if(useEnv) {
        apiKeyEdit->clear();
    }
}

void OpenAiConfigDialog::handleReset()
{
    useEnvVarCheckbox->setChecked(false);
    apiKeyEdit->clear();
    apiKeyEdit->setEnabled(true);
    llmModelCombo->setCurrentText(LLM_MODEL_GPT35_TURBO);
}

void OpenAiConfigDialog::handleRefresh()
{
    // resolve effective API key: checkbox has priority over env var fallback
    string apiKey{};
    if(useEnvVarCheckbox->isChecked()) {
        const char* envKey = std::getenv(ENV_VAR_OPENAI_API_KEY);
        if(envKey) {
            apiKey = string(envKey);
        }
    } else {
        apiKey = apiKeyEdit->text().trimmed().toStdString();
    }

    if(apiKey.empty()) {
        QMessageBox::warning(
            this,
            tr("API Key Required"),
            tr("Please enter an API key or set the %1 environment variable before refreshing models.")
            .arg(ENV_VAR_OPENAI_API_KEY));
        return;
    }
    try {
        OpenAiWingman wingman{apiKey};
        const vector<string>& models = wingman.getModels();

        llmModelCombo->clear();
        for (const auto& model : models) {
            llmModelCombo->addItem(QString::fromStdString(model));
        }

        if (wingman.didLastListModelsSucceed()) {
            QMessageBox::information(
                this,
                tr("Models Refreshed"),
                tr("Successfully fetched %1 models from OpenAI API.").arg(models.size()));
        } else {
            QMessageBox::warning(
                this,
                tr("Refresh Failed"),
                tr("Could not connect to the OpenAI API with the given API key - "
                   "showing %1 default model(s) instead.").arg(models.size()));
        }
    } catch (const exception& e) {
        QMessageBox::critical(
            this,
            tr("Refresh Failed"),
            tr("Failed to fetch models from OpenAI API: %1").arg(e.what()));
    }
}

void OpenAiConfigDialog::handleProbe()
{
    string apiKey = useEnvVarCheckbox->isChecked()
        ? string{}
        : apiKeyEdit->text().trimmed().toStdString();
    string model = llmModelCombo->currentText().toStdString();
    string errorMessage;

    if (!config.validateOpenAiProviderInput(apiKey, model, errorMessage)) {
        QMessageBox::critical(
            this,
            tr("Configuration Invalid"),
            tr("OpenAI provider configuration is invalid: %1")
            .arg(QString::fromStdString(errorMessage)));
        configValid = false;
        return;
    }

    string effectiveApiKey = apiKey;
    if (useEnvVarCheckbox->isChecked()) {
        const char* envKey = std::getenv(ENV_VAR_OPENAI_API_KEY);
        if (envKey) {
            effectiveApiKey = string(envKey);
        }
    }

    // live check
    OpenAiWingman wingman{effectiveApiKey};
    if (wingman.didLastListModelsSucceed()) {
        QMessageBox::information(
            this,
            tr("Configuration Valid"),
            tr("OpenAI provider configuration is valid - successfully connected to the OpenAI API."));
        configValid = true;
    } else {
        QMessageBox::critical(
            this,
            tr("Configuration Invalid"),
            tr("Could not connect to the OpenAI API with the given API key. Please check the key and try again."));
        configValid = false;
    }
}

void OpenAiConfigDialog::handleAdd()
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
        providerConfig.id = "openai-" + to_string(timestamp);
    }
    providerConfig.displayName = "OpenAI " + model;
    providerConfig.providerType = WINGMAN_PROVIDER_OPENAI;
    providerConfig.apiKey = apiKey;  // empty when useEnvVar is true
    providerConfig.llmModel = model;
    providerConfig.isValid = configValid;
    providerConfig.useEnvVar = useEnv;

    accept();
}

}
