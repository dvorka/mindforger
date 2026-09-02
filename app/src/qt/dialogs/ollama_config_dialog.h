/*
 ollama_config_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_OLLAMA_CONFIG_DIALOG_H
#define M8RUI_OLLAMA_CONFIG_DIALOG_H

#include <QtWidgets>
#include <chrono>

#include "../../lib/src/config/configuration.h"
#include "../../lib/src/mind/ai/llm/ollama_wingman.h"

namespace m8r {

class OllamaConfigDialog : public QDialog
{
    Q_OBJECT

private:
    Configuration& config;

    QLineEdit* urlEdit;
    QPushButton* resetButton;
    QComboBox* llmModelCombo;
    QPushButton* refreshModelsButton;
    QPushButton* probeButton;
    QPushButton* addButton;
    QPushButton* cancelButton;

    LlmProviderConfig providerConfig;
    bool configValid;
    bool editMode;

public:
    explicit OllamaConfigDialog(QWidget* parent);
    OllamaConfigDialog(const OllamaConfigDialog&) = delete;
    OllamaConfigDialog(const OllamaConfigDialog&&) = delete;
    OllamaConfigDialog& operator=(const OllamaConfigDialog&) = delete;
    OllamaConfigDialog& operator=(const OllamaConfigDialog&&) = delete;
    ~OllamaConfigDialog();

    void setEditProvider(const LlmProviderConfig& provider);

    const LlmProviderConfig& getProviderConfig() const { return providerConfig; }
    bool isConfigValid() const { return configValid; }

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void handleReset();
    void handleRefresh();
    void handleProbe();
    void handleAdd();
};

}
#endif // M8RUI_OLLAMA_CONFIG_DIALOG_H
