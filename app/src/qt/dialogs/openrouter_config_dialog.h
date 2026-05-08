/*
 openrouter_config_dialog.h     MindForger thinking notebook

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
#ifndef M8RUI_OPENROUTER_CONFIG_DIALOG_H
#define M8RUI_OPENROUTER_CONFIG_DIALOG_H

#include <QtWidgets>
#include <chrono>

#include "../../lib/src/config/configuration.h"
#include "../../lib/src/mind/ai/llm/openrouter_wingman.h"

namespace m8r {

class OpenRouterConfigDialog : public QDialog
{
    Q_OBJECT

private:
    Configuration& config;

    QLineEdit* apiKeyEdit;
    QCheckBox* useEnvVarCheckbox;
    QPushButton* resetButton;
    QLabel* envVarInfoLabel;
    QComboBox* llmModelCombo;
    QPushButton* probeButton;
    QPushButton* addButton;
    QPushButton* cancelButton;

    LlmProviderConfig providerConfig;
    bool configValid;

public:
    explicit OpenRouterConfigDialog(QWidget* parent);
    OpenRouterConfigDialog(const OpenRouterConfigDialog&) = delete;
    OpenRouterConfigDialog(const OpenRouterConfigDialog&&) = delete;
    OpenRouterConfigDialog& operator=(const OpenRouterConfigDialog&) = delete;
    OpenRouterConfigDialog& operator=(const OpenRouterConfigDialog&&) = delete;
    ~OpenRouterConfigDialog();

    const LlmProviderConfig& getProviderConfig() const { return providerConfig; }
    bool isConfigValid() const { return configValid; }

    void show();

private slots:
    void handleEnvVarCheckbox(int state);
    void handleReset();
    void handleProbe();
    void handleAdd();
};

}
#endif // M8RUI_OPENROUTER_CONFIG_DIALOG_H
