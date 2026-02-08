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
      configValid(false)
{
    // API key field
    QLabel* apiKeyLabel = new QLabel(tr("API Key:"), this);
    apiKeyEdit = new QLineEdit(this);
    apiKeyEdit->setEchoMode(QLineEdit::Password);
    
    resetButton = new QPushButton(tr("Reset"), this);
    
    QHBoxLayout* apiKeyLayout = new QHBoxLayout();
    apiKeyLayout->addWidget(apiKeyEdit);
    apiKeyLayout->addWidget(resetButton);
    
    // environment variable info
    envVarInfoLabel = new QLabel(
        tr("Environment variable: %1<br>(if set, overrides the value above)")
        .arg(ENV_VAR_OPENAI_API_KEY), this);
    
    // LLM model combo (editable)
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
        tr("(You can type model name or select from list)"), this);
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
    mainLayout->addWidget(envVarInfoLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(modelLabel);
    mainLayout->addLayout(modelLayout);
    mainLayout->addWidget(modelHelpLabel);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
    
    // signals
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

void OpenAiConfigDialog::show()
{
    // load current config if any
    apiKeyEdit->setText(QString::fromStdString(config.getWingmanOpenAiApiKey()));
    llmModelCombo->setCurrentText(LLM_MODEL_GPT35_TURBO);
    configValid = false;
    
    QDialog::show();
}

void OpenAiConfigDialog::handleReset()
{
    apiKeyEdit->clear();
    llmModelCombo->setCurrentText(LLM_MODEL_GPT35_TURBO);
}

void OpenAiConfigDialog::handleRefresh()
{
    // validate API key is set
    string apiKey = apiKeyEdit->text().toStdString();
    if (apiKey.empty() && !config.canWingmanOpenAiFromEnv()) {
        QMessageBox::warning(
            this,
            tr("API Key Required"),
            tr("Please enter an API key or set the %1 environment variable before refreshing models.")
            .arg(ENV_VAR_OPENAI_API_KEY));
        return;
    }
    
    // temporarily set API key to fetch models
    string originalKey = config.getWingmanOpenAiApiKey();
    if (!apiKey.empty()) {
        config.setWingmanOpenAiApiKey(apiKey);
    }
    
    try {
        OpenAiWingman wingman;
        vector<string>& models = wingman.listModels();
        
        llmModelCombo->clear();
        for (const auto& model : models) {
            llmModelCombo->addItem(QString::fromStdString(model));
        }
        
        if (!models.empty()) {
            QMessageBox::information(
                this,
                tr("Models Refreshed"),
                tr("Successfully fetched %1 models from OpenAI API.").arg(models.size()));
        }
    } catch (const exception& e) {
        QMessageBox::critical(
            this,
            tr("Refresh Failed"),
            tr("Failed to fetch models from OpenAI API: %1").arg(e.what()));
    }
    
    // restore original key
    config.setWingmanOpenAiApiKey(originalKey);
}

void OpenAiConfigDialog::handleProbe()
{
    string apiKey = apiKeyEdit->text().toStdString();
    string model = llmModelCombo->currentText().toStdString();
    string errorMessage;
    
    if (config.probeOpenAiProvider(apiKey, model, errorMessage)) {
        QMessageBox::information(
            this,
            tr("Configuration Valid"),
            tr("OpenAI provider configuration is valid."));
        configValid = true;
    } else {
        QMessageBox::critical(
            this,
            tr("Configuration Invalid"),
            tr("OpenAI provider configuration is invalid: %1")
            .arg(QString::fromStdString(errorMessage)));
        configValid = false;
    }
}

void OpenAiConfigDialog::handleAdd()
{
    string apiKey = apiKeyEdit->text().toStdString();
    string model = llmModelCombo->currentText().toStdString();
    
    // validate inputs
    if (apiKey.empty() && !config.canWingmanOpenAiFromEnv()) {
        QMessageBox::warning(
            this,
            tr("API Key Required"),
            tr("Please enter an API key or set the %1 environment variable.")
            .arg(ENV_VAR_OPENAI_API_KEY));
        return;
    }
    
    if (model.empty()) {
        QMessageBox::warning(
            this,
            tr("Model Required"),
            tr("Please select or enter a model name."));
        return;
    }
    
    // generate unique ID using timestamp
    auto now = chrono::system_clock::now();
    auto timestamp = chrono::duration_cast<chrono::seconds>(now.time_since_epoch()).count();
    providerConfig.id = "openai-" + to_string(timestamp);
    providerConfig.displayName = "OpenAI " + model;
    providerConfig.providerType = WINGMAN_PROVIDER_OPENAI;
    providerConfig.apiKey = apiKey;
    providerConfig.llmModel = model;
    providerConfig.isValid = configValid;
    
    accept();
}

}
