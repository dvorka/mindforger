/*
 ollama_config_dialog.cpp     MindForger thinking notebook

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
#include "ollama_config_dialog.h"

namespace m8r {

using namespace std;

OllamaConfigDialog::OllamaConfigDialog(QWidget* parent)
    : QDialog(parent),
      config(Configuration::getInstance()),
      configValid(false)
{
    // URL field
    QLabel* urlLabel = new QLabel(tr("ollama Server URL:"), this);
    urlEdit = new QLineEdit(this);
    urlEdit->setText(DEFAULT_OLLAMA_URL);
    
    resetButton = new QPushButton(tr("Reset"), this);
    
    QHBoxLayout* urlLayout = new QHBoxLayout();
    urlLayout->addWidget(urlEdit);
    urlLayout->addWidget(resetButton);
    
    // LLM model combo (editable)
    QLabel* modelLabel = new QLabel(tr("LLM Model:"), this);
    llmModelCombo = new QComboBox(this);
    llmModelCombo->setEditable(true);
    
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
    mainLayout->addWidget(urlLabel);
    mainLayout->addLayout(urlLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(modelLabel);
    mainLayout->addLayout(modelLayout);
    mainLayout->addWidget(modelHelpLabel);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
    
    // signals
    QObject::connect(resetButton, &QPushButton::clicked, this, &OllamaConfigDialog::handleReset);
    QObject::connect(refreshModelsButton, &QPushButton::clicked, this, &OllamaConfigDialog::handleRefresh);
    QObject::connect(probeButton, &QPushButton::clicked, this, &OllamaConfigDialog::handleProbe);
    QObject::connect(addButton, &QPushButton::clicked, this, &OllamaConfigDialog::handleAdd);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    // dialog
    setWindowTitle(tr("Configure ollama Provider"));
    resize(fontMetrics().averageCharWidth()*60, height());
    setModal(true);
}

OllamaConfigDialog::~OllamaConfigDialog()
{
}

void OllamaConfigDialog::show()
{
    urlEdit->setText(DEFAULT_OLLAMA_URL);
    
    llmModelCombo->clear();
    configValid = false;
    
    QDialog::show();
}

void OllamaConfigDialog::handleReset()
{
    urlEdit->setText(DEFAULT_OLLAMA_URL);
    llmModelCombo->clear();
}

void OllamaConfigDialog::handleRefresh()
{
    string url = urlEdit->text().toStdString();
    if (url.empty()) {
        QMessageBox::warning(
            this,
            tr("URL Required"),
            tr("Please enter the ollama server URL."));
        return;
    }
    
    try {
        OllamaWingman wingman(url);
        vector<string>& models = wingman.listModels();
        
        llmModelCombo->clear();
        for (const auto& model : models) {
            llmModelCombo->addItem(QString::fromStdString(model));
        }
        
        if (models.empty()) {
            QMessageBox::warning(
                this,
                tr("No Models Found"),
                tr("No models found on ollama server. Please ensure ollama is running and has models installed."));
        } else {
            QMessageBox::information(
                this,
                tr("Models Refreshed"),
                tr("Successfully fetched %1 models from ollama server.").arg(models.size()));
        }
    } catch (const exception& e) {
        QMessageBox::critical(
            this,
            tr("Refresh Failed"),
            tr("Failed to fetch models from ollama server: %1").arg(e.what()));
    }
}

void OllamaConfigDialog::handleProbe()
{
    string url = urlEdit->text().toStdString();
    string model = llmModelCombo->currentText().toStdString();
    string errorMessage;
    
    if (config.probeOllamaProvider(url, model, errorMessage)) {
        QMessageBox::information(
            this,
            tr("Configuration Valid"),
            tr("ollama provider configuration is valid."));
        configValid = true;
    } else {
        QMessageBox::critical(
            this,
            tr("Configuration Invalid"),
            tr("ollama provider configuration is invalid: %1")
            .arg(QString::fromStdString(errorMessage)));
        configValid = false;
    }
}

void OllamaConfigDialog::handleAdd()
{
    string url = urlEdit->text().toStdString();
    string model = llmModelCombo->currentText().toStdString();
    
    // validate inputs
    if (url.empty()) {
        QMessageBox::warning(
            this,
            tr("URL Required"),
            tr("Please enter the ollama server URL."));
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
    
    // extract host from URL for display name
    string host = url;
    size_t pos = url.find("://");
    if (pos != string::npos) {
        host = url.substr(pos + 3);
    }
    
    providerConfig.id = "ollama-" + to_string(timestamp);
    providerConfig.displayName = "ollama " + model + " @ " + host;
    providerConfig.providerType = WINGMAN_PROVIDER_OLLAMA;
    providerConfig.url = url;
    providerConfig.llmModel = model;
    providerConfig.isValid = configValid;
    
    accept();
}

}
